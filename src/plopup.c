#include "coincoin.h"
#include "dock.h"
#include "coin_xutil.h"

typedef struct _Plopup_entry {
  int  id;
  PicoHtml *ph;
  struct _Plopup_entry *next;
  int ph_width, ph_height;
  int x, y, w, h;
  int is_separ;
} Plopup_entry;

struct _Plopup {
  Plopup_entry *first;
  Window win;
  Pixmap pix;
  Plopup_entry *active_entry;
  plopup_callback_t callback;
  int win_width, win_height, win_xpos, win_ypos;
  char *descr;
  int descr_width, descr_height;
  unsigned long bgpixel, lightpixel, darkpixel;
};

void
plopup_build(Dock *dock)
{
  Plopup *pup;
  ALLOC_OBJ(pup, Plopup);
  pup->first = NULL;
  pup->win = None;
  pup->pix = None;
  dock->plopup = pup;
  pup->bgpixel = IRGB2PIXEL(0xa0a0a0);
  pup->lightpixel = IRGB2PIXEL(lighten_color(0xa0a0a0, 1.2));
  pup->darkpixel = IRGB2PIXEL(lighten_color(0xa0a0a0, .6));
  pup->descr = NULL;
}


static void 
plopup_push(Plopup *pup, Plopup_entry *n)
{
  Plopup_entry *e;
  
  for (e = pup->first; e; e = e->next) {
    if (e->next == NULL) break;
  }
  if (e) {
    e->next = n;
  } else {
    pup->first = n;
  }
}

void
plopup_pushentry(Dock *dock, char *txt, int id)
{
  Plopup *pup = dock->plopup;
  Plopup_entry *n;
  assert(pup->win == None);
  
  ALLOC_OBJ(n, Plopup_entry);

  n->id = id;
  n->next = NULL;
  n->is_separ = 0;
  n->ph = picohtml_create(dock, Prefs.balloon_fn_family, Prefs.balloon_fn_size, 0);
  picohtml_set_parag_skip(n->ph, 1.0);
  picohtml_set_line_skip(n->ph, 1.0);
  picohtml_set_parag_indent(n->ph, 0);
  picohtml_set_tabul_skip(n->ph, 10);
  picohtml_parse(n->ph, txt, 300);
  picohtml_gettxtextent(n->ph, &n->ph_width, &n->ph_height);
  plopup_push(pup, n);
}

void
plopup_pushsepar(Dock *dock)
{
  Plopup *pup = dock->plopup;
  Plopup_entry *n;

  for (n = pup->first; n && n->next; n = n->next) /*plop*/ ;
  if (n==NULL || n->is_separ) return;

  ALLOC_OBJ(n, Plopup_entry);

  n->id = -1;
  n->next = NULL;
  n->is_separ = 1;
  n->ph = NULL;
  n->ph_width  = 0;
  n->ph_height = 6;
  plopup_push(pup, n);
}

void
plopup_set_description(Dock *dock, char *txt) {
  Plopup *pup = dock->plopup;
  assert(pup->win == None);
  pup->descr = strdup(txt);
}

