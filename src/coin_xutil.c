/*
  coin_xutil : diverses fonctions complémentaires à raster.c pour la manip des images

  rcsid=$Id: coin_xutil.c,v 1.13 2005/09/26 21:40:24 pouaite Exp $

  ChangeLog:
  $Log: coin_xutil.c,v $
  Revision 1.13  2005/09/26 21:40:24  pouaite
  v 2.5.1b

  Revision 1.12  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.11  2003/08/26 21:50:48  pouaite
  2.6.4b au mastic

  Revision 1.10  2003/07/20 22:22:28  pouaite
  ce commit est dedie a Pierre Tramo

  Revision 1.9  2003/06/29 23:58:39  pouaite
  suppression de l'overrideredirect du palmi et ajout de pinnipede_totoz.c et wmcoincoin-totoz-get etc

  Revision 1.8  2002/10/05 18:08:14  pouaite
  ajout menu contextuel + fix de la coloration des boutons du wmccc

  Revision 1.7  2002/09/07 16:21:15  pouaite
  ça va releaser en douce

  Revision 1.6  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.5  2002/04/09 23:38:29  pouaite
  boitakon et son cortège de bugfixes

  Revision 1.4  2002/04/02 22:29:28  pouaite
  bugfixes transparence

  Revision 1.3  2002/04/01 23:04:11  pouaite
  fix compilation avec gcc 2.9x

  Revision 1.2  2002/04/01 22:56:03  pouaite
  la pseudo-transparence du pinni, bugfixes divers, option tribune.backend_type

  Revision 1.1  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

*/

#include <libintl.h>
#define _(String) gettext (String)

#include <X11/Xatom.h>
#include <X11/Xmd.h>
#include <string.h>
#include "coin_xutil.h"


RGBAImage *
rimage_create_from_raw(int w, int h, int bpp, const unsigned char *data)
{
  RGBAImage *rimg;
  int x,y;

  rimg = RGBACreateImage(w,h);
  for (y=0; y < h; y++) {
    for (x=0; x < w; x++) {
      int pos;
      pos = (y*w+x)*bpp;
      rimg->data[y][x].rgba[0] = data[pos];
      rimg->data[y][x].rgba[1] = data[pos+1];
      rimg->data[y][x].rgba[2] = data[pos+2];
      if (bpp == 4) {
	rimg->data[y][x].rgba[3] = data[pos+3];
      } else {
	rimg->data[y][x].rgba[3] = 0;
      }
    }
  }
  return rimg;
}

Pixmap
pixmap_create_from_raw(RGBAContext *rc, int w, int h, int bpp, const unsigned char *data)
{
  RGBAImage *rimg;
  Pixmap p;

  rimg = rimage_create_from_raw(w,h,bpp,data);
  p = RGBAImage2Pixmap(rc, rimg);
  RGBADestroyImage(rimg);
  return p;
}

RGBAImage *
rimage_create_from_raw_with_tint(int w, int h, int bpp, const unsigned char *data, int rgb)
{
  RGBAImage *rimg;
  int r,g,b;
  int i;

  r = (rgb >> 16) & 0xff; g = (rgb >> 8) & 0xff; b = rgb & 0xff;
  //  printf("rimage_create_from_raw_with_tint: rgb=%06x r=%d g=%d b=%d\n", rgb, r, g, b);
  rimg = RGBACreateImage(w,h);
  
  for (i=0; i < w*h; i++) {
    float v;
    v =  (data[i*bpp] + data[i*bpp+1] + data[i*bpp+2])/(3.*128);
    if (v < 1)
      v = 1. + (v-1.)/4; // les couleurs de la porte sont un peu trop contrastees a mon gout
    else 
      v = 1. + (v-1.)/12; // les couleurs de la porte sont un peu trop contrastees a mon gout
    rimg->data[0][i].rgba[0] = (unsigned char)MIN(v*r, 255);
    rimg->data[0][i].rgba[1] = (unsigned char)MIN(v*g, 255);
    rimg->data[0][i].rgba[2] = (unsigned char)MIN(v*b, 255);
    rimg->data[0][i].rgba[3] = 0;
  }

  /* c'est equivalent à */
  /*
  for (y=0; y < h; y++) {
    for (x=0; x < w; x++) {
      int pos;
      float v;
      pos = (y*w+x)*bpp;
      v =  (data[pos] + data[pos+1] + data[pos+2])/(3.*128);
      v = 1. + (v-1.)/2.5; // les couleurs de la porte sont un peu trop contrastees a mon gout
      rimg->data[y][x].rgba[0] = (unsigned char)MIN(v*r, 255);
      rimg->data[y][x].rgba[1] = (unsigned char)MIN(v*g, 255);
      rimg->data[y][x].rgba[2] = (unsigned char)MIN(v*b, 255);
      if (bpp == 4) {
	rimg->data[y][x].rgba[3] = data[pos+3];
      } else {
	rimg->data[y][x].rgba[3] = 0;
      }
    }
    }*/
  return rimg;
}

