/*
    wmCoinCoin, the stupidest WindowMaker dock applet

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA


 */

/*
  rcsid=$Id: balloon.c,v 1.12 2004/02/29 19:01:26 pouaite Exp $
  ChangeLog:
  $Log: balloon.c,v $
  Revision 1.12  2004/02/29 19:01:26  pouaite
  et hop

  Revision 1.11  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.10  2003/07/20 22:22:28  pouaite
  ce commit est dedie a Pierre Tramo

  Revision 1.9  2003/06/25 20:18:21  pouaite
  support xinerama qui marche

  Revision 1.8  2003/06/24 22:27:56  pouaite
  speciale dedicace a nos amis de l'ile de beaute

  Revision 1.7  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.6  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

  Revision 1.5  2002/03/03 10:10:04  pouaite
  bugfixes divers et variés

  Revision 1.4  2001/12/18 12:43:37  pouaite
  ajout de l'option de la fonte des ballons d'aide (pour mr. imr !) + bugfix d'une connerie assez naze dans la gestion du nom du fichier d'options (merci glandium de me l'avoir signalé)

  Revision 1.3  2001/12/16 16:46:12  pouaite
  Clippouille joins C0IN C0IN

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include "coincoin.h"
#include "coin_xutil.h"
#include "dock.h"

#include <libintl.h>
#define _(String) gettext(String)

/* TRES largement inspire de balloon.c de Windowmaker ... */

struct _Balloon {
  Window win;
  Pixmap pix;
  GC monoGC;
  PicoHtml *ph;
  int mapped;
  unsigned long bgpixel;

  Pixmap imgpix; /* affichage d'un pixmap à gauche (pour le clippy dans editwin ! ) */
  int imgpix_w, imgpix_h;
};


#define TOP	0
#define BOTTOM	1
#define LEFT	0
#define RIGHT	2

#define 	SPACE	12

void
balloon_build(Dock *dock)
{
  XSetWindowAttributes wa;
  Balloon *b;

  ALLOC_OBJ(b, Balloon);

  /* couleur de fond */
  b->bgpixel = RGB2PIXEL(255,231,186);
  
  /* creation fenetre (une bonne fois pour toutes) */
  b->win = XCreateSimpleWindow (dock->display, RootWindow(dock->display,dock->screennum), 0, 0, 64, 64, 0,
				BlackPixel(dock->display, dock->screennum),
				b->bgpixel);
  
  //wa.background_pixmap = ButtonBarImage ;
  wa.event_mask =
    ExposureMask | 
    PointerMotionMask | 
    EnterWindowMask | 
    LeaveWindowMask;
  wa.override_redirect = True ;
  wa.save_under = True;
  XChangeWindowAttributes (dock->display, b->win,
			   //CWBackPixmap | 
			   CWSaveUnder | CWEventMask | CWOverrideRedirect, &wa);
  //  b->gc = XCreateGC(dock->display, b->win, 0, NULL);
  //  XSetFont(display, b->gc, tw->fn_base->fid);
  b->monoGC = None;
  b->mapped = 0;
  b->imgpix = None; b->imgpix_w = b->imgpix_h = 0;

  b->ph = picohtml_create(dock, Prefs.balloon_fn_family, Prefs.balloon_fn_size, 0);
  picohtml_set_parag_skip(b->ph, 1.0);
  picohtml_set_line_skip(b->ph, 1.0);
  picohtml_set_parag_indent(b->ph, 0);
  picohtml_set_tabul_skip(b->ph, 10);

  dock->balloon = b;
}

#ifdef TEST_MEMLEAK
void
balloon_destroy(Dock *dock) {
  Balloon *b = dock->balloon;
  
  if (b->imgpix != None) XFreePixmap(dock->display, b->imgpix);
  XDestroyWindow(dock->display, b->win);
  if (b->ph) { picohtml_destroy(dock->display, b->ph); b->ph = NULL; }
  free(dock->balloon); dock->balloon = NULL;
}
#endif

