#include "global.h"
#include "scrollcoin.h"
#include "coin_util.h"
#include "coin_xutil.h"

#include <libintl.h>
#define _(String) gettext(String)


RGBAContext *xctx;

#define BT_H 10

#define IS_IN_BTUP(sc,mx,my) (IS_INSIDE((mx),(my), \
  				        sc->x, sc->y, \
				        sc->x + SC_W - 1, sc->y + BT_H-1))
#define IS_IN_BTDN(sc,mx,my) (IS_INSIDE(mx,my, \
					sc->x, sc->y + sc->h - BT_H + 1, \
					sc->x + SC_W - 1, sc->y + sc->h - 1))
#define IS_IN_BAR(sc,mx,my) (IS_INSIDE(mx,my, \
				       sc->x, sc->y + sc->pixel_pos, \
				       sc->x + SC_W - 1, sc->y + sc->pixel_pos + sc->bar_sz - 1))

struct _ScrollCoin {
  int vmin, vmax;
  int pos, pixel_pos;
  int requested_pos;

  int x,y,h;
  int dragging, drag_pos0, drag_y0, drag_pixel_pos0;
  int bar_sz;

  Pixmap pix;

  enum { BT_UP, BT_DOWN, BT_BAR, BT_NONE} bt_state; 

  unsigned long scroll_bg_pixel;
  unsigned long scroll_bg_light_pixel;
  unsigned long scroll_bg_dark_pixel;
  unsigned long scroll_arrow_normal_pixel;
  unsigned long scroll_arrow_emphasized_pixel;
  
  unsigned long scroll_bar_pixel;
  unsigned long scroll_bar_light_pixel;
  unsigned long scroll_bar_dark_pixel;
};



static void
scrollcoin_draw_btup(ScrollCoin *sc)
{
  int y0, y1;

  y0 = 1; y1 = BT_H-1;
  if (sc->bt_state != BT_UP) {
    XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bg_light_pixel);
    XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 1, y0, SC_W-2, y0);  
    XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 1, y0, 1, y1);
  } else {
    XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bg_dark_pixel);
    XFillRectangle(xctx->dpy, sc->pix, xctx->copy_gc, 1, y0, SC_W-2, BT_H-1);
  }

  if (sc->pos != sc->vmin) {
    XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_arrow_normal_pixel);
  } else {
    XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_arrow_emphasized_pixel);
  }
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 4, y0+3, 5, y0+3);
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 3, y0+4, 6, y0+4);
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 2, y0+5, 7, y0+5);
}

static void
scrollcoin_draw_btdn(ScrollCoin *sc)
{
  int y0, y1;

  y0 = sc->h - BT_H; y1 = sc->h-2;
  if (sc->bt_state != BT_DOWN) {
    XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bg_light_pixel);
    XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 1, y0, SC_W-2, y0);
    XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 1, y0, 1, y1);
  } else {
    XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bg_dark_pixel);
    XFillRectangle(xctx->dpy, sc->pix, xctx->copy_gc, 1, y0, SC_W-2, BT_H-1);
  }

  if (sc->pos != sc->vmax) {
    XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_arrow_normal_pixel);
  } else {
    XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_arrow_emphasized_pixel);
  }
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 4, y1-3, 5, y1-3);
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 3, y1-4, 6, y1-4);
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 2, y1-5, 7, y1-5);
}

static void
scrollcoin_draw_bar(ScrollCoin *sc)
{
  int by0,by1;

  if (sc->h < 2*BT_H+4) return;

  by0 = sc->pixel_pos; by1 = by0 + sc->bar_sz -1;

  XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bg_dark_pixel);
  XFillRectangle(xctx->dpy, sc->pix, xctx->copy_gc, 0, 0, SC_W, sc->h);

  XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bg_pixel);
  XFillRectangle(xctx->dpy, sc->pix, xctx->copy_gc, 1, 1, SC_W-2, sc->h-2);
  
  /* lignes sombres au-dessus et en dessous des deux boutons */
  XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bg_dark_pixel);
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 1, BT_H, SC_W-2, BT_H);  
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 1, sc->h - BT_H-1, SC_W-2, sc->h - BT_H-1);


  XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bar_dark_pixel);
  XDrawRectangle(xctx->dpy, sc->pix, xctx->copy_gc, 0, by0, SC_W, sc->bar_sz);

  XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bar_light_pixel);
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 1, by0+1, SC_W-2, by0+1);  
  XDrawLine(xctx->dpy, sc->pix, xctx->copy_gc, 1, by0+1, 1, by1);

  XSetForeground(xctx->dpy, xctx->copy_gc, sc->scroll_bar_pixel);
  XFillRectangle(xctx->dpy, sc->pix, xctx->copy_gc, 2, by0+2, SC_W-3, sc->bar_sz-2);
}