Pixmap
pixmap_create_from_raw_with_tint(RGBAContext *rc, int w, int h, int bpp, const unsigned char *data, int rgb)
{
  RGBAImage *rimg;
  Pixmap p;
  rimg = rimage_create_from_raw_with_tint(w,h,bpp,data,rgb);
  //rimg = rimage_create_from_raw(w,h,bpp,data);
  p = RGBAImage2Pixmap(rc, rimg);
  RGBADestroyImage(rimg);
  return p;
}

/* conversion du channel alpha d'une image RGBA (32bits) en un masque binaire
   utilise pour separer les deux morceaux de la porte

   TRES TRES largement inspire de createXBMfromXPM dans wmgeneral (sauf que moi
   je mets des commentaires ;-)
*/
void
alpha2mask(int w, int h, unsigned char *bitmap, const unsigned char *rgba, unsigned char mask_val)
{
  int i,j;
  unsigned char bits;
  unsigned int nbits;

  for (i=0; i < h; i++) {
    nbits = 0; bits = 0;
    for (j=0; j < w; j++) {
      bits >>= 1;
      if (rgba[i*w*4 + j*4 + 3] == mask_val) {
	bits |= 0x80;
      }
      //printf("%1d", rgba[i*w*4 + j*4 + 3] == mask_val);
      nbits++;
      if (nbits == 8) {
	*bitmap = bits;
	bitmap++; nbits = 0; bits = 0;
      }
    }
    //    printf("\n");
  }
}

Pixmap
alpha2pixmap_mask(Display *dpy, Window win, int w, int h, const unsigned char *rgba, unsigned char mask_val)
{
  unsigned char *bitmap;
  Pixmap p;

  ALLOC_VEC(bitmap, h*((w+7)/8), unsigned char);
  alpha2mask(w, h, bitmap, rgba, mask_val);
  p = XCreateBitmapFromData(dpy, win, bitmap, w, h);
  FREE_VEC(bitmap);
  return p;
}

/* sert à colorier le pixmap des lettres -> le noir devient la color 'tint' 

a virer: c'est nul, ca alloue de la memoire et c'est trop lourd de changer la couleur du texte defilant dans le dock
*/
void
colorize_black_pixmap(char **pixtxt, int tint)
{
  int w,h,ncolor;
  int i;

  assert(sscanf(pixtxt[0], "%d %d %d", &w, &h, &ncolor) == 3);
  for (i=1; i <= ncolor; i++) {
    int l,r,g,b;
    if (strstr(pixtxt[i], "None") == NULL) {
      char *s;
      s = strdup(pixtxt[i]);

      l = strlen(s); assert(l > 8);
      assert(sscanf(s+l-6, "%2x%2x%2x", &r, &g, &b));
      s[l-6] = '0';
      r = ((0xff-r)*((tint >> 16) & 0xff)) / 0xff;
      g = ((0xff-g)*((tint >>  8) & 0xff)) / 0xff;
      b = ((0xff-b)*((tint      ) & 0xff)) / 0xff;
      snprintf(s+l-6, 7, "%02x%02x%02x", r, g, b);
      
      pixtxt[i] = s;
    }
  }
}

 
void 
get_window_pos_with_decor(Display *display, Window base_win, int *screen_x, int *screen_y)
{
  Window child_win;
  Window parent_win;
  Window root_win;
  Window* child_windows;
  int num_child_windows;
  XWindowAttributes win_attr;
  Window win;
  
  *screen_x = -1; *screen_y = -1;
  win = base_win;
  
  while (XQueryTree(display, win,
		    &root_win,
		    &parent_win,
		    &child_windows, &num_child_windows)) {
    if (child_windows) XFree(child_windows);
    if (parent_win == root_win) break;
    win = parent_win;
  }
  
  //  printf("win = %lx, parent=%lx, root=%lx\n", win, parent_win, root_win);
  if (parent_win) {
    XGetWindowAttributes(display, win, &win_attr);
    XTranslateCoordinates(display, win, root_win,
			  -win_attr.border_width, -win_attr.border_width, screen_x, screen_y,
			  &child_win);
  }
}

