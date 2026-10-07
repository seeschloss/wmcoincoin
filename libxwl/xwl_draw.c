/* libxwl: GC, pixmaps, drawing primitives, XImage, windows, properties. */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <cairo.h>
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#include "xwl_priv.h"
#include "xwl.h"

static XwlDisplay *XD(Display *d) { return (XwlDisplay *)d; }

static void set_color(cairo_t *cr, unsigned long px)
{
  cairo_set_source_rgb(cr, ((px >> 16) & 0xff) / 255.0, ((px >> 8) & 0xff) / 255.0,
                       (px & 0xff) / 255.0);
}

/* ------------------------------------------------------------------ */
/* GC                                                                 */
/* ------------------------------------------------------------------ */

static void gc_apply_values(XwlGC *g, unsigned long mask, XGCValues *v)
{
  if (mask & GCFunction)   g->function = v->function;
  if (mask & GCForeground) g->fg = v->foreground;
  if (mask & GCBackground) g->bg = v->background;
  if (mask & GCLineWidth)  g->line_width = v->line_width;
  if (mask & GCLineStyle)  g->line_style = v->line_style;
  if (mask & GCClipMask)   g->clip_mask = v->clip_mask;
  if (mask & GCClipXOrigin) g->clip_x = v->clip_x_origin;
  if (mask & GCClipYOrigin) g->clip_y = v->clip_y_origin;
  if (mask & GCFont)       g->font = (void *)(uintptr_t)v->font;
}

GC XCreateGC(Display *dpy, Drawable d, unsigned long mask, XGCValues *v)
{
  (void)dpy; (void)d;
  XwlGC *g = calloc(1, sizeof *g);
  g->function = GXcopy; g->line_width = 1; g->line_style = LineSolid; g->fg = 0; g->bg = 0xffffff;
  if (v) gc_apply_values(g, mask, v);
  return (GC)g;
}
int XFreeGC(Display *dpy, GC gc) { (void)dpy; if (gc) { free(((XwlGC *)gc)->clip_rects); free(gc); } return 0; }
int XChangeGC(Display *dpy, GC gc, unsigned long mask, XGCValues *v)
{ (void)dpy; gc_apply_values((XwlGC *)gc, mask, v); return 0; }
Status XGetGCValues(Display *dpy, GC gc, unsigned long mask, XGCValues *v)
{
  XwlGC *g = (XwlGC *)gc; (void)dpy; (void)mask;
  memset(v, 0, sizeof *v);
  v->function = g->function; v->foreground = g->fg; v->background = g->bg;
  v->line_width = g->line_width; v->line_style = g->line_style;
  v->font = (Font)(uintptr_t)g->font;
  return 1;
}
int XSetForeground(Display *dpy, GC gc, unsigned long p) { (void)dpy; ((XwlGC *)gc)->fg = p; return 0; }
int XSetBackground(Display *dpy, GC gc, unsigned long p) { (void)dpy; ((XwlGC *)gc)->bg = p; return 0; }
int XSetFont(Display *dpy, GC gc, Font f) { (void)dpy; ((XwlGC *)gc)->font = (void *)(uintptr_t)f; return 0; }
int XSetClipMask(Display *dpy, GC gc, Pixmap m)
{
  XwlGC *g = (XwlGC *)gc; (void)dpy;
  g->clip_mask = m; free(g->clip_rects); g->clip_rects = NULL; g->nclip = 0;
  return 0;
}
int XSetClipOrigin(Display *dpy, GC gc, int x, int y)
{ (void)dpy; ((XwlGC *)gc)->clip_x = x; ((XwlGC *)gc)->clip_y = y; return 0; }
int XSetClipRectangles(Display *dpy, GC gc, int xo, int yo, XRectangle *r, int n, int ord)
{
  XwlGC *g = (XwlGC *)gc; (void)dpy; (void)ord;
  free(g->clip_rects);
  g->clip_rects = malloc(sizeof(XRectangle) * (n > 0 ? n : 1));
  memcpy(g->clip_rects, r, sizeof(XRectangle) * n);
  g->nclip = n; g->clip_x = xo; g->clip_y = yo; g->clip_mask = None;
  return 0;
}