static void
scrollcoin_draw(ScrollCoin *sc)
{
  scrollcoin_draw_bar(sc);
  scrollcoin_draw_btup(sc);
  scrollcoin_draw_btdn(sc);  
}

void
scrollcoin_update_pixmap(ScrollCoin *sc)
{
  assert(sc);
  assert(sc->h > 0);
  if (sc->pix != None) { XFreePixmap(xctx->dpy, sc->pix); sc->pix = None; }
  
  sc->pix = XCreatePixmap(xctx->dpy, xctx->drawable, SC_W, sc->h, xctx->depth); 
  assert(sc->pix != None);

  scrollcoin_draw(sc);
}


static void
scrollcoin_compute_bar_size(ScrollCoin *sc) {
  int npos;

  sc->bar_sz = -1;

  if (sc->vmin > sc->vmax) { return;}
  assert(sc->pos >= sc->vmin && sc->pos <= sc->vmax);

  npos = sc->vmax - sc->vmin + 1;
  sc->bar_sz = MAX((sc->h - 2*BT_H - 1) / npos,30);
  /*  printf("bar_sz <- %d\n",sc->bar_sz); */
}

static void
scrollcoin_compute_pixel_pos(ScrollCoin *sc, int pos) {
  int npos;
  int nxpos;
  assert(sc->bar_sz > 1);

  nxpos = (sc->h - sc->bar_sz - 2*BT_H);
  npos = sc->vmax - sc->vmin + 1;
  
  if (pos != sc->vmax) {
    sc->pixel_pos = (int)(0.5+(((float)(pos - sc->vmin))/npos * nxpos)) + BT_H;
  } else {
    sc->pixel_pos = sc->h - BT_H - sc->bar_sz - 1;
  }
  

  /*  printf("pos=%d, vmin=%d, vmax=%d, nxpos = %d, pixel_pos = %d\n",
      pos, sc->vmin, sc->vmax, nxpos, sc->pixel_pos);*/
}

void
scrollcoin_set_pos(ScrollCoin *sc, int pos)
{
  assert(sc);
  assert(sc->vmin <= sc->vmax);
  sc->pos = MIN(MAX(pos, sc->vmin), sc->vmax);
}


/* fonctions publiques */

void
scrollcoin_build(RGBAContext *rgbactx)
{
  xctx = rgbactx;
}

#define GET_BICOLOR(x) (opaque_color ? _IRGB2PIXEL(xctx, x.transp) : _IRGB2PIXEL(xctx, x.opaque))
void
scrollcoin_change_colors(ScrollCoin *sc, int opaque_color) {
  sc->scroll_bg_pixel               = GET_BICOLOR(Prefs.sc_bg_color);
  sc->scroll_bg_light_pixel         = GET_BICOLOR(Prefs.sc_bg_light_color);
  sc->scroll_bg_dark_pixel          = GET_BICOLOR(Prefs.sc_bg_dark_color);
  sc->scroll_arrow_normal_pixel     = GET_BICOLOR(Prefs.sc_arrow_normal_color);
  sc->scroll_arrow_emphasized_pixel = GET_BICOLOR(Prefs.sc_arrow_emphasized_color);
  
  sc->scroll_bar_pixel              = GET_BICOLOR(Prefs.sc_bar_color);
  sc->scroll_bar_light_pixel        = GET_BICOLOR(Prefs.sc_bar_light_color);
  sc->scroll_bar_dark_pixel         = GET_BICOLOR(Prefs.sc_bar_dark_color);
}