void 
get_window_pos_without_decor(Display *display, Window root_win, Window win, int *screen_x, int *screen_y)
{
  Window child_win = None;
  XWindowAttributes win_attr;

  XGetWindowAttributes(display, win, &win_attr);
  XTranslateCoordinates(display, win, root_win,
			win_attr.x, win_attr.y, screen_x, screen_y,
			&child_win);
}

void get_window_dimensions(Display *display, Window win, int *pw, int *ph) {
  Window root;
  int x,y;
  unsigned depth, border;
  XGetGeometry(display, win, &root, &x, &y, pw, ph, 
               &border, &depth);
}

/* volée dans les sources de aterm :) 
   maintenant, faut voir dans quelles conditions cette fonction marche 
   (avec wmaker/wmsetbg ça marche en tout cas)

   race condition possible si le rootpixmap est changé entre le moment où cette fonction
   est appelée et le moment où on utilise le pixmap...
*/
Pixmap
get_rootwin_pixmap(const RGBAContext *rc)
{
  Atom id;
  Pixmap currentRootPixmap = None;
  id = XInternAtom (rc->dpy, "_XROOTPMAP_ID", True);

  if (id != None) {
    Atom act_type;
    int act_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop = NULL;
    
    /*fprintf(stderr, "\n aterm GetRootPixmap(): root pixmap is set");    		    */
    if (XGetWindowProperty (rc->dpy, rc->drawable, id, 0, 1, False, XA_PIXMAP,
			    &act_type, &act_format, &nitems, &bytes_after,
			    &prop) == Success)
      {
	if (prop)
	  {
	    currentRootPixmap = *((Pixmap *) prop);
	    XFree (prop);
	  }
      }
  }
  return currentRootPixmap ;
}

/* une de mes macros les plus laides :) */
#define SELECT_SHADE_OR_TINT(_c1,_c2) { if (ti->type == SHADING) { _c1; \
          r = (r*shade_coef)/256 + shade_inc; \
          g = (g*shade_coef)/256 + shade_inc; \
          b = (b*shade_coef)/256 + shade_inc; \
          _c2; } else { int l; _c1; \
          l = (r*76)/256 + (g*150)/256 + (b*30)/256; \
          r = (l*wr + (256-l)*br)/256; \
	  g = (l*wg + (256-l)*bg)/256; \
	  b = (l*wb + (256-l)*bb)/256; _c2; } }

//#define TRANSFO(x,m,d) ((d) > 0 ? (((x)&(m))>>(unsigned)(d)) : (((x)&(m))<<(unsigned)(-(d))))

