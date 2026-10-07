/* libxwl: emulation of the X selection protocol (PRIMARY / CLIPBOARD) on top
 * of wl_data_device and zwp_primary_selection_v1.
 *
 * Owner side:  XSetSelectionOwner() creates a Wayland data source.  When a
 *   Wayland client asks for the data we synthesize an X SelectionRequest for
 *   the owning window; the application answers with XChangeProperty() +
 *   XSendEvent(SelectionNotify) as it would on X, and we intercept that reply
 *   and stream the property to the requesting pipe.
 * Requestor side: XConvertSelection() receives from the current offer into a
 *   pipe that is drained by xwl_sel_pump(); the data ends up in a window
 *   property and a SelectionNotify event is queued.
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <wayland-client.h>
#include "primary-selection-client-protocol.h"
#include "xwl_priv.h"

enum { CLIP = 0, PRIM = 1 };
#define MAX_XFER (4 * 1024 * 1024)

typedef struct Offer {
  int which;
  void *proxy;                      /* wl_data_offer or zwp_primary_selection_offer_v1 */
  char **mimes; int n;
} Offer;

typedef struct Xfer {
  struct Xfer *next;
  int fd; char *buf; size_t len, cap;
  Window requestor; Atom selection, target, property;
} Xfer;

typedef struct SendReq { struct SendReq *next; int fd; } SendReq;

typedef struct Sel {
  struct wl_data_device_manager *ddm; struct wl_data_device *dd;
  struct zwp_primary_selection_device_manager_v1 *psm; struct zwp_primary_selection_device_v1 *pd;
  Offer *offer[2];
  struct wl_data_source *clip_src;
  struct zwp_primary_selection_source_v1 *prim_src;
  Window owner[2];
  XwlDrawable req;                  /* pseudo-window receiving the owner's replies */
  Xfer *xfers; SendReq *sends;
  Atom a_clipboard, a_utf8, a_prop;
  int no_shortcut;
} Sel;

static Sel *SEL(XwlDisplay *d)
{
  if (!d->sel) d->sel = calloc(1, sizeof(Sel));
  return d->sel;
}

static const char *const our_mimes[] = { "text/plain;charset=utf-8", "UTF8_STRING", "text/plain" };

/* ------------------------------------------------------------------ */
/* offers                                                             */
/* ------------------------------------------------------------------ */
static void offer_free(Offer *o)
{
  if (!o) return;
  if (o->which == CLIP) wl_data_offer_destroy(o->proxy);
  else zwp_primary_selection_offer_v1_destroy(o->proxy);
  for (int i = 0; i < o->n; i++) free(o->mimes[i]);
  free(o->mimes); free(o);
}
static void offer_add_mime(Offer *o, const char *mime)
{
  o->mimes = realloc(o->mimes, sizeof(char *) * (o->n + 1));
  o->mimes[o->n++] = strdup(mime);
}
static void wl_offer_offer(void *d, struct wl_data_offer *of, const char *m) { (void)of; offer_add_mime(d, m); }
static void wl_offer_src_actions(void *d, struct wl_data_offer *o, uint32_t a) { (void)d; (void)o; (void)a; }
static void wl_offer_action(void *d, struct wl_data_offer *o, uint32_t a) { (void)d; (void)o; (void)a; }
static const struct wl_data_offer_listener wl_offer_listener = { wl_offer_offer, wl_offer_src_actions, wl_offer_action };
static void pr_offer_offer(void *d, struct zwp_primary_selection_offer_v1 *of, const char *m) { (void)of; offer_add_mime(d, m); }
static const struct zwp_primary_selection_offer_v1_listener pr_offer_listener = { pr_offer_offer };

static const char *pick_mime(Offer *o)
{
  static const char *pref[] = { "text/plain;charset=utf-8", "UTF8_STRING", "text/plain", "TEXT", "STRING" };
  for (unsigned p = 0; p < sizeof pref / sizeof *pref; p++)
    for (int i = 0; i < o->n; i++) if (!strcmp(o->mimes[i], pref[p])) return o->mimes[i];
  return NULL;
}