static void
balloon_draw_frame(Display *dpy, Pixmap pix, GC gc, int x, int y, int w, int h, int side)
{
  //int rad = 6; //h*3/10;
  //XPoint pt[3];
  
  /*
    XFillArc(dpy, pix, gc, x, y, rad, rad, 90*64, 90*64);
    XFillArc(dpy, pix, gc, x, y+h-1-rad, rad, rad, 180*64, 90*64);
    
    XFillArc(dpy, pix, gc, x+w-1-rad, y, rad, rad, 0*64, 90*64);
    XFillArc(dpy, pix, gc, x+w-1-rad, y+h-1-rad, rad, rad, 270*64, 90*64);
    XFillRectangle(dpy, pix, gc, x, y+rad/2, w, h-rad);
    XFillRectangle(dpy, pix, gc, x+rad/2, y, w-rad, h);
  */
  
  XFillRectangle(dpy, pix, gc, x, y, w, h);
  /*
  if (side & BOTTOM) {
    pt[0].y = y+h-1;
    pt[1].y = y+h-1+SPACE;
    pt[2].y = y+h-1;
  } else {
    pt[0].y = y;
    pt[1].y = y-SPACE;
    pt[2].y = y;
  }
  if (side & RIGHT) {
    pt[0].x = x+w-h+2*h/16;
    pt[1].x = x+w-h+11*h/16;
    pt[2].x = x+w-h+7*h/16;
  } else {
    pt[0].x = x+h-2*h/16;
    pt[1].x = x+h-11*h/16;
    pt[2].x = x+h-7*h/16;
  }
  XFillPolygon(dpy, pix, gc, pt, 3, Convex, CoordModeOrigin);
  */
  side=0;
}

static void
balloon_makepixmap(Dock *dock, Balloon *b, int bx, int by, int width, int height, int side, Pixmap *pix, Pixmap *mask)
{
    Pixmap bitmap;
    Pixmap pixmap;
    int x, y;

    bitmap = XCreatePixmap(dock->display, dock->rootwin, width+SPACE, height+SPACE, 1);

    if (!b->monoGC) {
      b->monoGC = XCreateGC(dock->display, bitmap, 0, NULL);
    }
    
    if (side & BOTTOM) {
	y = 0;
    } else {
	y = SPACE;
    }
    x = 0;

    XSetForeground(dock->display, b->monoGC, 0); 
    XFillRectangle(dock->display, bitmap, b->monoGC, 0, 0, width+SPACE, height+SPACE);
    pixmap = XCreatePixmap(dock->display, dock->rootwin, width+SPACE, height+SPACE,
			   DefaultDepth(dock->display,dock->screennum));
                   
    XSetForeground(dock->display, dock->NormalGC, BlackPixel(dock->display,dock->screennum));
    XFillRectangle(dock->display, pixmap, dock->NormalGC, 0, 0, width+SPACE, height+SPACE);
    XSetForeground(dock->display, dock->NormalGC, b->bgpixel); //WhitePixel(dock->display,dock->screennum));
    balloon_draw_frame(dock->display, pixmap, dock->NormalGC, x+1, y+1, width-2, height-2, side);

    XSetForeground(dock->display, b->monoGC, 1);
    balloon_draw_frame(dock->display, bitmap, b->monoGC, x, y, width, height, side);

    *mask = bitmap;
    *pix = pixmap;
    bx = by = 0;
}

void
balloon_hide(Dock *dock)
{
  Balloon *b = dock->balloon;

  BLAHBLAH(3, printf(_("hide balloon\n")));
  if (b->mapped) {
    b->mapped = 0;
    
    picohtml_freetxt(b->ph);
    XUnmapWindow(dock->display, b->win);
    assert(b->pix);
    XFreePixmap(dock->display, b->pix); b->pix = None;

    b->imgpix = None; b->imgpix_w = b->imgpix_h = 0;
  }
}

