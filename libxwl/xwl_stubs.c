/* libxwl: X11 functionality that has no Wayland meaning.  Everything here is
 * either a harmless no-op or maps onto an equivalent concept. */
#define XLIB_ILLEGAL_ACCESS 1
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include "xwl_priv.h"
void xwl_window_set_argb(XwlDisplay *d, XwlDrawable *w, int argb);

/* --- XShape: arbitrary window shapes do not exist; the compositor draws rectangles --- */
void XShapeCombineMask(Display *d, Window w, int kind, int x, int y, Pixmap src, int op)
{
  XwlDrawable *dr = xwl_lookup(w);
  XwlDrawable *m = src ? xwl_lookup(src) : NULL;
  (void)x; (void)y; (void)op;
  if (!dr || dr->kind != XWL_WINDOW || kind != ShapeBounding) return;
  if (dr->shape) { cairo_surface_destroy(dr->shape); dr->shape = NULL; }
  if (m && m->surf && m->depth == 1) {
    /* private copy: the application frees its mask pixmap right after this call */
    cairo_surface_flush(m->surf);
    dr->shape = cairo_image_surface_create(CAIRO_FORMAT_A8, m->w, m->h);
    cairo_t *cr = cairo_create(dr->shape);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr, m->surf, 0, 0);
    cairo_paint(cr); cairo_destroy(cr);
  }
  xwl_window_set_argb((XwlDisplay *)d, dr, dr->shape != NULL);
}
void XShapeOffsetShape(Display *d, Window w, int kind, int x, int y)
{ (void)d; (void)w; (void)kind; (void)x; (void)y; }

/* --- Xmu atom helper (struct layout from X11/Xmu/Atoms.h) --- */
typedef struct _AtomRec { char *name; Atom atom; } *XwlAtomPtr;
Atom XmuInternAtom(Display *dpy, XwlAtomPtr a)
{
  if (!a->atom) a->atom = XInternAtom(dpy, a->name, False);
  return a->atom;
}

/* --- cursors: selection of cursor shapes could map to wp_cursor_shape later --- */
Cursor XCreateFontCursor(Display *d, unsigned shape) { (void)d; return (Cursor)(shape + 1); }
int XDefineCursor(Display *d, Window w, Cursor c) { (void)d; (void)w; (void)c; return 0; }
int XUndefineCursor(Display *d, Window w) { (void)d; (void)w; return 0; }

/* --- window tree / reparenting / dock / session: not applicable --- */
Status XQueryTree(Display *d, Window w, Window *root, Window *parent, Window **children, unsigned *n)
{
  /* every window is a top-level child of the (virtual) root; callers walk up until parent == root */
  Window r = XDefaultRootWindow(d);
  if (root) *root = r;
  if (parent) *parent = (w == r) ? None : r;
  if (children) *children = NULL;
  if (n) *n = 0;
  return 1;
}
int XReparentWindow(Display *d, Window w, Window p, int x, int y) { (void)d; (void)w; (void)p; (void)x; (void)y; return 0; }
int XSetCommand(Display *d, Window w, char **argv, int argc) { (void)d; (void)w; (void)argv; (void)argc; return 0; }
Status XGetIconSizes(Display *d, Window w, XIconSize **sizes, int *n) { (void)d; (void)w; *sizes = NULL; *n = 0; return 0; }
typedef int (*XIOErrorHandler_)(Display *);
XIOErrorHandler_ XSetIOErrorHandler(XIOErrorHandler_ h) { (void)h; return NULL; }

/* --- input methods: replaced by xkbcommon compose in xwl_input.c --- */
XIM XOpenIM(Display *d, struct _XrmHashBucketRec *db, char *name, char *cls) { (void)d; (void)db; (void)name; (void)cls; return NULL; }

/* --- legacy core fonts: the app uses fontcoincoin (cairo) for all live text --- */
XFontStruct *XLoadQueryFont(Display *d, _Xconst char *name)
{
  /* Only the (hidden) dock applet still wants a core font.  Hand it a dummy so its
   * bookkeeping works; nothing is ever drawn with it (XDrawString is a no-op). */
  XFontStruct *f = calloc(1, sizeof *f);
  (void)d; (void)name;
  f->ascent = 9; f->descent = 2; f->fid = 1;
  return f;
}
int XFreeFont(Display *d, XFontStruct *f) { (void)d; free(f); return 0; }
int XTextWidth(XFontStruct *f, _Xconst char *s, int n) { (void)f; (void)s; return n * 6; }
int XDrawString(Display *d, Drawable w, GC gc, int x, int y, _Xconst char *s, int n)
{ (void)d; (void)w; (void)gc; (void)x; (void)y; (void)s; (void)n; return 0; }

/* --- geometry strings: [=][WxH][{+-}X{+-}Y] (pure parser, same semantics as libX11) --- */
int XParseGeometry(_Xconst char *s, int *x, int *y, unsigned *w, unsigned *h)
{
  int mask = NoValue; char *p = (char *)s, *e;
  if (!s || !*s) return mask;
  if (*p == '=') p++;
  if (*p != '+' && *p != '-' && *p != 'x' && *p != 'X') {
    long v = strtol(p, &e, 10);
    if (e != p) { *w = v; mask |= WidthValue; p = e; }
  }
  if (*p == 'x' || *p == 'X') {
    p++; long v = strtol(p, &e, 10);
    if (e != p) { *h = v; mask |= HeightValue; p = e; }
  }
  if (*p == '+' || *p == '-') {
    int neg = *p == '-'; p++; long v = strtol(p, &e, 10);
    if (e != p) { *x = neg ? -v : v; mask |= XValue | (neg ? XNegative : 0); p = e; }
    if (*p == '+' || *p == '-') {
      neg = *p == '-'; p++; v = strtol(p, &e, 10);
      if (e != p) { *y = neg ? -v : v; mask |= YValue | (neg ? YNegative : 0); }
    }
  }
  return mask;
}
int XWMGeometry(Display *d, int scr, _Xconst char *user, _Xconst char *def, unsigned bw,
                XSizeHints *hints, int *x, int *y, int *w, int *h, int *grav)
{
  int ux = 0, uy = 0, dx = 0, dy = 0, mask, dmask; unsigned uw = 0, uh = 0, dw = 0, dh = 0;
  (void)d; (void)scr; (void)bw;
  mask = XParseGeometry(user, &ux, &uy, &uw, &uh);
  dmask = def ? XParseGeometry(def, &dx, &dy, &dw, &dh) : 0;
  *x = (mask & XValue) ? ux : dx; *y = (mask & YValue) ? uy : dy;
  *w = (mask & WidthValue) ? (int)uw : (int)dw; *h = (mask & HeightValue) ? (int)uh : (int)dh;
  if (hints) { hints->flags |= (mask | dmask) & (XValue | YValue | WidthValue | HeightValue); }
  if (grav) *grav = NorthWestGravity;
  return mask | (dmask & ~mask);
}
int _Xdebug = 0;   /* referenced by wmcoincoin.c for X synchronous debugging; meaningless here */

/* function forms of display accessors that code may call instead of the macros */
Window XDefaultRootWindow(Display *d) { return ((XwlDisplay *)d)->screen.root; }
int XDefaultScreen(Display *d) { (void)d; return 0; }

/* Xmu's AtomPtr globals used through XA_UTF8_STRING(dpy) */
static struct _AtomRec xa_utf8_rec = { "UTF8_STRING", 0 };
XwlAtomPtr _XA_UTF8_STRING = &xa_utf8_rec;