/* ---- wl_data_device ---- */
static void dd_data_offer(void *data, struct wl_data_device *dd, struct wl_data_offer *id)
{
  (void)data; (void)dd;
  Offer *o = calloc(1, sizeof *o); o->which = CLIP; o->proxy = id;
  wl_data_offer_add_listener(id, &wl_offer_listener, o);
}
static void dd_enter(void *data, struct wl_data_device *dd, uint32_t s, struct wl_surface *surf,
                     wl_fixed_t x, wl_fixed_t y, struct wl_data_offer *id)
{ (void)data; (void)dd; (void)s; (void)surf; (void)x; (void)y; (void)id; /* DnD unsupported */ }
static void dd_leave(void *d, struct wl_data_device *dd) { (void)d; (void)dd; }
static void dd_motion(void *d, struct wl_data_device *dd, uint32_t t, wl_fixed_t x, wl_fixed_t y)
{ (void)d; (void)dd; (void)t; (void)x; (void)y; }
static void dd_drop(void *d, struct wl_data_device *dd) { (void)d; (void)dd; }
static void dd_selection(void *data, struct wl_data_device *dd, struct wl_data_offer *id)
{
  XwlDisplay *d = data; Sel *s = SEL(d); (void)dd;
  offer_free(s->offer[CLIP]);
  s->offer[CLIP] = id ? wl_data_offer_get_user_data(id) : NULL;
}
static const struct wl_data_device_listener dd_listener = {
  dd_data_offer, dd_enter, dd_leave, dd_motion, dd_drop, dd_selection
};

/* ---- primary selection device ---- */
static void pd_data_offer(void *data, struct zwp_primary_selection_device_v1 *pd,
                          struct zwp_primary_selection_offer_v1 *id)
{
  (void)data; (void)pd;
  Offer *o = calloc(1, sizeof *o); o->which = PRIM; o->proxy = id;
  zwp_primary_selection_offer_v1_add_listener(id, &pr_offer_listener, o);
}
static void pd_selection(void *data, struct zwp_primary_selection_device_v1 *pd,
                         struct zwp_primary_selection_offer_v1 *id)
{
  XwlDisplay *d = data; Sel *s = SEL(d); (void)pd;
  offer_free(s->offer[PRIM]);
  s->offer[PRIM] = id ? zwp_primary_selection_offer_v1_get_user_data(id) : NULL;
}
static const struct zwp_primary_selection_device_v1_listener pd_listener = { pd_data_offer, pd_selection };

/* ------------------------------------------------------------------ */
/* registry hooks                                                     */
/* ------------------------------------------------------------------ */
void xwl_selection_global(XwlDisplay *d, struct wl_registry *reg, uint32_t name, const char *iface, uint32_t ver)
{
  Sel *s = SEL(d);
  if (!strcmp(iface, "wl_data_device_manager"))
    s->ddm = wl_registry_bind(reg, name, &wl_data_device_manager_interface, ver < 3 ? ver : 3);
  else if (!strcmp(iface, "zwp_primary_selection_device_manager_v1"))
    s->psm = wl_registry_bind(reg, name, &zwp_primary_selection_device_manager_v1_interface, 1);
}

void xwl_selection_init(XwlDisplay *d)
{
  Sel *s = SEL(d);
  s->req.kind = XWL_ROOT; s->req.dpy = d;
  s->a_clipboard = XInternAtom((Display *)d, "CLIPBOARD", False);
  s->a_utf8 = XInternAtom((Display *)d, "UTF8_STRING", False);
  s->a_prop = XInternAtom((Display *)d, "XWL_SELECTION", False);
  s->no_shortcut = getenv("XWL_SEL_NOSHORTCUT") != NULL;
  if (!d->seat) return;
  if (s->ddm) { s->dd = wl_data_device_manager_get_data_device(s->ddm, d->seat); wl_data_device_add_listener(s->dd, &dd_listener, d); }
  if (s->psm) { s->pd = zwp_primary_selection_device_manager_v1_get_device(s->psm, d->seat); zwp_primary_selection_device_v1_add_listener(s->pd, &pd_listener, d); }
  wl_display_roundtrip(d->wl);
}

/* ------------------------------------------------------------------ */
/* owner side                                                         */
/* ------------------------------------------------------------------ */
static void queue_request(XwlDisplay *d, Sel *s, int which, Window requestor, Atom target, Atom prop)
{
  XEvent *e = xwl_push_event(d, SelectionRequest, s->owner[which]);
  e->xselectionrequest.owner = s->owner[which];
  e->xselectionrequest.requestor = requestor;
  e->xselectionrequest.selection = which == PRIM ? XA_PRIMARY : s->a_clipboard;
  e->xselectionrequest.target = target;
  e->xselectionrequest.property = prop;
  e->xselectionrequest.time = CurrentTime;
}

static void handle_send(XwlDisplay *d, int which, int fd)
{
  Sel *s = SEL(d);
  if (!s->owner[which]) { close(fd); return; }
  SendReq *r = calloc(1, sizeof *r); r->fd = fd;
  SendReq **pp = &s->sends; while (*pp) pp = &(*pp)->next; *pp = r;     /* FIFO */
  queue_request(d, s, which, (Window)(uintptr_t)&s->req, s->a_utf8, s->a_prop);
}