/* Build a cairo context for drawing with a GC onto a drawable. */
static cairo_t *begin(Display *dpy, Drawable d, GC gc)
{
  XwlDrawable *dr = xwl_lookup(d);
  if (!dr || !dr->surf) return NULL;
  cairo_t *cr = cairo_create(dr->surf);
  XwlGC *g = (XwlGC *)gc;
  cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  if (g) {
    if (dr->depth == 1) cairo_set_source_rgba(cr, 0, 0, 0, g->fg ? 1.0 : 0.0);   /* bitmap: 0 clears, 1 sets */
    else set_color(cr, g->fg);
    if (g->nclip) {
      for (int i = 0; i < g->nclip; i++)
        cairo_rectangle(cr, g->clip_x + g->clip_rects[i].x, g->clip_y + g->clip_rects[i].y,
                        g->clip_rects[i].width, g->clip_rects[i].height);
      cairo_clip(cr);
    }
    if (g->clip_mask) {
      XwlDrawable *m = xwl_lookup(g->clip_mask);
      if (m && m->surf) {
        cairo_push_group(cr);   /* handled by callers via mask when needed */
        cairo_pop_group_to_source(cr);
        set_color(cr, g->fg);
        cairo_mask_surface(cr, m->surf, g->clip_x, g->clip_y);
        /* fall through: the mask is applied as a clip region instead */
      }
    }
  }
  (void)dpy;
  return cr;
}

static void finish(cairo_t *cr, Drawable d)
{
  cairo_destroy(cr);
  XwlDrawable *dr = xwl_lookup(d);
  if (dr && dr->kind == XWL_WINDOW) dr->dirty = 1;
}

/* ------------------------------------------------------------------ */
/* Pixmaps                                                            */
/* ------------------------------------------------------------------ */

Pixmap XCreatePixmap(Display *dpy, Drawable d, unsigned w, unsigned h, unsigned depth)
{
  (void)d;
  XwlDrawable *p = calloc(1, sizeof *p);
  p->kind = XWL_PIXMAP; p->dpy = XD(dpy);
  p->w = w ? w : 1; p->h = h ? h : 1; p->depth = depth;
  p->surf = cairo_image_surface_create(depth == 1 ? CAIRO_FORMAT_A8 : CAIRO_FORMAT_RGB24, p->w, p->h);
  return (Pixmap)(uintptr_t)p;
}
int XFreePixmap(Display *dpy, Pixmap pm)
{
  XwlDrawable *p = xwl_lookup(pm); (void)dpy;
  if (!p) return 0;
  cairo_surface_destroy(p->surf); free(p);
  return 0;
}
Pixmap XCreateBitmapFromData(Display *dpy, Drawable d, _Xconst char *data, unsigned w, unsigned h)
{
  Pixmap pm = XCreatePixmap(dpy, d, w, h, 1);
  XwlDrawable *p = xwl_lookup(pm);
  unsigned char *dst = cairo_image_surface_get_data(p->surf);
  int stride = cairo_image_surface_get_stride(p->surf), bpl = (w + 7) / 8;
  cairo_surface_flush(p->surf);
  for (unsigned y = 0; y < h; y++)
    for (unsigned x = 0; x < w; x++)   /* XBM: LSB first */
      dst[y * stride + x] = (data[y * bpl + x / 8] >> (x % 8)) & 1 ? 0xff : 0;
  cairo_surface_mark_dirty(p->surf);
  return pm;
}

/* ------------------------------------------------------------------ */
/* Drawing primitives                                                 */
/* ------------------------------------------------------------------ */