void
shade_XImage(const RGBAContext *rc, XImage *ximg, TransparencyInfo *ti)
{
  int wr=0,wg=0,wb=0,br=0,bg=0,bb=0; 
  int x, y;
  int shade_coef=0, shade_inc=0;

  if (ti->type == TINTING) {
    wr = (ti->tint.white & 0xff0000)>>16; 
    wg = (ti->tint.white & 0x00ff00)>>8; 
    wb = (ti->tint.white & 0x0000ff);
    br = (ti->tint.black & 0xff0000)>>16; 
    bg = (ti->tint.black & 0x00ff00)>>8; 
    bb = (ti->tint.black & 0x0000ff);
  } else if (ti->type == SHADING) {
  //  int l_tint = (tr*76)/256 + (tg*150)/256 + (tb*30)/256;
    shade_inc = ((ti->shade.luminosite)*256)/100;
    shade_coef = ((ti->shade.assombrissement-ti->shade.luminosite)*256)/100;
  }
  if (ximg->bits_per_pixel == 16 || ximg->bits_per_pixel == 15) {
    CARD16 *p = (CARD16*)ximg->data;
    SELECT_SHADE_OR_TINT(
			 for (y=0; y < ximg->height; y++) {
			   for (x=0; x < ximg->width; x++) {
			     int r = PIXEL2R(rc,p[x]);
			     int g = PIXEL2G(rc,p[x]);
			     int b = PIXEL2B(rc,p[x]);
				,;
				p[x] = rc->rtable[r] + rc->gtable[g] + rc->btable[b];
			   }
			   p += ximg->bytes_per_line/2;
			 }
			 );
  } else if (ximg->bits_per_pixel == 24) {
    unsigned char *p = (unsigned char*) ximg->data;
    SELECT_SHADE_OR_TINT(
			 for (y=0; y < ximg->height; y++) {
			   for (x=0; x < ximg->width; x++) {
			     int r = p[x*3];
			     int g = p[x*3+1];
			     int b = p[x*3+2];
			     ,;
			     p[x*3] = r; p[x*3+1] = g; p[x*3+2] = b;
			   }
			   p += ximg->bytes_per_line;
			 }
			 );
    
  } else if (ximg->bits_per_pixel == 32) {
    CARD32 *p = (CARD32*) ximg->data;
    SELECT_SHADE_OR_TINT(
			 for (y=0; y < ximg->height; y++) {
			   for (x=0; x < ximg->width; x++) {
			     int r = (p[x] & 0xff0000)>>16;
			     int g = (p[x] & 0x00ff00)>>8;
			     int b = (p[x] & 0x0000ff);;
			     ,;
			     p[x] = (p[x] & 0xff000000) + (r << 16) + (g << 8) + b;
			   }
			   p += ximg->bytes_per_line/4;
			 }
			 );
  } else {
    printf(_("shade_ximage unsupported (bitsperpix=%d) !! Strange...\n"),ximg->bits_per_pixel);
  }
}

static int x11_error_occured = 0;

int x_error_handler_bidon(Display *dpy, XErrorEvent *err)
{
  char errmsg[80]; 
  XGetErrorText(dpy, err->error_code, errmsg, 80);
  fprintf(stderr, _("X11 error detected in a hazardous area:\n  %s\n"), errmsg);
  fprintf(stderr, _("  Protocol request: %d\n"), err->request_code);
  fprintf(stderr, _("  Resource ID:      0x%lx\n"), err->resourceid);
  fprintf(stderr, _(" --> THE SHOW MUST GO ON!\n"));
  x11_error_occured = 1;
  return 0;
}

/* obligé de gérer les erreurs de manière un peu cavalière, car le root pixmap peut être détruit
   à tout bout de champ etc.. */
int
extract_root_pixmap_and_shade(const RGBAContext *rc, int x, int y, int w, int h, 
			      TransparencyInfo *ti, int use_fake_real_transparency, Pixmap shade_pix, int spdx, int spdy)
{
  Pixmap root_pix;
  XImage *ximg;
  int rw,rh,rx,ry,dx,dy,sw,sh;
  

  if (rc->depth < 15) return None; /* pas de pseudotransp sur les visual non truecolor */

  root_pix = get_rootwin_pixmap(rc);
  if (root_pix == None) return None;


  dx = 0; dy = 0;
  rw = w; rh = h; rx = x; ry = y;
  sw = WidthOfScreen(XScreenOfDisplay(rc->dpy, rc->screen_number));
  sh = HeightOfScreen(XScreenOfDisplay(rc->dpy, rc->screen_number));
  if (rx < 0) {
    rw += rx; dx -= rx; rx = 0;
  }
  if (ry < 0) {
    rh += ry; dy -= ry; ry = 0;
  }
  if (rx+rw > sw) {
    rw = sw-rx;
  }
  if (ry+rh > sh) {
    rh = sh-ry;
  }
  if (rw < 0 || rh < 0) return None;

  dx += spdx; dy += spdy;

  x11_error_occured = 0;

  /* transparence pure, ça va vite */
  if (ti->type == FULL_TRANSPARENCY) {
    assert(shade_pix != None);
    int (*old_handler)(Display *, XErrorEvent *);
    old_handler = XSetErrorHandler(x_error_handler_bidon);

    if (use_fake_real_transparency) {
      ximg = XGetImage(rc->dpy, RootWindow(rc->dpy, rc->screen_number), rx, ry, rw, rh, 
		       AllPlanes, ZPixmap); assert(ximg);
      XPutImage(rc->dpy, shade_pix, rc->copy_gc, ximg, 0, 0, dx, dy, rw, rh);
      XDestroyImage(ximg);
    } else {
      XCopyArea(rc->dpy, root_pix, shade_pix, rc->copy_gc, 
		rx, ry, rw, rh, dx, dy);
    }
    XSync(rc->dpy,0);
    XSetErrorHandler(old_handler); 
    if (x11_error_occured) { return -1; }
    else return 0;
  }

  /* shade/tinte, il faut retravailler l'image */

  x11_error_occured = 0; XSetErrorHandler(x_error_handler_bidon);
  if (use_fake_real_transparency) {
    ximg = XGetImage(rc->dpy, RootWindow(rc->dpy, rc->screen_number), rx, ry, rw, rh, 
		     AllPlanes, ZPixmap);
  } else {
    ximg = XGetImage(rc->dpy, root_pix, rx, ry, rw, rh, 
		     AllPlanes, ZPixmap); 
  }
  XSetErrorHandler(NULL); if (x11_error_occured || ximg == NULL) return -1;

  shade_XImage(rc, ximg, ti);
  assert(shade_pix != None);
  XPutImage(rc->dpy, shade_pix, rc->copy_gc, ximg, 0, 0, dx, dy, rw, rh);
  XDestroyImage(ximg);
  return 0;
}