void
plopup_show(Dock *dock, int winx, int winy, plopup_callback_t cback)
{
  Plopup *pup = dock->plopup;
  Plopup_entry *e;
  XSetWindowAttributes wa;
  int y;
  int xiscrnum = 0;
  int scrw, scrh, scrx, scry;
  PicoHtml *ph_descr = NULL;

  assert(pup->win == None);
  if (pup->first == NULL) return;

  pup->callback = cback;

  xiscrnum = MAX(dock_find_xiscreen_num(dock, winx, winy), 0);

  pup->win_xpos = winx;
  pup->win_ypos = winy;
  pup->win_width = 0;
  pup->win_height = 0;
  for (e = pup->first; e; e = e->next) {
    pup->win_width = MAX(pup->win_width, e->ph_width);
    pup->win_height += e->ph_height;
  }

  pup->descr_width = pup->win_width;
  pup->descr_height = 0;
  if (pup->descr) {
    ph_descr = picohtml_create(dock, Prefs.balloon_fn_family, Prefs.balloon_fn_size, 0);
    picohtml_parse(ph_descr, pup->descr, pup->win_width);
    picohtml_gettxtextent(ph_descr, &pup->descr_width, &pup->descr_height);
  }

  pup->win_width += 8;
  pup->win_height += 7;

  pup->win_height += pup->descr_height;
  pup->win_ypos -= pup->descr_height;

  scrw = dock->xiscreen[xiscrnum].width; //WidthOfScreen(XScreenOfDisplay(dock->display, dock->screennum));
  scrh = dock->xiscreen[xiscrnum].height; //HeightOfScreen(XScreenOfDisplay(dock->display, dock->screennum));
  scrx = dock->xiscreen[xiscrnum].x_org;
  scry = dock->xiscreen[xiscrnum].y_org;
  if (pup->win_xpos < scrx) pup->win_xpos = scrx;
  if (pup->win_ypos < scry) pup->win_ypos = scry;
  if (pup->win_xpos + pup->win_width > scrx + scrw) pup->win_xpos = scrx + scrw - pup->win_width;
  if (pup->win_ypos + pup->win_height > scry + scrh) pup->win_ypos = scry + scrh - pup->win_height;

  pup->active_entry = NULL;
  pup->win = XCreateSimpleWindow (dock->display, RootWindow(dock->display,dock->screennum), 
				  0, 0, pup->win_width, pup->win_height, 0,
				  BlackPixel(dock->display, dock->screennum),
				  pup->bgpixel);
  wa.event_mask = ExposureMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask;
  wa.override_redirect = True ;
  wa.save_under = True;
  XChangeWindowAttributes (dock->display, pup->win, CWSaveUnder | CWEventMask | CWOverrideRedirect, &wa);
  pup->pix = XCreatePixmap(dock->display, pup->win, pup->win_width, pup->win_height,
			   DefaultDepth(dock->display,dock->screennum));
  XSetForeground(dock->display, dock->NormalGC, pup->bgpixel);
  XFillRectangle(dock->display, pup->pix, dock->NormalGC, 0, 0, pup->win_width, pup->win_height);
  XSetForeground(dock->display, dock->NormalGC, pup->lightpixel);
  XDrawLine(dock->display, pup->pix, dock->NormalGC, 0, 0, pup->win_width-1, 0);
  XDrawLine(dock->display, pup->pix, dock->NormalGC, 0, 0, 0, pup->win_height-1);
  XSetForeground(dock->display, dock->NormalGC, pup->darkpixel);
  XDrawLine(dock->display, pup->pix, dock->NormalGC, pup->win_width-1, 0, pup->win_width-1, pup->win_height-1);
  XDrawLine(dock->display, pup->pix, dock->NormalGC, 0, pup->win_height-1, pup->win_width-1, pup->win_height-1);

  if (ph_descr) {
    picohtml_render(ph_descr, pup->pix, 4, 0);
  }
  for (y = 1+pup->descr_height, e = pup->first; e; e = e->next) {
    e->y = y+2;
    e->x = 4;
    e->w = pup->win_width - 8;
    e->h = e->ph_height;
    if (e->is_separ == 0) {
      picohtml_render(e->ph, pup->pix, 4, y);
    }
    if (e->is_separ || (e == pup->first && ph_descr)) {
      int Y = y;
      if (e->is_separ == 0) Y = y-2; 
      XSetForeground(dock->display, dock->NormalGC, pup->lightpixel);
      XDrawLine(dock->display, pup->pix, dock->NormalGC, 
		1, Y+3, pup->win_width-2, Y+3);
      XSetForeground(dock->display, dock->NormalGC, pup->darkpixel);
      XDrawLine(dock->display, pup->pix, dock->NormalGC, 
		1, Y+4, pup->win_width-2, Y+4);
    }
    y += e->ph_height;
  }
  XSetWindowBackgroundPixmap(dock->display, pup->win, pup->pix);
  XMoveWindow(dock->display, pup->win, pup->win_xpos, pup->win_ypos);
  XMapRaised(dock->display, pup->win);
  if (ph_descr)
    picohtml_destroy(ph_descr);
}