int XFillRectangle(Display *dpy, Drawable d, GC gc, int x, int y, unsigned w, unsigned h)
{
  cairo_t *cr = begin(dpy, d, gc); if (!cr) return 0;
  cairo_rectangle(cr, x, y, w, h); cairo_fill(cr); finish(cr, d); return 0;
}
int XDrawRectangle(Display *dpy, Drawable d, GC gc, int x, int y, unsigned w, unsigned h)
{
  cairo_t *cr = begin(dpy, d, gc); if (!cr) return 0;
  cairo_set_line_width(cr, 1); cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
  cairo_rectangle(cr, x + 0.5, y + 0.5, w, h); cairo_stroke(cr); finish(cr, d); return 0;
}
static void apply_line_style(cairo_t *cr, XwlGC *g)
{
  cairo_set_line_width(cr, g && g->line_width > 0 ? g->line_width : 1);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
  if (g && g->line_style != LineSolid) {
    double dash[2] = { 4, 4 };
    cairo_set_dash(cr, dash, 2, 0);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
  }
}
int XDrawLine(Display *dpy, Drawable d, GC gc, int x1, int y1, int x2, int y2)
{
  cairo_t *cr = begin(dpy, d, gc); if (!cr) return 0;
  apply_line_style(cr, (XwlGC *)gc);
  cairo_move_to(cr, x1 + 0.5, y1 + 0.5); cairo_line_to(cr, x2 + 0.5, y2 + 0.5);
  cairo_stroke(cr); finish(cr, d); return 0;
}
int XDrawLines(Display *dpy, Drawable d, GC gc, XPoint *p, int n, int mode)
{
  cairo_t *cr = begin(dpy, d, gc); if (!cr || n < 2) { if (cr) finish(cr, d); return 0; }
  apply_line_style(cr, (XwlGC *)gc);
  double x = p[0].x + 0.5, y = p[0].y + 0.5; cairo_move_to(cr, x, y);
  for (int i = 1; i < n; i++) {
    if (mode == CoordModePrevious) { x += p[i].x; y += p[i].y; } else { x = p[i].x + 0.5; y = p[i].y + 0.5; }
    cairo_line_to(cr, x, y);
  }
  cairo_stroke(cr); finish(cr, d); return 0;
}
int XDrawPoint(Display *dpy, Drawable d, GC gc, int x, int y)
{
  cairo_t *cr = begin(dpy, d, gc); if (!cr) return 0;
  cairo_rectangle(cr, x, y, 1, 1); cairo_fill(cr); finish(cr, d); return 0;
}
int XDrawPoints(Display *dpy, Drawable d, GC gc, XPoint *p, int n, int mode)
{
  cairo_t *cr = begin(dpy, d, gc); if (!cr) return 0;
  int x = 0, y = 0;
  for (int i = 0; i < n; i++) {
    if (mode == CoordModePrevious) { x += p[i].x; y += p[i].y; } else { x = p[i].x; y = p[i].y; }
    cairo_rectangle(cr, x, y, 1, 1);
  }
  cairo_fill(cr); finish(cr, d); return 0;
}
int XFillPolygon(Display *dpy, Drawable d, GC gc, XPoint *p, int n, int shape, int mode)
{
  (void)shape;
  cairo_t *cr = begin(dpy, d, gc); if (!cr || n < 3) { if (cr) finish(cr, d); return 0; }
  double x = p[0].x, y = p[0].y; cairo_move_to(cr, x, y);
  for (int i = 1; i < n; i++) {
    if (mode == CoordModePrevious) { x += p[i].x; y += p[i].y; } else { x = p[i].x; y = p[i].y; }
    cairo_line_to(cr, x, y);
  }
  cairo_close_path(cr); cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);
  cairo_fill(cr); finish(cr, d); return 0;
}
int XFillArc(Display *dpy, Drawable d, GC gc, int x, int y, unsigned w, unsigned h, int a1, int a2)
{
  cairo_t *cr = begin(dpy, d, gc); if (!cr) return 0;
  cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);
  cairo_save(cr); cairo_translate(cr, x + w / 2.0, y + h / 2.0); cairo_scale(cr, w / 2.0, h / 2.0);
  double s = -a1 / 64.0 * M_PI / 180.0, e = -(a1 + a2) / 64.0 * M_PI / 180.0;
  cairo_move_to(cr, 0, 0); cairo_arc_negative(cr, 0, 0, 1, s, e); cairo_close_path(cr);
  cairo_restore(cr); cairo_fill(cr); finish(cr, d); return 0;
}

int XClearArea(Display *dpy, Window w, int x, int y, unsigned width, unsigned height, Bool exp)
{
  XwlDrawable *dr = xwl_lookup(w); (void)exp;
  if (!dr || !dr->surf) return 0;
  cairo_t *cr = cairo_create(dr->surf); set_color(cr, dr->bg_pixel);
  if (!width) width = dr->w - x;
  if (!height) height = dr->h - y;
  cairo_rectangle(cr, x, y, width, height); cairo_fill(cr); cairo_destroy(cr);
  dr->dirty = 1; (void)dpy; return 0;
}