/* ceci ressemble à WindowMaker/src/balloon.c ... 
 [x,y,h,w] = position & dimension de la zone rectangulaire que doit designer
 le ballon
*/
void
balloon_show(Dock *dock, int x, int y, int h, int w, const char *text, int bwidth)
{
  int width;
  int height;
  Pixmap mask;
  int side = 0;
  int ty;
  int bx, by;
  Balloon *b = dock->balloon;
  int xiscr;
  int sx,sy,sw,sh;
  xiscr = MAX(dock_find_xiscreen_num(dock,x,y),0);
  sx = dock->xiscreen[xiscr].x_org; sw = dock->xiscreen[xiscr].width;
  sy = dock->xiscreen[xiscr].y_org; sh = dock->xiscreen[xiscr].height;
  if (b->mapped) {
    balloon_hide(dock);
  }
  assert(b->pix == None);
  assert(picohtml_isempty(b->ph));
  
  width = bwidth; //300; //w - 10;
  picohtml_parse(b->ph, text, width);
  picohtml_gettxtextent(b->ph, &width, &height);
  
  height = MAX(height, b->imgpix_h);
  height += 7;
  width += 10 + b->imgpix_w;
  
  if (height < 16)
    height = 16;
  if (width < height)
    width = height;
  

  if (x + width > sx + sw) {
    side = RIGHT;
    bx = x - width + w/2;
    if (bx < sx)
      bx = sx;
  } else {
    side = LEFT;
    bx = x + w/2;
  }
  if (bx + width > sx+sw)
    bx = sx+sw - width;

  if (y - (height + SPACE) < sy) {
    side |= TOP;
    by = y+h-1;
    ty = SPACE;
  } else {
    side |= BOTTOM;
    by = y - (height + SPACE);
    ty = 0;
  }
  //printf("ballon x=%d y=%d width=%d, h=%d side=%d, x=%d y=%d sx=%d sy=%d sw=%d sh=%d\n", bx, by, width, height,side,x,y,sx,sy,sw,sh);

  XSetForeground(dock->display, dock->NormalGC, BlackPixel(dock->display, dock->screennum));
  
  balloon_makepixmap(dock, b, bx, by, width, height, side, &b->pix, &mask);

  picohtml_render(b->ph, b->pix, 5+b->imgpix_w, 3+ty);

  if (b->imgpix != None) {
    XCopyArea(dock->display, b->imgpix, b->pix, dock->NormalGC, 0, 0, b->imgpix_w, b->imgpix_h, 3, 3);
  }

  XSetWindowBackgroundPixmap(dock->display, b->win, b->pix);

  XResizeWindow(dock->display, b->win, width, height+SPACE);
  XShapeCombineMask(dock->display, b->win, ShapeBounding, 0, 0, mask,
		    ShapeSet);
  XFreePixmap(dock->display, mask);
  XMoveWindow(dock->display, b->win, bx, by);
  XMapRaised(dock->display, b->win);
    
  b->mapped = 1;
}

void
balloon_show_with_image(Dock *dock, int x, int y, int h, int w, const char *text, int bwidth, Pixmap image, int img_w, int img_h)
{
  Balloon *b = dock->balloon;
  b->imgpix = image;
  b->imgpix_w = img_w;
  b->imgpix_h = img_h;
  balloon_show(dock,x,y,h,w,text,bwidth);
}

int 
balloon_ismapped(Dock *dock)
{
  return dock->balloon->mapped;
}

unsigned check_key=0;

void 
balloon_check_event(Dock *dock, XEvent *event)
{
  Balloon *b = dock->balloon;

  if (b->mapped) {
    switch (event->type) {
    case ButtonPress:
    case ButtonRelease:
    case MotionNotify:
      //    case EnterNotify:
      //case LeaveNotify:
      balloon_hide(dock);
      break;
    case KeyPress:
      {
        if (event->xkey.keycode != check_key)
          balloon_hide(dock);
        check_key = 0;
      } break;
    }
  }
}

void
balloon_disable_key(Dock *dock UNUSED, unsigned keycode) {
  check_key=keycode;
}

int
balloon_test_nomsg(Dock *dock, int x, int y, int bcnt, int bx, int by, int bw, int bh) {
  if (dock->mouse_cnt >= (bcnt)) {
    if (IS_INSIDE(x,y,(bx),(by),(bx)+(bw)-1,(by)+(bh)-1)) {
      return 1;
    }
  }
  return 0;
}


int
balloon_test(Dock *dock, int x, int y, int winx, int winy, int bcnt, int bx, int by, int bw, int bh, const char *btxt) {
  if (balloon_test_nomsg(dock,x,y,bcnt,bx,by,bw,bh)) {
    balloon_show(dock, winx+(bx), winy+(by), (bw), (bh), (btxt), 300);
    return 1;
  }
  return 0;
}

int
balloon_test_with_image(Dock *dock, int x, int y, int winx, int winy, int bcnt, int bx, int by, int bw, int bh, const char *btxt, Pixmap image, int img_w, int img_h) {
  if (dock->mouse_cnt >= (bcnt)) {
    if (IS_INSIDE(x,y,(bx),(by),(bx)+(bw)-1,(by)+(bh)-1)) {
      balloon_show_with_image(dock, winx+(bx), winy+(by), (bw), (bh), (btxt), 300, image, img_w, img_h);
      return 1;
    }
  }
  return 0;
}