static Plopup_entry *
get_entry(Dock *dock, int x, int y)
{
  Plopup *pup = dock->plopup;
  Plopup_entry *e;
  if (x < 1 || x > pup->win_width-1) return NULL;
  if (y < 1 || y > pup->win_height-1) return NULL;
  for (e = pup->first; e; e = e->next) {
    if (x >= e->x && x <= e->x + e->w &&
	y >= e->y && y <= e->y + e->h) {
      return e->is_separ ? NULL : e;
    }
  }
  return NULL;
}

static void
plopup_refresh(Dock *dock, int x, int y)
{
  Plopup *pup = dock->plopup;
  Plopup_entry *e;

  e = get_entry(dock, x, y);
  if (e != pup->active_entry) {
    pup->active_entry = e;
    XClearArea(dock->display, pup->win, 0, 0, pup->win_width, pup->win_height, False);
    if (pup->active_entry) {
      int x1,y1,x2,y2;
      x1 = 1; x2 = pup->win_width-2;
      y1 = pup->active_entry->y; 
      y2 = pup->active_entry->y + pup->active_entry->h;
      XSetForeground(dock->display, dock->NormalGC, pup->lightpixel);
      XDrawLine(dock->display, pup->win, dock->NormalGC, x1, y1, x2, y1);
      XDrawLine(dock->display, pup->win, dock->NormalGC, x1, y1, x1, y2);
      XSetForeground(dock->display, dock->NormalGC, pup->darkpixel);
      XDrawLine(dock->display, pup->win, dock->NormalGC, x1, y2, x2, y2);
      XDrawLine(dock->display, pup->win, dock->NormalGC, x2, y1, x2, y2);
    }
  }
}

int
plopup_ismapped(Dock *dock)
{
  return (dock->plopup->win != None);
}

void
plopup_unmap(Dock *dock)
{
  Plopup *pup = dock->plopup;
  Plopup_entry *e, *e_next;
  if (pup->win == None) {
    assert(pup->pix == None); return;
  }
  XFreePixmap(dock->display, pup->pix);
  XDestroyWindow(dock->display, pup->win);
  pup->active_entry = NULL;
  for (e = pup->first; e; e = e_next) {
    e_next = e->next;
    if (e->ph)
      picohtml_destroy(e->ph);
    free(e);
  }
  if (pup->descr) free(pup->descr);
  pup->descr = NULL;
  pup->first = NULL;
  pup->callback = NULL;
  pup->win = None;
  pup->pix = None;
}

void 
plopup_dispatch_event(Dock *dock, XEvent *event)
{
  Plopup *pup = dock->plopup;

  if (pup->win) {
    switch (event->type) {
    case MotionNotify: 
      {
	plopup_refresh(dock, event->xmotion.x_root-pup->win_xpos, event->xmotion.y_root-pup->win_ypos);
      } break;
      //    case ButtonPress:
    case ButtonRelease:
      {
	if (pup->callback) {
	  pup->callback(pup->active_entry ? pup->active_entry->id : -1);
	} else printf("buuuug plopup!!\n");
	plopup_unmap(dock);
      } break;
    }
  }
}

static int plopup_show_modal_id = -1;

void
plopup_show_modal_callback(int id) {
  plopup_show_modal_id = id;
}

int
plopup_show_modal(Dock *dock, int x, int y)
{
  XEvent event;
  plopup_show(dock,x,y, plopup_show_modal_callback);
  while (plopup_ismapped(dock)) {
    XNextEvent(dock->display, &event);
    plopup_dispatch_event(dock, &event);
  }
  return plopup_show_modal_id;
}