int XCopyArea(Display *dpy, Drawable src, Drawable dst, GC gc, int sx, int sy,
              unsigned w, unsigned h, int dx, int dy)
{
  XwlDrawable *s = xwl_lookup(src), *t = xwl_lookup(dst);
  XwlGC *g = (XwlGC *)gc; (void)dpy;
  if (!s || !t || !s->surf || !t->surf) return 0;
  cairo_t *cr = cairo_create(t->surf);
  cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
  cairo_rectangle(cr, dx, dy, w, h); cairo_clip(cr);
  if (g && g->nclip) {
    cairo_new_path(cr);
    for (int i = 0; i < g->nclip; i++)
      cairo_rectangle(cr, g->clip_x + g->clip_rects[i].x, g->clip_y + g->clip_rects[i].y,
                      g->clip_rects[i].width, g->clip_rects[i].height);
    cairo_clip(cr);
  }
  /* depth-1 source (bitmap): paint with fg/bg like X does */
  if (s->depth == 1) {
    set_color(cr, g ? g->fg : 0);
    cairo_mask_surface(cr, s->surf, dx - sx, dy - sy);
  } else {
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr, s->surf, dx - sx, dy - sy);
    if (g && g->clip_mask) {
      XwlDrawable *m = xwl_lookup(g->clip_mask);
      if (m && m->surf) {
        cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
        cairo_mask_surface(cr, m->surf, g->clip_x, g->clip_y);
        goto done;
      }
    }
    cairo_paint(cr);
  }
done:
  cairo_destroy(cr);
  if (t->kind == XWL_WINDOW) t->dirty = 1;
  return 0;
}

/* ------------------------------------------------------------------ */
/* XImage (software, 32 bpp)                                          */
/* ------------------------------------------------------------------ */

static unsigned long img_get(XImage *im, int x, int y)
{ return ((uint32_t *)(im->data + (size_t)y * im->bytes_per_line))[x]; }
static int img_put(XImage *im, int x, int y, unsigned long p)
{ ((uint32_t *)(im->data + (size_t)y * im->bytes_per_line))[x] = (uint32_t)p; return 0; }
static int img_destroy(XImage *im) { free(im->data); free(im); return 0; }

XImage *XCreateImage(Display *dpy, Visual *vis, unsigned depth, int format, int offset,
                     char *data, unsigned w, unsigned h, int pad, int bpl)
{
  (void)dpy; (void)vis; (void)format; (void)offset; (void)pad;
  XImage *im = calloc(1, sizeof *im);
  im->width = w; im->height = h; im->depth = depth; im->format = ZPixmap;
  im->bits_per_pixel = 32; im->bitmap_unit = 32; im->bitmap_pad = 32;
  im->bytes_per_line = bpl ? bpl : w * 4;
  im->byte_order = LSBFirst; im->bitmap_bit_order = LSBFirst;
  im->red_mask = 0xff0000; im->green_mask = 0xff00; im->blue_mask = 0xff;
  im->data = data;
  im->f.get_pixel = img_get; im->f.put_pixel = img_put; im->f.destroy_image = img_destroy;
  return im;
}

int XPutImage(Display *dpy, Drawable d, GC gc, XImage *im, int sx, int sy, int dx, int dy,
              unsigned w, unsigned h)
{
  XwlDrawable *t = xwl_lookup(d); (void)gc; (void)dpy;
  if (!t || !t->surf) return 0;
  cairo_surface_flush(t->surf);
  unsigned char *dst = cairo_image_surface_get_data(t->surf);
  int stride = cairo_image_surface_get_stride(t->surf);
  for (unsigned y = 0; y < h; y++) {
    int ty = dy + y; if (ty < 0 || ty >= t->h) continue;
    for (unsigned x = 0; x < w; x++) {
      int tx = dx + x; if (tx < 0 || tx >= t->w) continue;
      ((uint32_t *)(dst + (size_t)ty * stride))[tx] = (uint32_t)img_get(im, sx + x, sy + y) | 0xff000000u;
    }
  }
  cairo_surface_mark_dirty(t->surf);
  if (t->kind == XWL_WINDOW) t->dirty = 1;
  return 0;
}

