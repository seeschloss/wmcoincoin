/* libxwl core: display, atoms, drawables, GC, drawing, windows, events. */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <cairo.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include <linux/input-event-codes.h>
#include "xdg-shell-client-protocol.h"
#include "xwl_priv.h"
#include "xwl.h"

/* ------------------------------------------------------------------ */
/* helpers                                                            */
/* ------------------------------------------------------------------ */

static XwlDisplay *XD(Display *dpy) { return (XwlDisplay *)dpy; }
int xwl_debug_on(void) { static int v = -1; if (v < 0) v = getenv("XWL_DEBUG") != NULL; return v; }
#define DBG(...) do { if (xwl_debug_on()) fprintf(stderr, "xwl: " __VA_ARGS__); } while (0)

XwlDrawable *xwl_lookup(Drawable d) { return (XwlDrawable *)(uintptr_t)d; }

static void set_color(cairo_t *cr, unsigned long px)
{
  cairo_set_source_rgb(cr, ((px >> 16) & 0xff) / 255.0,
                           ((px >> 8) & 0xff) / 255.0,
                           (px & 0xff) / 255.0);
}

/* ------------------------------------------------------------------ */
/* event queue                                                        */
/* ------------------------------------------------------------------ */


XEvent *xwl_push_event(XwlDisplay *d, int type, Window win)
{
  EvNode *n = calloc(1, sizeof *n);
  n->ev.type = type;
  n->ev.xany.serial = ++d->serial;
  n->ev.xany.display = (Display *)d;
  n->ev.xany.window = win;
  if (d->evtail) d->evtail->next = n; else d->evhead = n;
  d->evtail = n;
  d->evcount++;
  return &n->ev;
}

static int pop_event(XwlDisplay *d, XEvent *out)
{
  EvNode *n = d->evhead;
  if (!n) return 0;
  d->evhead = n->next;
  if (!d->evhead) d->evtail = NULL;
  d->evcount--;
  *out = n->ev;
  free(n);
  return 1;
}

/* Does window `w` want events of this kind? */
int xwl_wants(Window w, long mask)
{
  XwlDrawable *dr = xwl_lookup(w);
  return dr && dr->kind == XWL_WINDOW && (dr->event_mask & mask);
}

/* ------------------------------------------------------------------ */
/* buffers (wl_shm)                                                   */
/* ------------------------------------------------------------------ */

static void buffer_release(void *data, struct wl_buffer *b)
{
  XwlBuf *xb = data;
  (void)b;
  xb->busy = 0;
}
static const struct wl_buffer_listener buffer_listener = { buffer_release };

static void free_buffers_all(XwlDrawable *w)
{
  for (int i = 0; i < 2; i++)
    if (w->bufs[i].buf) wl_buffer_destroy(w->bufs[i].buf);
  if (w->shm_map) munmap(w->shm_map, w->shm_total);
  w->shm_map = NULL;
  memset(w->bufs, 0, sizeof w->bufs);
}

static int alloc_buffers(XwlDisplay *d, XwlDrawable *w)
{
  free_buffers_all(w);
  int stride = w->w * 4;
  size_t one = (size_t)stride * w->h;
  size_t total = one * 2;
  int fd = memfd_create("xwl-shm", MFD_CLOEXEC);
  if (fd < 0 || ftruncate(fd, total) < 0) return -1;
  void *map = mmap(NULL, total, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) { close(fd); return -1; }
  struct wl_shm_pool *pool = wl_shm_create_pool(d->shm, fd, total);
  for (int i = 0; i < 2; i++) {
    XwlBuf *b = &w->bufs[i];
    b->data = (char *)map + i * one;
    b->size = one;
    b->w = w->w; b->h = w->h;
    b->buf = wl_shm_pool_create_buffer(pool, i * one, w->w, w->h, stride,
                                       w->argb ? WL_SHM_FORMAT_ARGB8888 : WL_SHM_FORMAT_XRGB8888);
    wl_buffer_add_listener(b->buf, &buffer_listener, b);
  }
  wl_shm_pool_destroy(pool);
  close(fd);
  w->shm_map = map; w->shm_total = total;
  return 0;
}

