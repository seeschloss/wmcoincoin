/* The big red button, now living in the pinnipede.
 *
 * Wayland port.  In the original wmcoincoin the red button was part of the dock applet (behind a
 * little door): holding Return, or the mouse button, pushed it down over 5 timer ticks and the
 * message was only sent if it was released once fully pressed.  The dock is gone, so the button is
 * drawn here, in the pinnipede, using the very same sprites (coin.xpm) and the very same state
 * machine (dock->red_button_press_flag / red_button_press_state, advanced by the 40ms timer in
 * wmcoincoin.c; dock_red_button_check() decides whether the message goes out).
 *
 * The sprites were designed to be OR-ed onto the dock's black hole.  On the pinnipede's light
 * background that would wash them out, so a transparency mask (black = transparent) is built once
 * from the sprite sheet instead.
 */
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <cairo.h>
#include "xwl.h"
#include "coincoin.h"
#include "pinnipede.h"
#include "dock.h"

#define RB_W 31
#define RB_H 30
#define RB_SHEET_X 128            /* frames: (128 + (state%3)*32, (state/3)*31) in dock->coinpix */
#define RB_SHEET_W 96
#define RB_SHEET_H 62
#define RB_MIN_COLUMN 38          /* the left tab column must be at least this wide to host the button */

static Pixmap rb_mask = None;
static GC rb_gc = None;
static int rb_mouse_down = 0;
static int rb_last_drawn_state = -1;

/* Build the transparency mask once: a pixel is opaque unless it is pure black. */
static int rb_init(Dock *dock)
{
  cairo_surface_t *src, *dst;
  unsigned char *sp, *dp;
  int sstride, dstride, x, y;

  if (rb_mask != None) return 1;
  if (dock->coinpix == None) return 0;
  src = xwl_surface(dock->display, dock->coinpix);
  if (!src || cairo_image_surface_get_width(src) < RB_SHEET_X + RB_SHEET_W
           || cairo_image_surface_get_height(src) < RB_SHEET_H) return 0;

  rb_mask = XCreatePixmap(dock->display, dock->pinnipede->win, RB_SHEET_W, RB_SHEET_H, 1);
  dst = xwl_surface(dock->display, rb_mask);
  cairo_surface_flush(src);
  sp = cairo_image_surface_get_data(src); sstride = cairo_image_surface_get_stride(src);
  dp = cairo_image_surface_get_data(dst); dstride = cairo_image_surface_get_stride(dst);
  for (y = 0; y < RB_SHEET_H; y++)
    for (x = 0; x < RB_SHEET_W; x++) {
      unsigned px = ((unsigned *)(sp + (size_t)y * sstride))[RB_SHEET_X + x] & 0xFFFFFF;
      dp[(size_t)y * dstride + x] = px ? 0xFF : 0x00;
    }
  cairo_surface_mark_dirty(dst);

  rb_gc = XCreateGC(dock->display, dock->pinnipede->win, 0, NULL);
  return 1;
}

/* Where the button goes: top of the left tab column, or the top-right corner if there is none. */
static int rb_pos(Dock *dock, int *x, int *y)
{
  Pinnipede *pp = dock->pinnipede;
  if (!pp || !pp->mapped) return 0;
  if (pp->zmsg_x1 >= RB_MIN_COLUMN) { *x = (pp->zmsg_x1 - RB_W) / 2; *y = 6; }
  else { *x = pp->win_width - RB_W - 20; *y = 4; }
  return 1;
}

void pp_redbutton_draw(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  int x, y, st, fx, fy;

  if (!rb_pos(dock, &x, &y) || !rb_init(dock)) return;
  st = dock->red_button_press_state;
  if (st < 0) st = 0;
  if (st > 5) st = 5;
  fx = st % 3; fy = st / 3;
  rb_last_drawn_state = dock->red_button_press_state;

  /* dark bezel, like the hole of the dock */
  XSetClipMask(dock->display, rb_gc, None);
  XSetForeground(dock->display, rb_gc, 0x2c2c2c);
  XFillArc(dock->display, pp->win, rb_gc, x - 2, y - 2, RB_W + 3, RB_H + 3, 0, 360 * 64);

  /* the sprite, through the mask (mask origin chosen so that frame (fx,fy) lands on (x,y)) */
  XSetClipMask(dock->display, rb_gc, rb_mask);
  XSetClipOrigin(dock->display, rb_gc, x - fx * 32, y - fy * 31);
  XCopyArea(dock->display, dock->coinpix, pp->win, rb_gc,
            RB_SHEET_X + fx * 32, fy * 31, RB_W, RB_H, x, y);
  XSetClipMask(dock->display, rb_gc, None);
}

/* called by the 40ms timer: only redraw when the animation moved */
void pp_redbutton_tick(Dock *dock)
{
  if (dock->red_button_press_state != rb_last_drawn_state && dock->pinnipede && dock->pinnipede->mapped)
    pp_redbutton_draw(dock);
}

/* mouse: returns 1 if the event was consumed */
int pp_redbutton_press(Dock *dock, int mx, int my, unsigned state)
{
  Pinnipede *pp = dock->pinnipede;
  int x, y;
  if (!rb_pos(dock, &x, &y)) return 0;
  if (mx < x - 2 || mx >= x + RB_W + 2 || my < y - 2 || my >= y + RB_H + 2) return 0;
  if (!editw_ismapped(dock->editw)) {          /* nothing to send yet: open the editor */
    editw_show(dock, pp->active_tab ? pp->active_tab->site->prefs : 0, 0);
  } else {
    dock->post_anonyme = (state & ShiftMask) ? 1 : 0;
    dock->red_button_press_flag = 1;           /* the timer pushes the button down */
    rb_mouse_down = 1;
  }
  return 1;
}

int pp_redbutton_release(Dock *dock)
{
  if (!rb_mouse_down) return 0;
  rb_mouse_down = 0;
  if (dock_red_button_check(dock)) editw_hide(dock, dock->editw);
  return 1;
}