XImage *XGetImage(Display *dpy, Drawable d, int x, int y, unsigned w, unsigned h,
                  unsigned long mask, int format)
{
  XwlDrawable *s = xwl_lookup(d); (void)mask; (void)format;
  if (!s || !s->surf || s->kind == XWL_ROOT) return NULL;   /* no root-window capture on Wayland */
  char *buf = calloc(w * h, 4);
  XImage *im = XCreateImage(dpy, NULL, 24, ZPixmap, 0, buf, w, h, 32, 0);
  cairo_surface_flush(s->surf);
  unsigned char *src = cairo_image_surface_get_data(s->surf);
  int stride = cairo_image_surface_get_stride(s->surf);
  for (unsigned j = 0; j < h; j++)
    for (unsigned i = 0; i < w; i++) {
      int sx = x + i, sy = y + j;
      if (sx < 0 || sy < 0 || sx >= s->w || sy >= s->h) continue;
      img_put(im, i, j, ((uint32_t *)(src + (size_t)sy * stride))[sx] & 0xffffff);
    }
  return im;
}

/* ------------------------------------------------------------------ */
/* Windows                                                            */
/* ------------------------------------------------------------------ */

extern void xwl_window_realize(XwlDisplay *d, XwlDrawable *w);

Window XCreateWindow(Display *dpy, Window parent, int x, int y, unsigned w, unsigned h,
                     unsigned bw, int depth, unsigned cls, Visual *vis, unsigned long vmask,
                     XSetWindowAttributes *a)
{
  XwlDisplay *d = XD(dpy);
  (void)x; (void)y; (void)bw; (void)depth; (void)cls; (void)vis; (void)parent;
  XwlDrawable *win = calloc(1, sizeof *win);
  win->kind = XWL_WINDOW; win->dpy = d; win->depth = 24;
  win->bg_pixel = 0xffffff;
  if (vmask & CWBackPixel) win->bg_pixel = a->background_pixel;
  if (vmask & CWEventMask) win->event_mask = a->event_mask;
  win->w = 0; win->h = 0;
  /* backing store created through the shared resize path */
  extern void xwl_window_init_size(XwlDisplay *, XwlDrawable *, int, int);
  xwl_window_init_size(d, win, w, h);
  win->next_win = d->windows; d->windows = win;
  return (Window)(uintptr_t)win;
}
Window XCreateSimpleWindow(Display *dpy, Window parent, int x, int y, unsigned w, unsigned h,
                           unsigned bw, unsigned long border, unsigned long bg)
{
  XSetWindowAttributes a; (void)border;
  a.background_pixel = bg;
  return XCreateWindow(dpy, parent, x, y, w, h, bw, 24, InputOutput, NULL, CWBackPixel, &a);
}
int XChangeWindowAttributes(Display *dpy, Window w, unsigned long m, XSetWindowAttributes *a)
{
  XwlDrawable *dr = xwl_lookup(w); (void)dpy;
  if (!dr) return 0;
  if (m & CWEventMask) dr->event_mask = a->event_mask;
  if (m & CWBackPixel) dr->bg_pixel = a->background_pixel;
  return 0;
}
int XSelectInput(Display *dpy, Window w, long mask)
{ XwlDrawable *dr = xwl_lookup(w); (void)dpy; if (dr) dr->event_mask = mask; return 0; }
int XSetWindowBackground(Display *dpy, Window w, unsigned long px)
{ XwlDrawable *dr = xwl_lookup(w); (void)dpy; if (dr) dr->bg_pixel = px; return 0; }
int XSetWindowBackgroundPixmap(Display *dpy, Window w, Pixmap p)
{ (void)dpy; (void)w; (void)p; return 0; }   /* TODO: transparency/bg pixmap */
int XSetWindowBorderWidth(Display *dpy, Window w, unsigned bw) { (void)dpy; (void)w; (void)bw; return 0; }