static void clear_owner(XwlDisplay *d, int which, int notify)
{
  Sel *s = SEL(d);
  Window w = s->owner[which];
  s->owner[which] = 0;
  if (notify && w) {
    XEvent *e = xwl_push_event(d, SelectionClear, w);
    e->xselectionclear.selection = which == PRIM ? XA_PRIMARY : s->a_clipboard;
    e->xselectionclear.time = CurrentTime;
  }
}

static void ds_target(void *d, struct wl_data_source *s, const char *m) { (void)d; (void)s; (void)m; }
static void ds_send(void *data, struct wl_data_source *src, const char *mime, int32_t fd)
{ (void)src; (void)mime; handle_send(data, CLIP, fd); }
static void ds_cancelled(void *data, struct wl_data_source *src)
{
  XwlDisplay *d = data; Sel *s = SEL(d);
  if (s->clip_src == src) { s->clip_src = NULL; clear_owner(d, CLIP, 1); }
  wl_data_source_destroy(src);
}
static void ds_dnd_drop(void *d, struct wl_data_source *s) { (void)d; (void)s; }
static void ds_dnd_fin(void *d, struct wl_data_source *s) { (void)d; (void)s; }
static void ds_action(void *d, struct wl_data_source *s, uint32_t a) { (void)d; (void)s; (void)a; }
static const struct wl_data_source_listener ds_listener = { ds_target, ds_send, ds_cancelled, ds_dnd_drop, ds_dnd_fin, ds_action };

static void ps_send(void *data, struct zwp_primary_selection_source_v1 *src, const char *mime, int32_t fd)
{ (void)src; (void)mime; handle_send(data, PRIM, fd); }
static void ps_cancelled(void *data, struct zwp_primary_selection_source_v1 *src)
{
  XwlDisplay *d = data; Sel *s = SEL(d);
  if (s->prim_src == src) { s->prim_src = NULL; clear_owner(d, PRIM, 1); }
  zwp_primary_selection_source_v1_destroy(src);
}
static const struct zwp_primary_selection_source_v1_listener ps_listener = { ps_send, ps_cancelled };

static int which_of(Sel *s, Atom a)
{
  if (a == XA_PRIMARY) return PRIM;
  if (a == s->a_clipboard) return CLIP;
  return -1;
}

int XSetSelectionOwner(Display *dpy, Atom selection, Window w, Time t)
{
  XwlDisplay *d = (XwlDisplay *)dpy; Sel *s = SEL(d); (void)t;
  int which = which_of(s, selection);
  if (which < 0) return 0;
  int use_prim = (which == PRIM && s->psm && s->pd);
  if (!s->dd && !use_prim) return 0;
  int slot = use_prim ? PRIM : CLIP;     /* PRIMARY falls back to the clipboard device */

  /* drop previous source of the same device *without* notifying (same app re-owning) */
  if (use_prim && s->prim_src) { zwp_primary_selection_source_v1_destroy(s->prim_src); s->prim_src = NULL; }
  if (!use_prim && s->clip_src) { wl_data_source_destroy(s->clip_src); s->clip_src = NULL; }

  if (w == None) {
    if (use_prim) zwp_primary_selection_device_v1_set_selection(s->pd, NULL, d->last_serial);
    else wl_data_device_set_selection(s->dd, NULL, d->last_serial);
    s->owner[slot] = 0;
    return 0;
  }
  if (use_prim) {
    s->prim_src = zwp_primary_selection_device_manager_v1_create_source(s->psm);
    zwp_primary_selection_source_v1_add_listener(s->prim_src, &ps_listener, d);
    for (unsigned i = 0; i < 3; i++) zwp_primary_selection_source_v1_offer(s->prim_src, our_mimes[i]);
    zwp_primary_selection_device_v1_set_selection(s->pd, s->prim_src, d->last_serial);
  } else {
    s->clip_src = wl_data_device_manager_create_data_source(s->ddm);
    wl_data_source_add_listener(s->clip_src, &ds_listener, d);
    for (unsigned i = 0; i < 3; i++) wl_data_source_offer(s->clip_src, our_mimes[i]);
    wl_data_device_set_selection(s->dd, s->clip_src, d->last_serial);
  }
  s->owner[slot] = w;
  wl_display_flush(d->wl);
  return 0;
}

Window XGetSelectionOwner(Display *dpy, Atom selection)
{
  XwlDisplay *d = (XwlDisplay *)dpy; Sel *s = SEL(d);
  int which = which_of(s, selection);
  if (which < 0) return None;
  if (s->owner[which]) return s->owner[which];
  if (which == PRIM && !s->pd && s->owner[CLIP]) return s->owner[CLIP];
  if (s->offer[which] || (which == PRIM && s->offer[CLIP])) return d->screen.root;   /* a foreign owner exists */
  return None;
}