int
lighten_color(int icol, float v)
{
  int r = (icol & 0xff0000)>>16;
  int g = (icol & 0xff00)>>8;
  int b = (icol & 0xff);
  r = (int)((r+.5)*v);
  g = (int)((g+.5)*v);
  b = (int)((b+.5)*v);
  r = MIN(r,255);
  g = MIN(g,255);
  b = MIN(b,255);
  return (r<<16) + (g<<8) + b;
}

/* motif window manager styles 
 */ 
#define MwmHintsDecorations (1L << 1)
#define MwmDecorAll         (1L << 0)
#define MwmDecorBorder      (1L << 1)
#define MwmDecorHandle      (1L << 2)
#define MwmDecorTitle       (1L << 3)
#define MwmDecorMenu        (1L << 4)
#define MwmDecorMinimize    (1L << 5)
#define MwmDecorMaximize    (1L << 6)
 
#define PropMotifWmHintsElements 3
 
typedef struct {
  int flags;
  int functions;
  int decorations;
  int inputMode;
  int unknown;
} MwmHints;
 
/* gnome hints */
#define WIN_HINTS_SKIP_WINLIST		(1<<1) /* not in win list */
#define WIN_HINTS_SKIP_TASKBAR		(1<<2) /* not on taskbar */

/* aaaah */
int
set_borderless_window_hints(Display *display, Window win) {
  enum { MOTIF_WM_HINTS=0, KWM_WIN_DECORATION, WIN_HINTS, 
         NET_WM_STATE, NET_WM_STATE_SKIP_TASKBAR, NET_WM_WINDOW_TYPE, NET_WM_WINDOW_TYPE_DOCK, NET_WM_STATE_STAYS_ON_TOP, NB_ATOMS };

  Atom atom[NB_ATOMS];
  char *atom_names[NB_ATOMS] = { "_MOTIF_WM_HINTS", 
                                 "KWM_WIN_DECORATION", 
                                 "_WIN_HINTS", 
                                 "_NET_WM_STATE",
                                 "_NET_WM_STATE_SKIP_TASKBAR", 
                                 "_NET_WM_WINDOW_TYPE",
                                 "_NET_WM_WINDOW_TYPE_DOCK",
                                 "_NET_WM_STATE_STAYS_ON_TOP" };
  int ok = 0;
  XInternAtoms(display, atom_names, NB_ATOMS, True, atom);
  /*  dock->atom_NET_WM_WINDOW_TYPE = XInternAtom(dock->display, "_NET_WM_WINDOW_TYPE", True);
  dock->atom_NET_WM_WINDOW_TYPE_DESKTOP = XInternAtom(dock->display, "_NET_WM_WINDOW_TYPE_DESKTOP", True);
  dock->atom_NET_WM_WINDOW_TYPE_DOCK = XInternAtom(dock->display, "_NET_WM_WINDOW_TYPE_DOCK", True);
  dock->atom_NET_WM_WINDOW_TYPE_TOOLBAR = XInternAtom(dock->display, "_NET_WM_WINDOW_TYPE_TOOLBAR", True);
  dock->atom_NET_WM_WINDOW_TYPE_MENU = XInternAtom(dock->display, "_NET_WM_WINDOW_TYPE_MENU", True);
  dock->atom_NET_WM_WINDOW_TYPE_DIALOG = XInternAtom(dock->display, "_NET_WM_WINDOW_TYPE_DIALOG", True);
  dock->atom_NET_WM_WINDOW_TYPE_NORMAL = XInternAtom(dock->display, "_NET_WM_WINDOW_TYPE_NORMAL", True);
  dock->atom_NET_WM_WINDOW_TYPE_SPLASH = XInternAtom(dock->display, "_NET_WM_WINDOW_TYPE_SPLASH", True);
  dock->atom_NET_WM_WINDOW_TYPE_UTILITY = XInternAtom(dock->display, "_NET_WM_WINDOW_TYPE_UTILITY", True);
  dock->atom_NET_WM_STATE = XInternAtom(dock->display, "_NET_WM_STATE", True);
  dock->atom_NET_WM_STATE_MODAL = XInternAtom(dock->display, "_NET_WM_STATE_MODAL", True);
  dock->atom_NET_WM_STATE_STICKY = XInternAtom(dock->display, "_NET_WM_STATE_STICKY", True);
  dock->atom_NET_WM_STATE_MAXIMIZED_VERT = XInternAtom(dock->display, "_NET_WM_STATE_MAXIMIZED_VERT", True);
  dock->atom_NET_WM_STATE_SHADED = XInternAtom(dock->display, "_NET_WM_STATE_SHADED", True);
  dock->atom_NET_WM_STATE_SKIP_PAGER = XInternAtom(dock->display, "_NET_WM_STATE_SKIP_PAGER", True);
  dock->atom_NET_WM_STATE_HIDDEN = XInternAtom(dock->display, "_NET_WM_STATE_HIDDEN", True);
  dock->atom_NET_WM_STATE_FULLSCREEN = XInternAtom(dock->display, "_NET_WM_STATE_FULLSCREEN", True);
  dock->atom_NET_WM_DESKTOP = XInternAtom(dock->display, "_NET_WM_DESKTOP", True);
  */
  if (atom[MOTIF_WM_HINTS] != None) {
    MwmHints hints;
    hints.flags = MwmHintsDecorations;
    hints.decorations = 0;
    XChangeProperty(display, win,
                    atom[MOTIF_WM_HINTS], atom[MOTIF_WM_HINTS], 32,
                    PropModeReplace, (unsigned char *) &hints, sizeof(MwmHints)/4);
    ok = 1;
  }
  if (atom[KWM_WIN_DECORATION] != None) {
    long KWMHints = 0;
    XChangeProperty(display, win, atom[KWM_WIN_DECORATION], XA_CARDINAL, 32,
                    PropModeReplace, (unsigned char *)&KWMHints,
                    sizeof(KWMHints)/4);
    ok = 1;
  }
  if (atom[WIN_HINTS] != None) {
    long GNOMEHints = WIN_HINTS_SKIP_WINLIST | WIN_HINTS_SKIP_TASKBAR;
    XChangeProperty(display, win, atom[WIN_HINTS], XA_CARDINAL, 32,
                    PropModeReplace, (unsigned char *)&GNOMEHints,
                    sizeof(GNOMEHints)/4);
    ok = 1;
  }
    if (atom[NET_WM_STATE] != None) {
    Atom p[2]; 
    p[0] = atom[NET_WM_STATE_SKIP_TASKBAR]; 
    p[1] = atom[NET_WM_STATE_STAYS_ON_TOP];
    XChangeProperty(display, win, atom[NET_WM_STATE], XA_ATOM, 32,
                    PropModeReplace, (unsigned char *)p, 2);
  }
  if (atom[NET_WM_WINDOW_TYPE] != None) {
    XChangeProperty(display, win, atom[NET_WM_WINDOW_TYPE], XA_ATOM, 32,
                    PropModeReplace, (unsigned char*)&atom[NET_WM_WINDOW_TYPE_DOCK], 1);
  }
  return ok; /* pour savoir si il faut la passer en overrideredirect */
}