static void commit_window(XwlDisplay *d, XwlDrawable *w)
{
  if (!w->mapped || !w->configured || !w->dirty) return;
  XwlBuf *b = NULL;
  for (int i = 0; i < 2; i++) if (w->bufs[i].buf && !w->bufs[i].busy) { b = &w->bufs[i]; break; }
  if (!b) return;                       /* both busy: retry on next pump */
  cairo_surface_flush(w->surf);
  memcpy(b->data, cairo_image_surface_get_data(w->surf), (size_t)w->w * 4 * w->h);
  if (w->argb) {                                   /* XShape: alpha from the mask, premultiplied */
    uint32_t *px = b->data;
    int mw = w->shape ? cairo_image_surface_get_width(w->shape) : 0;
    int mh = w->shape ? cairo_image_surface_get_height(w->shape) : 0;
    int ms = w->shape ? cairo_image_surface_get_stride(w->shape) : 0;
    unsigned char *mp = w->shape ? cairo_image_surface_get_data(w->shape) : NULL;
    for (int y = 0; y < w->h; y++)
      for (int x = 0; x < w->w; x++) {
        int in = !mp || (x < mw && y < mh && mp[y * ms + x] >= 128);
        px[y * w->w + x] = in ? (px[y * w->w + x] | 0xff000000u) : 0;
      }
  }
  wl_surface_attach(w->wls, b->buf, 0, 0);
  wl_surface_damage_buffer(w->wls, 0, 0, w->w, w->h);
  wl_surface_commit(w->wls);
  b->busy = 1;
  w->dirty = 0;
  (void)d;
}

static void commit_all(XwlDisplay *d)
{
  for (XwlDrawable *w = d->windows; w; w = w->next_win) commit_window(d, w);
}

/* resize the backing store of a window (contents are cleared) */
static void window_set_size(XwlDisplay *d, XwlDrawable *w, int nw, int nh)
{
  if (nw < 1) nw = 1;
  if (nh < 1) nh = 1;
  if (w->surf && nw == w->w && nh == w->h) return;
  if (w->surf) cairo_surface_destroy(w->surf);
  w->w = nw; w->h = nh;
  w->surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, nw, nh);
  { cairo_t *cr = cairo_create(w->surf); set_color(cr, w->bg_pixel);
    cairo_paint(cr); cairo_destroy(cr); }
  if (w->mapped) { free_buffers_all(w); alloc_buffers(d, w); }
  w->dirty = 1;
}

/* ------------------------------------------------------------------ */
/* xdg-shell listeners                                                */
/* ------------------------------------------------------------------ */

static void wm_base_ping(void *data, struct xdg_wm_base *b, uint32_t serial)
{ (void)data; xdg_wm_base_pong(b, serial); }
static const struct xdg_wm_base_listener wm_base_listener = { wm_base_ping };

static void xdg_surface_configure(void *data, struct xdg_surface *xs, uint32_t serial)
{
  XwlDrawable *w = data;
  XwlDisplay *d = w->dpy;
  xdg_surface_ack_configure(xs, serial);
  DBG("configure win=%p title='%s' pending=%dx%d current=%dx%d\n", (void *)w, w->title ? w->title : "", w->pending_w, w->pending_h, w->w, w->h);
  if (w->pending_w > 0 && w->pending_h > 0 &&
      (w->pending_w != w->w || w->pending_h != w->h))
    window_set_size(d, w, w->pending_w, w->pending_h);
  int first = !w->configured;
  w->configured = 1;
  if (first && !w->bufs[0].buf) alloc_buffers(d, w);
  w->dirty = 1;
  XEvent *e = xwl_push_event(d, ConfigureNotify, (Window)(uintptr_t)w);
  e->xconfigure.window = e->xconfigure.event;
  e->xconfigure.x = 0; e->xconfigure.y = 0;
  e->xconfigure.width = w->w; e->xconfigure.height = w->h;
  e = xwl_push_event(d, Expose, (Window)(uintptr_t)w);
  e->xexpose.x = 0; e->xexpose.y = 0;
  e->xexpose.width = w->w; e->xexpose.height = w->h; e->xexpose.count = 0;
}
static const struct xdg_surface_listener xdg_surface_listener = { xdg_surface_configure };