int XMapWindow(Display *dpy, Window w)
{
  XwlDrawable *dr = xwl_lookup(w);
  if (dr && dr->kind == XWL_WINDOW && !dr->mapped) xwl_window_realize(XD(dpy), dr);
  return 0;
}
int XMapRaised(Display *dpy, Window w) { return XMapWindow(dpy, w); }
int XRaiseWindow(Display *dpy, Window w) { (void)dpy; (void)w; return 0; }
int XUnmapWindow(Display *dpy, Window w)
{
  XwlDrawable *dr = xwl_lookup(w); (void)dpy;
  if (!dr || !dr->mapped) return 0;
  extern void xwl_window_unrealize(XwlDrawable *);
  xwl_window_unrealize(dr);
  return 0;
}
int XDestroyWindow(Display *dpy, Window w)
{
  XwlDisplay *d = XD(dpy);
  XwlDrawable *dr = xwl_lookup(w);
  if (!dr) return 0;
  XUnmapWindow(dpy, w);
  for (XwlDrawable **pp = &d->windows; *pp; pp = &(*pp)->next_win)
    if (*pp == dr) { *pp = dr->next_win; break; }
  cairo_surface_destroy(dr->surf);
  free(dr->title); free(dr->app_id); free(dr);
  return 0;
}
int XMoveWindow(Display *dpy, Window w, int x, int y) { (void)dpy; (void)w; (void)x; (void)y; return 0; }
int XMoveResizeWindow(Display *dpy, Window w, int x, int y, unsigned width, unsigned height)
{
  XwlDrawable *dr = xwl_lookup(w); (void)x; (void)y;
  extern void xwl_window_client_resize(XwlDisplay *, XwlDrawable *, int, int);
  if (dr) xwl_window_client_resize(XD(dpy), dr, width, height);
  return 0;
}
int XResizeWindow(Display *dpy, Window w, unsigned width, unsigned height)
{
  XwlDrawable *dr = xwl_lookup(w);
  extern void xwl_window_client_resize(XwlDisplay *, XwlDrawable *, int, int);
  if (dr) xwl_window_client_resize(XD(dpy), dr, width, height);
  return 0;
}
Status XGetGeometry(Display *dpy, Drawable d, Window *root, int *x, int *y, unsigned *w,
                    unsigned *h, unsigned *bw, unsigned *depth)
{
  XwlDrawable *dr = xwl_lookup(d);
  if (!dr) return 0;
  if (root) *root = XD(dpy)->screen.root;
  if (x) *x = 0;
  if (y) *y = 0;
  if (bw) *bw = 0;
  if (w) *w = dr->w;
  if (h) *h = dr->h;
  if (depth) *depth = dr->depth ? dr->depth : 24;
  return 1;
}
Status XGetWindowAttributes(Display *dpy, Window w, XWindowAttributes *at)
{
  XwlDrawable *dr = xwl_lookup(w);
  memset(at, 0, sizeof *at);
  if (!dr) return 0;
  at->width = dr->w; at->height = dr->h; at->depth = 24; at->visual = XD(dpy)->screen.root_visual;
  at->root = XD(dpy)->screen.root; at->screen = &XD(dpy)->screen;
  at->map_state = dr->mapped ? IsViewable : IsUnmapped; at->your_event_mask = dr->event_mask;
  at->class = InputOutput;
  return 1;
}
Bool XTranslateCoordinates(Display *dpy, Window sw, Window dw, int sx, int sy, int *dx, int *dy, Window *child)
{ (void)dpy; (void)sw; (void)dw; *dx = sx; *dy = sy; if (child) *child = None; return True; }

Screen *XScreenOfDisplay_fn(Display *dpy, int n) { return &XD(dpy)->screen; (void)n; }
Screen *XScreenOfDisplay(Display *dpy, int n) { (void)n; return &XD(dpy)->screen; }

/* ---------------- WM hints: mapped onto xdg_toplevel ---------------- */
void xwl_apply_title(XwlDrawable *w)
{ if (w->top && w->title) xdg_toplevel_set_title(w->top, w->title); }
void xwl_apply_hints(XwlDrawable *w)
{
  if (!w->top) return;
  if (w->app_id) xdg_toplevel_set_app_id(w->top, w->app_id);
  if (w->min_w > 0 && w->min_h > 0) xdg_toplevel_set_min_size(w->top, w->min_w, w->min_h);
  if (w->max_w > 0 && w->max_h > 0) xdg_toplevel_set_max_size(w->top, w->max_w, w->max_h);
}