ScrollCoin *
scrollcoin_create(int vmin, int vmax, int pos, 
		  int x, int y, int h, int opaque_color) {
  ScrollCoin *sc;

  ALLOC_OBJ(sc, ScrollCoin);
  
  sc->x = x;
  sc->y = y;
  sc->h = h;

  sc->bt_state = BT_NONE;
  sc->dragging = 0;

  sc->pix = None;
  sc->requested_pos = sc->pos;

  scrollcoin_change_colors(sc, opaque_color);

  scrollcoin_setbounds(sc, vmin, vmax);
  scrollcoin_setpos(sc, pos);

  scrollcoin_update_pixmap(sc);
  return sc;
}

void
scrollcoin_destroy(ScrollCoin *sc)
{
  if (sc->pix != None) { XFreePixmap(xctx->dpy, sc->pix); sc->pix = None; }
  free(sc);
}

void
scrollcoin_refresh(ScrollCoin *sc, Drawable d, int force_redraw)
{
  
  if (force_redraw) { scrollcoin_draw(sc); }
  XCopyArea(xctx->dpy, sc->pix, d, xctx->copy_gc, 0, 0, SC_W, sc->h, sc->x, sc->y);
}

void
scrollcoin_setpos(ScrollCoin *sc, int new_pos)
{
  if (!(new_pos >= sc->vmin && new_pos <= sc->vmax)) {
    BLAHBLAH(2,printf(_("setpos out of bounds: %d min=%d, mx=%d\n"), new_pos, sc->vmin, sc->vmax));
  }
  new_pos = MIN(MAX(new_pos, sc->vmin),sc->vmax);
  if (sc->pos != new_pos) {
    sc->pos = new_pos;
    sc->requested_pos = new_pos;
  }
  if (sc->dragging == 0) {
    scrollcoin_compute_bar_size(sc);
    scrollcoin_compute_pixel_pos(sc, sc->pos);
  }
}

int scrollcoin_get_vmax(const ScrollCoin *sc) { return sc->vmax; }
int scrollcoin_get_vmin(const ScrollCoin *sc) { return sc->vmin; }

/* renvoie zero si la position n'a pas été changée */
int
scrollcoin_read_requested_pos(ScrollCoin *sc, int *pos) 
{
  *pos = sc->requested_pos;
  if (sc->pos != sc->requested_pos) {
    return 1;
  } else return 0;
}

void
scrollcoin_setbounds(ScrollCoin *sc, int vmin, int vmax)
{
  if (vmin > vmax) vmin = vmax;
  sc->vmin = vmin; sc->vmax = vmax;
  if (sc->pos < vmin) scrollcoin_setpos(sc, vmin);
  if (sc->pos > vmax) scrollcoin_setpos(sc, vmax);
  scrollcoin_compute_bar_size(sc);
  scrollcoin_compute_pixel_pos(sc, sc->pos);
  scrollcoin_update_pixmap(sc);
}


void
scrollcoin_resize(ScrollCoin *sc, int x, int y, int h)
{
  int old_h;
  sc->x = x; sc->y = y; old_h = sc->h; sc->h = h;
  if (h != old_h) {
    scrollcoin_update_pixmap(sc);
  }
}

int
scrollcoin_handle_button_press(ScrollCoin *sc, XButtonEvent *ev, Drawable d)
{
  int mx, my;

  mx = ev->x;
  my = ev->y;
  /*  printf("clic %d %d (%d,%d,%d,%d)\n",mx,my,sc->x, sc->y, SC_W, sc->h); */
  if (mx >= sc->x && mx <= sc->x + SC_W -1 &&
      my >= sc->y && my <= sc->y + sc->h-1) {
    if (ev->button == Button1 || ev->button == Button2 || ev->button == Button3) {
      if (IS_IN_BTUP(sc,mx,my) && sc->pos != sc->vmin) {
	sc->bt_state = BT_UP; 
      } else if (IS_IN_BTDN(sc,mx,my) && sc->pos != sc->vmax) {
	sc->bt_state = BT_DOWN;
      } else if (IS_IN_BAR(sc,mx,my)) {
	sc->bt_state = BT_BAR;
	sc->drag_pos0 = sc->pos; 
	sc->drag_y0 = my; 
	sc->drag_pixel_pos0 = sc->pixel_pos;
	sc->dragging = 1;
      }
      scrollcoin_refresh(sc, d, 1);
    }
    return 1;
  } else { 
    return 0; 
  }
}