/* Intercept the owner's reply (XSendEvent(SelectionNotify) to our pseudo-window). */
int xwl_sel_intercept(XwlDisplay *d, Window w, XEvent *ev)
{
  Sel *s = SEL(d);
  if (w != (Window)(uintptr_t)&s->req || ev->type != SelectionNotify) return 0;
  SendReq *r = s->sends;
  if (!r) return 1;
  s->sends = r->next;
  if (ev->xselection.property != None) {
    Atom type; int fmt; unsigned long n, after; unsigned char *data = NULL;
    XGetWindowProperty((Display *)d, w, ev->xselection.property, 0, 0, True, AnyPropertyType,
                       &type, &fmt, &n, &after, &data);
    if (data && n) {
      size_t len = n * (fmt / 8), off = 0;
      while (off < len) {
        struct pollfd p = { r->fd, POLLOUT, 0 };
        if (poll(&p, 1, 2000) <= 0) break;
        ssize_t k = write(r->fd, data + off, len - off);
        if (k < 0) { if (errno == EINTR || errno == EAGAIN) continue; break; }
        off += k;
      }
    }
    XFree(data);
  }
  close(r->fd); free(r);
  return 1;
}

/* ------------------------------------------------------------------ */
/* requestor side                                                     */
/* ------------------------------------------------------------------ */
static void notify(XwlDisplay *d, Window req, Atom sel, Atom target, Atom prop)
{
  XEvent *e = xwl_push_event(d, SelectionNotify, req);
  e->xselection.requestor = req; e->xselection.selection = sel;
  e->xselection.target = target; e->xselection.property = prop; e->xselection.time = CurrentTime;
}

int XConvertSelection(Display *dpy, Atom selection, Atom target, Atom prop, Window requestor, Time t)
{
  XwlDisplay *d = (XwlDisplay *)dpy; Sel *s = SEL(d); (void)t;
  int which = which_of(s, selection);
  if (which < 0) { notify(d, requestor, selection, target, None); return 0; }
  int slot = (which == PRIM && !s->pd) ? CLIP : which;

  /* we own it ourselves: ask our own app directly, unless debugging the Wayland path */
  if (s->owner[slot] && !s->no_shortcut) {
    queue_request(d, s, slot, requestor, target, prop);
    return 0;
  }
  Offer *o = s->offer[slot];
  if (!o && which == PRIM) o = s->offer[CLIP];       /* PRIMARY empty -> use the clipboard */
  const char *mime = o ? pick_mime(o) : NULL;
  int fds[2];
  if (!o || !mime || pipe2(fds, O_CLOEXEC) < 0) { notify(d, requestor, selection, target, None); return 0; }
  if (o->which == CLIP) wl_data_offer_receive(o->proxy, mime, fds[1]);
  else zwp_primary_selection_offer_v1_receive(o->proxy, mime, fds[1]);
  close(fds[1]);
  fcntl(fds[0], F_SETFL, O_NONBLOCK);
  wl_display_flush(d->wl);
  Xfer *x = calloc(1, sizeof *x);
  x->fd = fds[0]; x->requestor = requestor; x->selection = selection; x->target = target; x->property = prop;
  x->next = s->xfers; s->xfers = x;
  return 0;
}

int xwl_sel_busy(XwlDisplay *d) { return d->sel && ((Sel *)d->sel)->xfers != NULL; }

void xwl_sel_pump(XwlDisplay *d)
{
  Sel *s = d->sel; if (!s) return;
  for (Xfer **pp = &s->xfers; *pp; ) {
    Xfer *x = *pp; int done = 0;
    for (;;) {
      if (x->len + 4096 > x->cap) { x->cap = x->cap ? x->cap * 2 : 8192; x->buf = realloc(x->buf, x->cap); }
      ssize_t k = read(x->fd, x->buf + x->len, x->cap - x->len);
      if (k > 0) { x->len += k; if (x->len > MAX_XFER) { done = 1; break; } continue; }
      if (k == 0) { done = 1; break; }
      if (errno == EINTR) continue;
      break;                                           /* EAGAIN: wait for more */
    }
    if (!done) { pp = &x->next; continue; }
    close(x->fd);
    if (x->len) {
      XChangeProperty((Display *)d, x->requestor, x->property, s->a_utf8, 8, PropModeReplace,
                      (unsigned char *)x->buf, (int)x->len);
      notify(d, x->requestor, x->selection, x->target, x->property);
    } else notify(d, x->requestor, x->selection, x->target, None);
    *pp = x->next; free(x->buf); free(x);
  }
}