int XSetWMName_(Display *dpy, Window w, const char *s) { (void)dpy; XwlDrawable *dr = xwl_lookup(w); if (dr) { free(dr->title); dr->title = strdup(s); xwl_apply_title(dr);} return 0; }
void XSetWMName(Display *dpy, Window w, XTextProperty *p) { if (p && p->value) XSetWMName_(dpy, w, (char *)p->value); }
void XSetWMIconName(Display *dpy, Window w, XTextProperty *p) { (void)dpy; (void)w; (void)p; }
int XStoreName(Display *dpy, Window w, _Xconst char *s) { return XSetWMName_(dpy, w, s); }
Status XStringListToTextProperty(char **list, int n, XTextProperty *p)
{
  p->value = (unsigned char *)strdup(n > 0 ? list[0] : ""); p->encoding = XA_STRING; p->format = 8;
  p->nitems = n > 0 ? strlen(list[0]) : 0; return 1;
}
Status XSetWMProtocols(Display *dpy, Window w, Atom *p, int n) { (void)dpy; (void)w; (void)p; (void)n; return 1; }
XSizeHints *XAllocSizeHints(void) { return calloc(1, sizeof(XSizeHints)); }
XWMHints *XAllocWMHints(void) { return calloc(1, sizeof(XWMHints)); }
XClassHint *XAllocClassHint(void) { return calloc(1, sizeof(XClassHint)); }
void XSetWMNormalHints(Display *dpy, Window w, XSizeHints *h)
{
  XwlDrawable *dr = xwl_lookup(w); (void)dpy;
  if (dr && h) {
    if (h->flags & PMinSize) { dr->min_w = h->min_width; dr->min_h = h->min_height; }
    if (h->flags & PMaxSize) { dr->max_w = h->max_width; dr->max_h = h->max_height; }
    xwl_apply_hints(dr);
  }
}
Status XGetWMNormalHints(Display *dpy, Window w, XSizeHints *h, long *supplied)
{ (void)dpy; (void)w; memset(h, 0, sizeof *h); if (supplied) *supplied = 0; return 1; }
int XSetWMHints(Display *dpy, Window w, XWMHints *h) { (void)dpy; (void)w; (void)h; return 0; }
int XSetClassHint(Display *dpy, Window w, XClassHint *c)
{
  XwlDrawable *dr = xwl_lookup(w); (void)dpy;
  if (dr && c && c->res_class) { free(dr->app_id); dr->app_id = strdup(c->res_class); xwl_apply_hints(dr); }
  return 0;
}
int XSetTransientForHint(Display *dpy, Window w, Window prop)
{ XwlDrawable *dr = xwl_lookup(w); (void)dpy; if (dr) dr->transient_for = prop; return 0; }

/* ---------------- properties (used by the selection emulation) --------- */
int XChangeProperty(Display *dpy, Window w, Atom prop, Atom type, int format, int mode,
                    _Xconst unsigned char *data, int n)
{
  XwlDrawable *dr = xwl_lookup(w); (void)dpy;
  if (!dr) return 0;
  XwlProp *p = dr->props;
  while (p && p->name != prop) p = p->next;
  if (!p) { p = calloc(1, sizeof *p); p->name = prop; p->next = dr->props; dr->props = p; }
  size_t unit = format / 8, old = (mode == PropModeAppend) ? (size_t)p->nelements * unit : 0;
  p->data = realloc(p->data, old + (size_t)n * unit + 1);
  memcpy(p->data + old, data, (size_t)n * unit);
  p->nelements = (mode == PropModeAppend ? p->nelements : 0) + n;
  p->data[(size_t)p->nelements * unit] = 0;
  p->type = type; p->format = format;
  return 0;
}
int XGetWindowProperty(Display *dpy, Window w, Atom prop, long off, long len, Bool del,
                       Atom req, Atom *rtype, int *rfmt, unsigned long *nitems,
                       unsigned long *after, unsigned char **data)
{
  XwlDrawable *dr = xwl_lookup(w); (void)dpy; (void)off; (void)len; (void)req;
  XwlProp *p = dr ? dr->props : NULL, **pp = dr ? &dr->props : NULL;
  while (p && p->name != prop) { pp = &p->next; p = p->next; }
  *rtype = None; *rfmt = 0; *nitems = 0; *after = 0; *data = NULL;
  if (!p) return Success;
  size_t unit = p->format / 8, sz = (size_t)p->nelements * unit;
  *data = malloc(sz + 1); memcpy(*data, p->data, sz); (*data)[sz] = 0;
  *rtype = p->type; *rfmt = p->format; *nitems = p->nelements;
  if (del) { *pp = p->next; free(p->data); free(p); }
  return Success;
}
int XFree(void *p) { free(p); return 0; }

/* ---------------- misc no-ops / error handling ---------------- */
XErrorHandler XSetErrorHandler(XErrorHandler h) { (void)h; return NULL; }
int XGetErrorText(Display *dpy, int code, char *buf, int n) { (void)dpy; snprintf(buf, n, "error %d", code); return 0; }
static int hexval(const char *p, int n, unsigned *out)
{
  unsigned v = 0;
  for (int i = 0; i < n; i++) {
    char c = p[i]; v <<= 4;
    if (c >= '0' && c <= '9') v |= c - '0';
    else if (c >= 'a' && c <= 'f') v |= c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') v |= c - 'A' + 10;
    else return 0;
  }
  *out = v; return 1;
}
/* scale an n-hex-digit component to 16 bits */
static unsigned scale16(unsigned v, int ndigits)
{ unsigned max = (1u << (4 * ndigits)) - 1; return (unsigned)((unsigned long)v * 65535 / max); }