static void top_configure(void *data, struct xdg_toplevel *t, int32_t w_, int32_t h_,
                          struct wl_array *states)
{
  XwlDrawable *w = data;
  (void)t; (void)states;
  w->pending_w = w_; w->pending_h = h_;   /* applied at xdg_surface.configure */
}
static void top_close(void *data, struct xdg_toplevel *t)
{
  XwlDrawable *w = data;
  XwlDisplay *d = w->dpy;
  (void)t;
  XEvent *e = xwl_push_event(d, ClientMessage, (Window)(uintptr_t)w);
  e->xclient.message_type = XInternAtom((Display *)d, "WM_PROTOCOLS", False);
  e->xclient.format = 32;
  e->xclient.data.l[0] = XInternAtom((Display *)d, "WM_DELETE_WINDOW", False);
  e->xclient.data.l[1] = CurrentTime;
}
static void top_configure_bounds(void *d, struct xdg_toplevel *t, int32_t w, int32_t h)
{ (void)d; (void)t; (void)w; (void)h; }
static void top_wm_caps(void *d, struct xdg_toplevel *t, struct wl_array *a)
{ (void)d; (void)t; (void)a; }
static const struct xdg_toplevel_listener toplevel_listener = {
  top_configure, top_close, top_configure_bounds, top_wm_caps
};

/* ------------------------------------------------------------------ */
/* registry / output                                                  */
/* ------------------------------------------------------------------ */

static void out_geometry(void *d, struct wl_output *o, int32_t x, int32_t y, int32_t pw,
                         int32_t ph, int32_t sp, const char *mk, const char *md, int32_t tr)
{ (void)d;(void)o;(void)x;(void)y;(void)pw;(void)ph;(void)sp;(void)mk;(void)md;(void)tr; }
static void out_mode(void *data, struct wl_output *o, uint32_t flags, int32_t w, int32_t h, int32_t r)
{
  XwlDisplay *d = data;
  (void)o; (void)r;
  if (flags & WL_OUTPUT_MODE_CURRENT) { d->screen.width = w; d->screen.height = h; }
}
static void out_done(void *d, struct wl_output *o) { (void)d; (void)o; }
static void out_scale(void *d, struct wl_output *o, int32_t s) { (void)d; (void)o; (void)s; }
static void out_name(void *d, struct wl_output *o, const char *n) { (void)d; (void)o; (void)n; }
static void out_desc(void *d, struct wl_output *o, const char *n) { (void)d; (void)o; (void)n; }
static const struct wl_output_listener output_listener = {
  out_geometry, out_mode, out_done, out_scale, out_name, out_desc
};

