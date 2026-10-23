/* libxwl: the pointer cursor.
 *
 * On Wayland a client has to say which cursor it wants every time the pointer enters one of its
 * surfaces; a client that says nothing gets whatever the previous pointer focus left behind, which looks
 * random.  X programs set a cursor per window with XDefineCursor(): that is kept (XwlDrawable.cursor, see
 * xwl_stubs.c) and applied here, with the serial of the last wl_pointer.enter, when the pointer enters
 * the window or when the window's cursor changes while the pointer is in it.  A window with no cursor
 * defined gets the default arrow.
 *
 * The cursor is requested with the wp_cursor_shape_v1 protocol (offered by current compositors: wlroots
 * ones such as sway and river, KWin, Mutter, Hyprland...), so the compositor draws the shape with its own
 * theme and at the right size and libxwl needs no cursor theme or cursor image of its own.  Without the
 * protocol (an old compositor, or wayland-protocols < 1.32 at build time) no cursor is requested and the
 * compositor decides what to show.
 * Only the X cursors that wmcoincoin uses are mapped: text caret (XC_xterm), hand (XC_hand1/2),
 * arrow (XC_left_ptr, XC_arrow), wait (XC_watch) and crosshair (XC_tcross, XC_cross, XC_crosshair);
 * anything else is the arrow.
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <wayland-client.h>
#ifdef HAVE_CURSOR_SHAPE
#include "cursor-shape-client-protocol.h"
#endif
#include "xwl_priv.h"

enum { SH_DEFAULT, SH_TEXT, SH_POINTER, SH_WAIT, SH_CROSSHAIR, SH_N };

static int shape_of(Cursor c)
{
  if (!c) return SH_DEFAULT;
  switch ((unsigned)(c - 1)) {
  case XC_xterm: return SH_TEXT;
  case XC_hand1: case XC_hand2: return SH_POINTER;
  case XC_watch: return SH_WAIT;
  case XC_tcross: case XC_cross: case XC_crosshair: return SH_CROSSHAIR;
  default: return SH_DEFAULT;
  }
}

void xwl_cursor_global(XwlDisplay *d, struct wl_registry *reg, uint32_t name, const char *iface, uint32_t ver)
{
  (void)ver;
#ifdef HAVE_CURSOR_SHAPE
  if (!strcmp(iface, "wp_cursor_shape_manager_v1"))
    d->cs_mgr = wl_registry_bind(reg, name, &wp_cursor_shape_manager_v1_interface, 1);
#else
  (void)d; (void)reg; (void)name; (void)iface;
#endif
}

#ifdef HAVE_CURSOR_SHAPE
static uint32_t cs_enum(int shape)
{
  switch (shape) {
  case SH_TEXT: return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_TEXT;
  case SH_POINTER: return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_POINTER;
  case SH_WAIT: return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_WAIT;
  case SH_CROSSHAIR: return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_CROSSHAIR;
  default: return WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT;
  }
}
#endif

void xwl_cursor_apply(XwlDisplay *d)
{
  int shape;
  if (!d->pointer || !d->ptr_win || !d->ptr_enter_serial) return;
  shape = shape_of(d->ptr_win->cursor);
  if (shape + 1 == d->cur_applied_shape && d->ptr_enter_serial == d->cur_applied_serial) return;
  d->cur_applied_shape = shape + 1; d->cur_applied_serial = d->ptr_enter_serial;
  (void)shape;
#ifdef HAVE_CURSOR_SHAPE
  if (d->cs_mgr && !d->cs_dev)
    d->cs_dev = wp_cursor_shape_manager_v1_get_pointer((struct wp_cursor_shape_manager_v1 *)d->cs_mgr, d->pointer);
  if (d->cs_dev) {
    wp_cursor_shape_device_v1_set_shape((struct wp_cursor_shape_device_v1 *)d->cs_dev, d->ptr_enter_serial, cs_enum(shape));
    if (xwl_debug_on()) fprintf(stderr, "xwl: cursor shape %d (cursor-shape-v1)\n", shape);
  }
#endif
}

void xwl_cursor_pointer_gone(XwlDisplay *d)
{
#ifdef HAVE_CURSOR_SHAPE
  if (d->cs_dev) { wp_cursor_shape_device_v1_destroy((struct wp_cursor_shape_device_v1 *)d->cs_dev); d->cs_dev = NULL; }
#endif
  d->cur_applied_shape = 0; d->cur_applied_serial = 0;
}