void
set_window_title(Display *display, Window win, char *window_title, char *icon_title) {
  int rc;
  XTextProperty window_title_property;
  /* nom de la fenetre */
  rc = XStringListToTextProperty(&window_title,1, &window_title_property); assert(rc);
  XSetWMName(display, win, &window_title_property);
  XFree(window_title_property.value);
  
  /* nom de la fenetre iconifiée */
  rc = XStringListToTextProperty(&icon_title,1, &window_title_property); assert(rc);
  XSetWMIconName(display, win, &window_title_property);
  XFree(window_title_property.value);
}

/*
void
set_window_pos_hints(Display *display, Window win, int x, int y) {
  long user_hints;
  XSizeHints* win_size_hints;
  win_size_hints= XAllocSizeHints(); assert(win_size_hints);
  if (!XGetWMNormalHints(display, win, win_size_hints, &user_hints))
    win_size_hints->flags = 0;
  win_size_hints->x = x; 
  win_size_hints->y = y;
  win_size_hints->flags |= USPosition;
  XSetWMNormalHints(display, win, win_size_hints);
  XFree(win_size_hints);
}
*/

/* -1 pour les quantité non utilisées */
void
set_window_sizepos_hints(Display *display, Window win,
                         int x, int y,
                         int minw, int basew, int maxw,
                         int minh, int baseh, int maxh) {
  //  long user_hints;
  XSizeHints* win_size_hints;
  win_size_hints= XAllocSizeHints(); assert(win_size_hints);
  /*  if (!XGetWMNormalHints(display, win, win_size_hints, &user_hints))
      win_size_hints->flags = 0;*/
  win_size_hints->x = x; 
  win_size_hints->y = y;
  win_size_hints->min_width = minw;
  win_size_hints->min_height = minh;
  win_size_hints->base_width = basew;
  win_size_hints->base_height = baseh;
  win_size_hints->max_width = maxw;
  win_size_hints->max_height = maxh;
  win_size_hints->flags = 0;
  if (x != -10000 && y != -10000) win_size_hints->flags |= USPosition;
  if (minw >= 0 && minh >= 0) win_size_hints->flags |= PMinSize;
  if (basew >= 0 && baseh >= 0) win_size_hints->flags |= PSize;
  if (maxw >= 0 && maxh >= 0) win_size_hints->flags |= PMaxSize;
  XSetWMNormalHints(display, win, win_size_hints);
  XFree(win_size_hints);
}

