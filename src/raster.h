/*
  rcsid=$Id: raster.h,v 1.10 2003/01/11 14:10:07 pouaite Exp $
  ChangeLog:
  $Log: raster.h,v $
  Revision 1.10  2003/01/11 14:10:07  pouaite
  fix du palmi pour xf 4.3

  Revision 1.9  2002/08/31 21:26:46  pouaite
  ajout du wmccc

  Revision 1.8  2002/06/23 22:26:01  pouaite
  bugfixes+support à deux francs des visuals pseudocolor

  Revision 1.7  2002/04/10 22:53:44  pouaite
  un commit et au lit

  Revision 1.6  2002/04/02 22:29:29  pouaite
  bugfixes transparence

  Revision 1.5  2002/03/21 22:53:07  pouaite
  ajout d'une icone pour la fenetre du pinnipede et des news

  Revision 1.4  2002/03/18 22:46:49  pouaite
  1 ou 2 bugfix mineurs, et surtout suppression de la dependance avec la libXpm .. un premier pas vers wmc² en 8bits

  Revision 1.3  2002/03/10 16:07:10  pouaite
  pseudo transp basique dans le pinnipede (en cours..)

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

*/

#ifndef RASTER_H__
#define RASTER_H__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef XINERAMA
# include <X11/extensions/Xinerama.h>
#endif
#define PSEUDOCOL_NCOLORS 6

#define _RGB2PIXEL(c,r,g,b) ((c->truecolor) ? c->rtable[r] + \
                             c->gtable[g] + \
                             c->btable[b] : c->pseudocol_palette[(r*PSEUDOCOL_NCOLORS+128)/255][(g*PSEUDOCOL_NCOLORS+128)/255][(b*PSEUDOCOL_NCOLORS+128)/255])
#define _IRGB2PIXEL(c,rgb) ((c->truecolor) ? c->rtable[((rgb)>>16) & 0xff] + \
                            c->gtable[((rgb)>> 8) & 0xff] + \
                            c->btable[((rgb)    ) & 0xff] : \
                            c->pseudocol_palette[((((rgb)>>16) & 0xff)*PSEUDOCOL_NCOLORS+128)/255][((((rgb)>> 8) & 0xff)*PSEUDOCOL_NCOLORS+128)/255][(((rgb) & 0xff)*PSEUDOCOL_NCOLORS+128)/255])


typedef struct RGBAContext {
  Display *dpy;
  int screen_number;
  Colormap cmap;
  
  GC copy_gc;
  
  Visual *visual;
  int depth;
  Window drawable;		       /* window to pass for XCreatePixmap().*/
				       /* generally = root */
  int vclass;
  
  unsigned long black;
  unsigned long white;

  unsigned long rtable[256], gtable[256], btable[256];

  /* decalages à appliquer sur un pixel (apres l'avoir masque avec visual->red_mask etc) pour se retrouver avec une valeur entre 0 et 255 
     ex: composante rouge: (pixel >> r_shift_left) << r_shift_right;
  */
  unsigned char r_shift_left , g_shift_left,  b_shift_left;
  unsigned char r_shift_right, g_shift_right, b_shift_right;
  int rmask, gmask, bmask;

  int truecolor;
  unsigned long pseudocol_palette[PSEUDOCOL_NCOLORS+1][PSEUDOCOL_NCOLORS+1][PSEUDOCOL_NCOLORS+1]; /* contient les pixel pour une palette en pseudocolor */
} RGBAContext;


typedef union RGBAPixel_ {
  unsigned char rgba[4];
  unsigned int v;
} RGBAPixel;

typedef struct {
  int w, h;
  RGBAPixel **data;
} RGBAImage;

RGBAContext* RGBACreateContext(Display *dpy, int screen_number);
RGBAImage* RGBACreateImage(int width, int height);
void RGBADestroyImage(RGBAImage *img);
XImage *RGBAImage2XImage(RGBAContext *ctx, RGBAImage *rimg);
Pixmap RGBAImage2Pixmap(RGBAContext *ctx, RGBAImage *rimg);
RGBAImage *RGBACreateRImgFromXpmData(RGBAContext *ctx, char **xpm);
Pixmap RGBACreatePixmapFromXpmData(RGBAContext *ctx, char **xpm);
Pixmap RGBACreatePixmapFromXpmFile(RGBAContext *ctx, char *xpm_file, int *w, int *h);
#endif