static void reg_global(void *data, struct wl_registry *reg, uint32_t name,
                       const char *iface, uint32_t ver)
{
  XwlDisplay *d = data;
  if (!strcmp(iface, "wl_compositor"))
    d->compositor = wl_registry_bind(reg, name, &wl_compositor_interface, ver < 4 ? ver : 4);
  else if (!strcmp(iface, "wl_shm"))
    d->shm = wl_registry_bind(reg, name, &wl_shm_interface, 1);
  else if (!strcmp(iface, "xdg_wm_base")) {
    d->wm_base = wl_registry_bind(reg, name, &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener(d->wm_base, &wm_base_listener, d);
  } else if (!strcmp(iface, "wl_seat")) {
    d->seat = wl_registry_bind(reg, name, &wl_seat_interface, ver < 5 ? ver : 5);
    d->seat_name = name;
    xwl_input_seat_bound(d);
  } else if (!strcmp(iface, "wl_output") && !d->output) {
    d->output = wl_registry_bind(reg, name, &wl_output_interface, ver < 2 ? ver : 2);
    wl_output_add_listener(d->output, &output_listener, d);
  } else
    xwl_selection_global(d, reg, name, iface, ver);
}
static void reg_remove(void *d, struct wl_registry *r, uint32_t n) { (void)d; (void)r; (void)n; }
static const struct wl_registry_listener registry_listener = { reg_global, reg_remove };

/* ------------------------------------------------------------------ */
/* Display                                                            */
/* ------------------------------------------------------------------ */

Display *XOpenDisplay(_Xconst char *name)
{
  XwlDisplay *d = calloc(1, sizeof *d);
  /* `name` is an *X* display string (applications pass getenv("DISPLAY"), e.g. ":1"), which
   * means nothing for Wayland: always connect through WAYLAND_DISPLAY, or XWL_DISPLAY if set. */
  const char *wname = getenv("XWL_DISPLAY");
  (void)name;
  d->wl = wl_display_connect(wname);
  if (!d->wl) {
    fprintf(stderr, "xwl: cannot connect to the Wayland compositor (WAYLAND_DISPLAY=%s, XDG_RUNTIME_DIR=%s): %s\n",
            wname ? wname : (getenv("WAYLAND_DISPLAY") ? getenv("WAYLAND_DISPLAY") : "(unset)"),
            getenv("XDG_RUNTIME_DIR") ? getenv("XDG_RUNTIME_DIR") : "(unset)", strerror(errno));
    free(d); return NULL;
  }

  d->screen.width = 1920; d->screen.height = 1080;
  d->visual.class = TrueColor;
  d->visual.visualid = 0x21;
  d->visual.red_mask = 0xff0000; d->visual.green_mask = 0x00ff00; d->visual.blue_mask = 0x0000ff;
  d->visual.bits_per_rgb = 8; d->visual.map_entries = 256;

  d->pub.fd = wl_display_get_fd(d->wl);
  d->pub.default_screen = 0; d->pub.nscreens = 1;
  d->pub.screens = &d->screen;
  d->pub.display_name = strdup(getenv("WAYLAND_DISPLAY") ? getenv("WAYLAND_DISPLAY") : "wayland");
  d->pub.vendor = "xwl"; d->pub.byte_order = LSBFirst; d->pub.bitmap_unit = 32;
  d->pub.bitmap_pad = 32; d->pub.bitmap_bit_order = LSBFirst;
  d->pub.proto_major_version = 11;

  d->root.kind = XWL_ROOT; d->root.dpy = d; d->root.depth = 24;
  d->screen.root = (Window)(uintptr_t)&d->root;
  d->screen.root_visual = &d->visual;
  d->screen.root_depth = 24; d->screen.cmap = 1;
  d->screen.white_pixel = 0xffffff; d->screen.black_pixel = 0;
  d->screen.display = (Display *)d;
  d->screen.ndepths = 0;

  d->registry = wl_display_get_registry(d->wl);
  wl_registry_add_listener(d->registry, &registry_listener, d);
  wl_display_roundtrip(d->wl);
  wl_display_roundtrip(d->wl);
  if (!d->compositor || !d->shm || !d->wm_base) {
    fprintf(stderr, "xwl: compositor is missing required globals (wl_compositor=%s wl_shm=%s xdg_wm_base=%s)\n",
            d->compositor ? "ok" : "MISSING", d->shm ? "ok" : "MISSING", d->wm_base ? "ok" : "MISSING");
    return NULL;
  }
  d->screen.mwidth = d->screen.width * 254 / 960;
  d->screen.mheight = d->screen.height * 254 / 960;
  d->root.w = d->screen.width; d->root.h = d->screen.height;
  xwl_input_init(d);
  xwl_selection_init(d);
  return (Display *)d;
}

int XCloseDisplay(Display *dpy)
{
  XwlDisplay *d = XD(dpy);
  wl_display_disconnect(d->wl);
  return 0;
}

int xwl_fd(Display *dpy) { return wl_display_get_fd(XD(dpy)->wl); }

Window xwl_dummy_root(Display *dpy) { return XD(dpy)->screen.root; }

/* ------------------------------------------------------------------ */
/* Atoms                                                              */
/* ------------------------------------------------------------------ */

Atom XInternAtom(Display *dpy, _Xconst char *name, Bool only_if_exists)
{
  XwlDisplay *d = XD(dpy);
  for (int i = 0; i < d->natoms; i++)
    if (!strcmp(d->atoms[i], name)) return (Atom)(1000 + i);
  /* predefined atoms get their real X values */
  static const struct { const char *n; Atom a; } pre[] = {
    {"PRIMARY", XA_PRIMARY}, {"SECONDARY", XA_SECONDARY}, {"STRING", XA_STRING},
    {"ATOM", XA_ATOM}, {"CARDINAL", XA_CARDINAL}, {"WM_NAME", XA_WM_NAME},
    {"WM_ICON_NAME", XA_WM_ICON_NAME}, {"WM_CLASS", XA_WM_CLASS},
    {"WM_HINTS", XA_WM_HINTS}, {"WM_NORMAL_HINTS", XA_WM_NORMAL_HINTS},
    {"INTEGER", XA_INTEGER}, {"WINDOW", XA_WINDOW}, {"CUT_BUFFER0", XA_CUT_BUFFER0},
  };
  for (unsigned i = 0; i < sizeof pre / sizeof *pre; i++)
    if (!strcmp(pre[i].n, name)) return pre[i].a;
  if (only_if_exists) return None;
  d->atoms = realloc(d->atoms, sizeof(char *) * (d->natoms + 1));
  d->atoms[d->natoms] = strdup(name);
  /* custom atoms live above the predefined range */
  return (Atom)(1000 + d->natoms++);
}

/* custom atoms are 1000+index; fix lookup accordingly */
const char *xwl_atom_name(XwlDisplay *d, Atom a);
static const char *atom_name_of(XwlDisplay *d, Atom a)
{
  if (a >= 1000 && (int)(a - 1000) < d->natoms) return d->atoms[a - 1000];
  return NULL;
}

Status XInternAtoms(Display *dpy, char **names, int count, Bool oie, Atom *out)
{
  for (int i = 0; i < count; i++) out[i] = XInternAtom(dpy, names[i], oie);
  return 1;
}

const char *xwl_atom_name(XwlDisplay *d, Atom a) { return atom_name_of(d, a); }

char *XGetAtomName(Display *dpy, Atom a)
{
  const char *n = atom_name_of(XD(dpy), a);
  return n ? strdup(n) : NULL;
}

/* ------------------------------------------------------------------ */
/* window lifecycle                                                   */
/* ------------------------------------------------------------------ */
extern void xwl_apply_title(XwlDrawable *w);
extern void xwl_apply_hints(XwlDrawable *w);

void xwl_window_init_size(XwlDisplay *d, XwlDrawable *w, int nw, int nh)
{ window_set_size(d, w, nw, nh); }

void xwl_window_realize(XwlDisplay *d, XwlDrawable *w)
{
  w->wls = wl_compositor_create_surface(d->compositor);
  wl_surface_set_user_data(w->wls, w);
  w->xs = xdg_wm_base_get_xdg_surface(d->wm_base, w->wls);
  xdg_surface_add_listener(w->xs, &xdg_surface_listener, w);
  w->top = xdg_surface_get_toplevel(w->xs);
  xdg_toplevel_add_listener(w->top, &toplevel_listener, w);
  xwl_apply_title(w);
  xwl_apply_hints(w);
  if (w->transient_for) {
    XwlDrawable *p = xwl_lookup(w->transient_for);
    if (p && p->top) xdg_toplevel_set_parent(w->top, p->top);
  }
  w->mapped = 1; w->configured = 0;
  wl_surface_commit(w->wls);            /* initial commit -> triggers configure */
}

void xwl_window_unrealize(XwlDrawable *w)
{
  if (!w->mapped) return;
  free_buffers_all(w);
  if (w->top) xdg_toplevel_destroy(w->top);
  if (w->xs) xdg_surface_destroy(w->xs);
  if (w->wls) wl_surface_destroy(w->wls);
  w->top = NULL; w->xs = NULL; w->wls = NULL;
  w->mapped = w->configured = 0;
}

void xwl_window_client_resize(XwlDisplay *d, XwlDrawable *w, int nw, int nh)
{
  DBG("client_resize win=%p '%s' %dx%d -> %dx%d\n", (void *)w, w->title ? w->title : "", w->w, w->h, nw, nh);
  if (nw == w->w && nh == w->h) return;
  window_set_size(d, w, nw, nh);
  if (w->mapped && w->configured) {
    XEvent *e = xwl_push_event(d, ConfigureNotify, (Window)(uintptr_t)w);
    e->xconfigure.window = e->xconfigure.event;
    e->xconfigure.width = w->w; e->xconfigure.height = w->h;
    e = xwl_push_event(d, Expose, (Window)(uintptr_t)w);
    e->xexpose.width = w->w; e->xexpose.height = w->h;
  }
}

/* ------------------------------------------------------------------ */
/* event pump + Xlib event API                                        */
/* ------------------------------------------------------------------ */

/* timeout_ms: 0 = poll, -1 = block */
static void pump(XwlDisplay *d, int timeout_ms)
{
  commit_all(d);
  while (wl_display_prepare_read(d->wl) != 0) wl_display_dispatch_pending(d->wl);
  wl_display_flush(d->wl);
  struct pollfd p = { wl_display_get_fd(d->wl), POLLIN, 0 };
  int r = poll(&p, 1, timeout_ms);
  if (r > 0 && (p.revents & POLLIN)) wl_display_read_events(d->wl);
  else wl_display_cancel_read(d->wl);
  wl_display_dispatch_pending(d->wl);
  xwl_input_pump(d);
  xwl_sel_pump(d);
  commit_all(d);
  wl_display_flush(d->wl);
}

int XPending(Display *dpy)
{
  XwlDisplay *d = XD(dpy);
  if (!d->evcount) pump(d, 0);
  return d->evcount;
}
int XNextEvent(Display *dpy, XEvent *ev)
{
  XwlDisplay *d = XD(dpy);
  while (!pop_event(d, ev)) pump(d, (d->rep_active || xwl_sel_busy(d)) ? 10 : -1);
  return 0;
}
int XPeekEvent(Display *dpy, XEvent *ev)
{
  XwlDisplay *d = XD(dpy);
  while (!d->evhead) pump(d, (d->rep_active || xwl_sel_busy(d)) ? 10 : -1);
  *ev = d->evhead->ev;
  return 0;
}
Bool XCheckIfEvent(Display *dpy, XEvent *ev, Bool (*pred)(Display *, XEvent *, XPointer), XPointer arg)
{
  XwlDisplay *d = XD(dpy);
  if (!d->evcount) pump(d, 0);
  for (EvNode **pp = (EvNode **)&d->evhead; *pp; pp = &(*pp)->next) {
    EvNode *n = *pp;
    if (pred(dpy, &n->ev, arg)) {
      *pp = n->next;
      if (d->evtail == n) {
        d->evtail = NULL;
        for (EvNode *t = d->evhead; t; t = t->next) d->evtail = t;
      }
      d->evcount--; *ev = n->ev; free(n); return True;
    }
  }
  return False;
}
static Bool pred_typed(Display *dpy, XEvent *e, XPointer a)
{ (void)dpy; long *v = (long *)a; return e->type == (int)v[0] && e->xany.window == (Window)v[1]; }
Bool XCheckTypedWindowEvent(Display *dpy, Window w, int type, XEvent *ev)
{ long v[2] = { type, (long)w }; return XCheckIfEvent(dpy, ev, pred_typed, (XPointer)v); }

int XFlush(Display *dpy) { pump(XD(dpy), 0); return 0; }
int XSync(Display *dpy, Bool discard)
{ XwlDisplay *d = XD(dpy); pump(d, 0); wl_display_roundtrip(d->wl); (void)discard; return 0; }

Status XSendEvent(Display *dpy, Window w, Bool prop, long mask, XEvent *ev)
{
  (void)prop; (void)mask;
  XwlDisplay *d = XD(dpy);
  if (xwl_sel_intercept(d, w, ev)) return 1;
  XEvent *e = xwl_push_event(d, ev->type, w);
  unsigned long s = e->xany.serial;
  *e = *ev; e->xany.serial = s; e->xany.send_event = True; e->xany.display = dpy;
  return 1;
}
Bool XFilterEvent(XEvent *ev, Window w) { (void)ev; (void)w; return False; }

/* switch a window between XRGB and ARGB buffers (needed for XShape) */
void xwl_window_set_argb(XwlDisplay *d, XwlDrawable *w, int argb)
{
  if (w->argb == argb) { w->dirty = 1; return; }
  w->argb = argb;
  if (w->mapped && w->bufs[0].buf) { free_buffers_all(w); alloc_buffers(d, w); }
  w->dirty = 1;
}