void
set_window_class_hint(Display *display, Window win, char *res_class, char *res_name) {
  XClassHint *class_hint;
  class_hint = XAllocClassHint();
  class_hint->res_name = res_name;
  class_hint->res_class = res_class;
  XSetClassHint(display, win, class_hint);
  XFree(class_hint);
}

void
show_gc(Display *display, GC gc) {
  XGCValues v;
  XGetGCValues(display, gc, (unsigned long)(-1L), &v);
  printf("GCValues:\n");
  printf(" int function             = %d\n", v.function);
  printf(" unsigned long plane_mask = %08lx\n",v.plane_mask);
  printf(" unsigned long foreground = %08lx\n",v.foreground);
  printf(" unsigned long background = %08lx\n",v.background);
  printf(" int line_width           = %d\n",v.line_width);
  printf(" int line_style           = %d\n",v.line_style);
  printf(" int cap_style            = %d\n",v.cap_style);
  printf(" int join_style           = %d\n",v.join_style);
  printf(" int fill_style           = %d\n",v.fill_style);
  printf(" int fill_rule            = %d\n",v.fill_rule);
  printf(" int arc_mode             = %d\n",v.arc_mode);
  printf(" Pixmap tile              = %08lx\n",v.tile);
  printf(" Pixmap stipple           = %08lx\n",v.stipple);
  printf(" int ts_x_origin          = %d\n",v.ts_x_origin);
  printf(" int ts_y_origin          = %d\n",v.ts_y_origin);
  printf(" Font font                = %08lx\n",v.font);
  printf(" int subwindow_mode       = %d\n",v.subwindow_mode);
  printf(" Bool graphics_exposures  = %d\n",v.graphics_exposures);
  printf(" int clip_x_origin        = %d\n",v.clip_x_origin);
  printf(" int clip_y_origin        = %d\n",v.clip_y_origin);
  printf(" Pixmap clip_mask         = %08lx\n",v.clip_mask);
  printf(" int dash_offset          = %d\n",v.dash_offset);
  printf(" char dashes              = %c\n",v.dashes);
}
