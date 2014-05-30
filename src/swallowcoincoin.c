#include "coincoin.h"
#include "coin_xutil.h"
#include "pinnipede.h"
#include "fontcoincoin.h"

Window sw_hostwin(Dock *dock) {
  if (Prefs.auto_swallow)
    return dock->pp_win;
  else {
    assert(dock->pinnipede);
    return dock->pinnipede->win;
  }    
}

SwallowedApp * sw_swallow_win_id(Dock *dock, Window wid) {
  Window ew = sw_hostwin(dock);
  SwallowedApp *sa;
  ALLOC_OBJ(sa, SwallowedApp);
  sa->winx = sa->winy = 0;
  sa->framex = sa->framey = sa->framew = sa->frameh = 0;
  sa->win = wid;
  get_window_dimensions(dock->display, wid, &sa->winw, &sa->winh);
  sa->name = NULL; //name_of_window(wid);
  XReparentWindow(dock->display, wid, ew, 0, 0);
  XMapWindow(dock->display,wid);
  sa->next = dock->swallow->apps;
  dock->swallow->apps = sa;
  dock->swallow->nb_swallowed++;
  if (wid == DOCK_WIN(dock)) dock->swallow->self = sa;
  BLAHBLAH(1,printf("swallowed win %8x dimensions %dx%d\n", (int)wid, sa->winw, sa->winh));
  dock->swallow->layout_dirty = 1;
  return sa;
}

void sw_layout_dockapps(Dock *dock, int x0, int y0, int x1, int y1) {
  //Window ew = sw_hostwin(dock);
  SwallowCoincoin *sw = dock->swallow;;
  SwallowedApp *sa;
  int y = y1, yspacing = 2, count = 0;
  if (sw->x0 != x0 || sw->x1 != x1 || sw->y0 != y0 || sw->y1 != y1 || sw->layout_dirty) {
    sw->layout_dirty = 0;
    sw->x0 = x0; sw->x1 = x1; sw->y0 = y0; sw->y1 = y1;
    for (sa = sw->apps; sa; sa = sa->next, ++count) {
      int nexty = y - (sa->winh);
      if (nexty >= y0) y = nexty; else break;
    }
    if (count) {
      if (y != y0) 
        yspacing = (y1-y0+1) / ((y - y0) * count);
      else yspacing = 0;
      
      y = y0 + MIN((x1-x0-64)/2, (y1-y0-64)/2);
      //y = y1;
      for (sa = sw->apps; sa; sa = sa->next) {
        sa->framex = x0;
        sa->framey = y; //- yspacing - sa->winh;
        sa->framew = x1 - x0 + 1;
        sa->frameh = sa->winh + yspacing;
        sa->winx = x0 + (sa->framew - sa->winw)/2;
        sa->winy = sa->framey + yspacing/2;
        XMoveWindow(dock->display, sa->win, sa->winx, sa->winy);
        //y -= sa->frameh;
        y += sa->frameh;
      }
    }
  }
}

void sw_redraw(Dock *dock) {
  SwallowCoincoin *sw = dock->swallow;;
  Window ew = sw_hostwin(dock);
  Pinnipede *pp = dock->pinnipede;
  if (pp_ismapped(dock) && pp->use_minibar && pp->tabs_pos == PPT_LEFT && sw->y1 >= sw->y0) {
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(dock->pinnipede->minib_color));
    XFillRectangle(dock->display, ew, dock->NormalGC, sw->x0, sw->y0, 
                   sw->x1 - sw->x0, sw->y1 - sw->y0 + 1);
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(dock->pinnipede->minib_dark_color));
    XDrawLine(dock->display, ew, dock->NormalGC, sw->x1, sw->y0, sw->x1, sw->y1);
  }
}


/* sluuuuuuuuuuuuuuuuuuurp */
void
swallower_autoswallow(Dock *dock) {
  dock->swallow->self = sw_swallow_win_id(dock, DOCK_WIN(dock));
}

int
swallower_is_autoswallowed(Dock *dock) {
  return dock->swallow->self != NULL;
}

void
swallower_init(Dock *dock) {
  SwallowCoincoin *sw;
  ALLOC_OBJ(dock->swallow, SwallowCoincoin);
  sw = dock->swallow;
  sw->hostwin = dock->pp_win;
  sw->x0 = sw->x1 = 0;
  sw->y0 = sw->y1 = 0;
  sw->nb_swallowed = 0; sw->apps = NULL;
  sw->layout_dirty = 0;
  sw->self = NULL;
}