#include "xwl_colors.h"
static int color_cmp(const void *k, const void *e) { return strcmp((const char *)k, ((const struct { const char *name; unsigned char r, g, b; } *)e)->name); }
static int lookup_named(const char *name, unsigned *r, unsigned *g, unsigned *b)
{
  char key[64]; int n = 0;
  for (const char *p = name; *p && n < 63; p++)
    if (*p != ' ') key[n++] = (*p >= 'A' && *p <= 'Z') ? *p + 32 : *p;
  key[n] = 0;
  const void *e = bsearch(key, xwl_colors, sizeof xwl_colors / sizeof *xwl_colors, sizeof *xwl_colors, color_cmp);
  if (!e) return 0;
  const typeof(xwl_colors[0]) *c = e;
  *r = c->r; *g = c->g; *b = c->b;
  return 1;
}

Status XParseColor(Display *dpy, Colormap cm, _Xconst char *spec, XColor *c)
{
  unsigned r, g, b; (void)dpy; (void)cm;
  c->flags = DoRed | DoGreen | DoBlue;
  if (spec[0] == '#') {
    size_t n = strlen(spec + 1);
    if (n == 3 || n == 6 || n == 9 || n == 12) {
      int k = n / 3;
      if (hexval(spec + 1, k, &r) && hexval(spec + 1 + k, k, &g) && hexval(spec + 1 + 2 * k, k, &b)) {
        c->red = scale16(r, k); c->green = scale16(g, k); c->blue = scale16(b, k); return 1;
      }
    }
    return 0;
  }
  if (!strncmp(spec, "rgb:", 4)) {
    const char *p = spec + 4; unsigned v[3]; int nd[3];
    for (int i = 0; i < 3; i++) {
      int k = 0; while (p[k] && p[k] != '/') k++;
      if (k < 1 || k > 4 || !hexval(p, k, &v[i])) return 0;
      nd[i] = k; p += k; if (i < 2) { if (*p != '/') return 0; p++; }
    }
    c->red = scale16(v[0], nd[0]); c->green = scale16(v[1], nd[1]); c->blue = scale16(v[2], nd[2]);
    return 1;
  }
  if (lookup_named(spec, &r, &g, &b)) { c->red = r * 257; c->green = g * 257; c->blue = b * 257; return 1; }
  return 0;
}
Status XAllocColor(Display *dpy, Colormap cm, XColor *c)
{
  (void)dpy; (void)cm;
  c->pixel = ((c->red >> 8) << 16) | ((c->green >> 8) << 8) | (c->blue >> 8);
  return 1;
}
Status XAllocNamedColor(Display *dpy, Colormap cm, _Xconst char *name, XColor *scr, XColor *exact)
{
  if (!XParseColor(dpy, cm, name, exact)) return 0;
  *scr = *exact; return XAllocColor(dpy, cm, scr);
}

/* ---------------- extras ---------------- */
cairo_surface_t *xwl_surface(Display *dpy, Drawable d)
{ XwlDrawable *dr = xwl_lookup(d); (void)dpy; return dr ? dr->surf : NULL; }
void xwl_damage(Display *dpy, Drawable d)
{ XwlDrawable *dr = xwl_lookup(d); (void)dpy; if (dr && dr->surf) { cairo_surface_mark_dirty(dr->surf); if (dr->kind == XWL_WINDOW) dr->dirty = 1; } }
int xwl_write_png(Display *dpy, Drawable d, const char *path)
{ XwlDrawable *dr = xwl_lookup(d); (void)dpy; return dr && dr->surf && cairo_surface_write_to_png(dr->surf, path) == CAIRO_STATUS_SUCCESS ? 0 : -1; }
int XSetLineAttributes(Display *dpy, GC gc, unsigned lw, int ls, int cs, int js)
{ XwlGC *g = (XwlGC *)gc; (void)dpy; (void)cs; (void)js; g->line_width = lw; g->line_style = ls; return 0; }
