/* libxwl: a tiny Xlib-compatible layer implemented on Wayland + Cairo.
 *
 * Applications are compiled against the *real* X11 headers (for types,
 * XEvent, keysyms...) but link against this library instead of libX11.
 * Only the subset of Xlib used by wmcoincoin's pinnipede/palmipede is
 * implemented.  Nothing here talks to an X server.
 */
#ifndef XWL_H
#define XWL_H

#include <X11/Xlib.h>
#include <cairo.h>

/* --- extras that have no Xlib equivalent --- */

/* Cairo surface backing a Window or a Pixmap (for text rendering etc.). */
cairo_surface_t *xwl_surface(Display *dpy, Drawable d);

/* Tell xwl that a drawable was modified through cairo directly. */
void xwl_damage(Display *dpy, Drawable d);

/* Debug: dump a drawable to a PNG. Returns 0 on success. */
int xwl_write_png(Display *dpy, Drawable d, const char *path);

/* File descriptor to poll() on in the application main loop. */
int xwl_fd(Display *dpy);

#endif