int
scrollcoin_handle_button_release(ScrollCoin *sc, XButtonEvent *ev, Drawable d)
{
  int mx, my;

  mx = ev->x;
  my = ev->y;
  if ((mx >= sc->x && mx <= sc->x + SC_W -1 &&
       my >= sc->y && my <= sc->y + sc->h-1) || sc->bt_state != BT_NONE) {
    if (sc->dragging == 0) {
      if (IS_IN_BTUP(sc,mx,my) && sc->bt_state == BT_UP) {
	if (ev->button == Button1) {
	  sc->requested_pos = MAX(sc->pos - 1, sc->vmin);
	} else if (ev->button == Button2) {
	  sc->requested_pos = MAX(sc->pos - 10, sc->vmin);
	} else if (ev->button == Button3) {
	  sc->requested_pos = sc->vmin;
	} 
      } else if (IS_IN_BTDN(sc,mx,my) && sc->bt_state == BT_DOWN) {
	if (ev->button == Button1) {
	  sc->requested_pos = MIN(sc->pos + 1, sc->vmax);
	} else if (ev->button == Button2) {
	  sc->requested_pos = MIN(sc->pos + 10, sc->vmax);
	} else if (ev->button == Button3) {
	  sc->requested_pos = sc->vmax;
	} 
      } else if (!IS_IN_BAR(sc,mx,my)) {
	int q = (my < sc->y + sc->pixel_pos) ? -1 : +1;
	if (ev->button == Button1) {
	  sc->requested_pos = MAX(MIN(sc->pos + 10*q, sc->vmax), sc->vmin);
	} else if (ev->button == Button2) {
	  sc->requested_pos = ((my-sc->y) * (sc->vmax - sc->vmin))/(sc->h-2*BT_H) + sc->vmin;
	  sc->requested_pos = MAX(MIN(sc->requested_pos, sc->vmax), sc->vmin);	  
	}
      }
    }
    if (sc->bt_state != BT_NONE) {
      sc->bt_state = BT_NONE;
    }
    scrollcoin_refresh(sc, d, 1);
    sc->dragging = 0;
    return 1;
  } else { 
    sc->dragging = 0;
    return 0; 
  }
}

int
scrollcoin_handle_motion(ScrollCoin *sc, XMotionEvent *ev, Drawable d)
{
  int my;

  my = ev->y;
  if (sc->dragging && (ev->state & Button1Mask)) {
    int decal;
    int pix_pos, pos, marge;
    float coef;

    decal = (my - sc->drag_y0);
    pix_pos = sc->drag_pixel_pos0 + decal;
    marge = sc->h - 2*BT_H - sc->bar_sz - 1;

    if (marge <= 0) return 1;

    pix_pos = MAX(pix_pos, BT_H);
    pix_pos = MIN(pix_pos, BT_H + marge);

    coef = ((float)pix_pos - BT_H)/(marge);
    pos = (int)(coef * (sc->vmax - sc->vmin) + sc->vmin);

    /*    printf("coef=%f , rmarge = %d, sc->y = %d, marge=%d pos=%d\n", coef, pix_pos - sc->y - BT_H, sc->y, marge, pos); */

    if (pos != sc->pos) {
      /*      printf("scrollcoin_handle_motion: requested_pos <- %d\n", pos); */
      sc->requested_pos = pos;
    }
    if (pix_pos != sc->pixel_pos) {
      sc->pixel_pos = pix_pos;
      scrollcoin_refresh(sc, d, 2);
    }
    return 1;
  } else {
    return 0;
  }
}
