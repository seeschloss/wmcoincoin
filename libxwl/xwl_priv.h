#define XLIB_ILLEGAL_ACCESS 1
#ifndef XWL_PRIV_H
#define XWL_PRIV_H
#include <stdint.h>
#include <X11/Xlib.h>
#include <cairo.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>

enum { XWL_ROOT = 0, XWL_WINDOW = 1, XWL_PIXMAP = 2 };

typedef struct XwlDisplay XwlDisplay;
typedef struct EvNode { struct EvNode *next; XEvent ev; } EvNode;

typedef struct XwlBuf {
  struct wl_buffer *buf;
  void *data; size_t size; int w, h; int busy;
} XwlBuf;

typedef struct XwlProp {
  struct XwlProp *next;
  Atom name, type; int format; int nelements;
  unsigned char *data;               /* nelements * (format/8) bytes (+1 NUL) */
} XwlProp;

typedef struct XwlDrawable {
  int kind;                          /* must stay first */
  int w, h, depth;
  cairo_surface_t *surf;
  XwlDisplay *dpy;
  /* window only */
  struct wl_surface *wls;
  struct xdg_surface *xs;
  struct xdg_toplevel *top;
  XwlBuf bufs[2];
  void *shm_map; size_t shm_total;
  int mapped, configured, dirty;
  int override_redirect;             /* X override-redirect window -> xdg_popup (tooltips, menus) */
  struct xdg_popup *popup;
  int px, py;                        /* popup position, in the coordinates of the pointer-focused window */
  cairo_surface_t *bg_surf;          /* XSetWindowBackgroundPixmap: private copy, tiled */
  cairo_surface_t *shape;            /* A8 bounding mask (XShape), NULL = rectangular */
  int argb;                          /* buffers use ARGB8888 (needed when shaped) */
  int pending_w, pending_h;
  long event_mask;
  unsigned long bg_pixel;
  char *title, *app_id;
  Window transient_for;
  int min_w, min_h, max_w, max_h;
  XwlProp *props;
  struct XwlDrawable *next_win;
} XwlDrawable;

typedef struct XwlGC {
  unsigned long fg, bg;
  int function;                      /* GXcopy only (others ignored) */
  int line_width, line_style;
  Pixmap clip_mask; int clip_x, clip_y;
  XRectangle *clip_rects; int nclip;
  void *font;                        /* XFontStruct-ish, see xwl_font */
} XwlGC;

typedef struct { uint32_t serial; KeySym sym; char utf8[8]; int len; } XwlKeyInfo;

struct XwlDisplay {
  struct _XDisplay pub;          /* must stay first */
  struct wl_display *wl;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct wl_shm *shm;
  struct xdg_wm_base *wm_base;
  struct wl_seat *seat; uint32_t seat_name;
  struct wl_output *output;
  Screen screen; Visual visual;
  XwlDrawable root;
  XwlDrawable *windows;
  char **atoms; int natoms;
  EvNode *evhead, *evtail; int evcount;
  unsigned long serial;
  /* input */
  struct wl_pointer *pointer; struct wl_keyboard *keyboard;
  struct xkb_context *xkb_ctx; struct xkb_keymap *xkb_map; struct xkb_state *xkb_st;
  struct xkb_compose_table *compose_tbl; struct xkb_compose_state *compose_st;
  XwlDrawable *ptr_win, *kbd_win;
  int ptr_x, ptr_y; unsigned ptr_buttons; unsigned kbd_mods;
  double axis_acc; uint32_t last_serial; uint32_t last_time;
  int rep_rate, rep_delay; uint32_t rep_key; long rep_next; int rep_active;
  XwlKeyInfo keyinfo[64];
  /* selection (see xwl_sel.c) */
  void *sel;
};

int xwl_debug_on(void);
XwlDrawable *xwl_lookup(Drawable d);
void xwl_window_bg_apply(XwlDrawable *w, int x, int y, int ww, int hh);
void xwl_window_unrealize(XwlDrawable *w);
void xwl_window_realize(XwlDisplay *d, XwlDrawable *w);
void xwl_window_popup_moved(XwlDisplay *d, XwlDrawable *w);
XEvent *xwl_push_event(XwlDisplay *d, int type, Window win);
int xwl_wants(Window w, long mask);
const char *xwl_atom_name(XwlDisplay *d, Atom a);

void xwl_input_seat_bound(XwlDisplay *d);
void xwl_input_init(XwlDisplay *d);
void xwl_input_pump(XwlDisplay *d);
void xwl_selection_global(XwlDisplay *d, struct wl_registry *r, uint32_t name,
                          const char *iface, uint32_t ver);
void xwl_selection_init(XwlDisplay *d);
void xwl_sel_pump(XwlDisplay *d);
int xwl_sel_busy(XwlDisplay *d);
int xwl_sel_intercept(XwlDisplay *d, Window w, XEvent *ev);
#endif
