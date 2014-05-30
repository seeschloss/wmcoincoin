
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "Imlib2.h"
#include "config.h"
#include "gif.h"
Display *disp = 0;
Window   win = None;
Pixmap   pm = 0;
Visual  *vis = 0;
Colormap cm;
int      depth;


#define MAX(a,b) ((a)<(b) ? (b) : (a))
#define MIN(a,b) ((a)>(b) ? (b) : (a))

struct _Prefs {
  int timeout_exit;
  char *totoz_path;
  float zoom;
  unsigned bgcolor;
} prefs;

typedef struct _Anim {
  int nimg;
  Imlib_Image **imgs;
  int *delay;
  int loopcnt;
  int width, height;
  DATA32 *data;
} Anim;


Anim anim;

static void init_prefs() {
  char s[1024];
  prefs.timeout_exit = 30;
  snprintf(s, 1024, "%s/.wmcoincoin/totoz", getenv("HOME"));
  prefs.totoz_path = strdup(s);
  prefs.zoom = 1.0;
  prefs.bgcolor = 0x00ffffff;
}

static void init_anim() {
  anim.nimg = 0;
  anim.imgs = NULL;
  anim.delay = 0;
  anim.loopcnt = -1;
  anim.width = anim.height = 0;
  anim.data = 0;
}

void
main_loop() {
  int w = MAX((int)(anim.width * prefs.zoom + .5),1);
  int h = MAX((int)(anim.height * prefs.zoom + .5),1);


  XResizeWindow(disp, win, w, h);
  XMapWindow(disp, win);
  XSync(disp, False);
  
  for(;;) {
    int img;
    for (img = 0; img < anim.nimg; ++img) {
      imlib_context_set_image(anim.imgs[img]);
      imlib_context_set_anti_alias(1);
      /*      imlib_render_image_part_on_drawable_at_size(0, 0, anim.width, anim.height,
                                                  0, 0, anim.width, anim.height);
      */
      imlib_render_image_part_on_drawable_at_size(0, 0, anim.width, anim.height,
                                                  0, 0, w, h);
      usleep(MAX(anim.delay[img],1) * 8000);
    }
  }
}

int 
main (int argc, char **argv)
{
   char        *fname = NULL;
   Gif_Stream  *gfs;
   FILE *f;
   int query_mode = 0;
   int stack_mode = 0;

   if (argc < 2)
      return 1;

   init_prefs();
   init_anim();

   disp  = XOpenDisplay(NULL);
   if (argc == 1) {
     printf("kikou je suis un player pourri\n");
     printf("mes options toutes plus nazes les unes que les autres sont:\n");
     printf(" wmcoincoin_player uneimage        pour voir une image\n");
     printf(" wmcoincoin_player -z unchiffre uneimage  pour voir une image avec un certain facteur de zoom\n");
     printf(" wmcoincoin_player -i uneimage     pour connaitres les dimensions d'une image\n");
     printf(" wmcoincoin_player -s uneimage     le stack-mode (pour debeuggai)\n");
     printf(" le dernier argument, optionel, est l'id de la fenetre a utiliser\n");
     exit(0);
   }
   if (argc >= 3 && strcmp(argv[1], "-i") == 0) {
     query_mode = 1;
     argc--; argv++;
   }
   if (argc >= 3 && strcmp(argv[1], "-s") == 0) {
     stack_mode = 1;
     argc--; argv++;
   }
   if (argc >= 3 && strcmp(argv[1], "-z") == 0) {
     prefs.zoom = atof(argv[2]);
     argc-=2; argv += 2;
   }
   if (argc == 2) {
     fname  = argv[1];
     if (!query_mode) {
       vis   = DefaultVisual(disp, DefaultScreen(disp));
       depth = DefaultDepth(disp, DefaultScreen(disp));    
       cm    = DefaultColormap(disp, DefaultScreen(disp));
       win   = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 10, 10, 
                                   0, 0, 0);
       XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask | 
                    ButtonMotionMask | PointerMotionMask);
     }
   } else {
     XWindowAttributes attr;
     unsigned long lwin;
     sscanf(argv[2],"0x%lx",&lwin); win = lwin;
     //printf("win=%08lx\n", lwin);
     fname = argv[1];
     XGetWindowAttributes(disp, win, &attr);
     vis = attr.visual;
     depth = attr.depth;
     cm = attr.colormap;
   }
   if (!query_mode) {
     imlib_context_set_display(disp);
     imlib_context_set_visual(vis);
     imlib_context_set_colormap(cm);
     /*imlib_context_set_progress_function(progress);*/
     imlib_context_set_progress_granularity(10);
     imlib_context_set_drawable(win);
   }
   
   f = fopen(fname, "rb");
   if (!f) {
     fprintf(stderr, "%s: %s", fname, strerror(errno));
     return 1;
   }
   gfs = Gif_FullReadFile(f, GIF_READ_COMPRESSED, 0, 0);
   if (!gfs || Gif_ImageCount(gfs) == 0) {
     anim.nimg = 1;
     anim.imgs = Gif_NewArray(Imlib_Image*, anim.nimg);
     anim.imgs[0] = imlib_load_image(fname);
     if (anim.imgs[0] == NULL) {
       fprintf(stderr,"'%s' is not a valid image according to imlib2\n",fname);
       return 1;
     }
     anim.delay = Gif_NewArray(int, anim.nimg);
     anim.delay[0] = 1000000;
     anim.loopcnt = -1;
     imlib_context_set_image(anim.imgs[0]);
     anim.width  = imlib_image_get_width();
     anim.height = imlib_image_get_height();
   } else {
     int last,disposal;
     int i, x, y, lastx,lasty,lastw,lasth;
     anim.width = anim.height = 0;
     if (stack_mode) {
       fprintf(stderr, "%s: background=%02x nbcol=%d sw=%d sh=%d\n", fname, gfs->background, gfs->global->ncol, gfs->screen_width, gfs->screen_height);
       fprintf(stderr, "loopcnt=%ld nimages=%d\n", gfs->loopcount, gfs->nimages);
     }
     for (i=0; i < gfs->nimages; ++i) {
       Gif_Image *gfi = gfs->images[i];
       if (stack_mode) 
         fprintf(stderr, "%4d transparent=%04x delay=%d disposal=%d %dx%d+%d+%d ncol=%d\n", i, gfi->transparent, gfi->delay, gfi->disposal, 
                 gfi->width, gfi->height, gfi->left, gfi->top,gfi->local ? gfi->local->ncol : -1);
       anim.width = MAX(anim.width, gfi->width + gfi->left);
       anim.height = MAX(anim.height, gfi->height + gfi->top);
     }

     anim.data = Gif_NewArray(DATA32, anim.width*anim.height*gfs->nimages);
     anim.nimg = stack_mode ? 1 : gfs->nimages;
     anim.imgs = Gif_NewArray(Imlib_Image*, anim.nimg);
     anim.delay = Gif_NewArray(int, anim.nimg);

     /*
       The thing to remember about Restore to Previous is that it's not
       necessarily the first frame of the animation that will be restored but
       the last frame set to Unspecified or Do Not Dispose. And the most
       important fact is that Netscape doesn't correctly support Restore to
       Previous. It treats Restore to Previous as Do Not Dispose, so the the
       last frame -- not the last undisposed frame -- shows through empty or
       transparent areas. Microsoft Internet Explorer 3.0 does handle Restore
       to Previous correctly.
     */

     last = -1;
     lastx = 0; lasty = 0; lastw = anim.width; lasth = anim.height;
     disposal = GIF_DISPOSAL_NONE; //gfs->images[gfs->nimages-1]->disposal;
     for (i=0; i < gfs->nimages; ++i) {
       DATA32 *data = anim.data + anim.width*anim.height*i;
       Gif_Image *gfi = gfs->images[i];
       int k;
       //disposal = gfi->disposal;
       if (!gfi->img && !gfi->image_data && gfi->compressed) {
         Gif_UncompressImage(gfi);
       }
       if (i) memcpy(data, data - anim.width*anim.height , anim.width*anim.height*sizeof(int));
       if ((disposal == GIF_DISPOSAL_NONE || 
            disposal == GIF_DISPOSAL_BACKGROUND) || last == -1 || stack_mode) {
         for (y=0; y < lasth; ++y) {
           for (x=0; x < lastw; ++x) {
             unsigned pos = (y+lasty)*anim.width + x + lastx;
             data[pos] = prefs.bgcolor;
           }
         }
       } else {
         int l = last;
         if (i && disposal != GIF_DISPOSAL_ASIS) l = i-1;
         if (disposal == GIF_DISPOSAL_ASIS)
           for (k=0; k < anim.width*anim.height; ++k) data[k] = anim.data[l*anim.width*anim.height + k];
         else
           for (y=0; y < lasth; ++y) {
             for (x=0; x < lastw; ++x) {
               unsigned pos = (y+lasty)*anim.width + x + lastx;
               data[pos] = data[pos - l*anim.width*anim.height];
             }
           }
         
       }

       for (y=0; y < gfi->height; ++y) {
         for (x=0; x < gfi->width; ++x) {
           Gif_Color *c;
           DATA32 p;
           unsigned pos;
           if (gfi->local) {
             c = &gfi->local->col[gfi->img[y][x]];
           } else {
             c = &gfs->global->col[gfi->img[y][x]];
           }           
           p = (0 << 24) + (c->red << 16) + (c->green << 8) + c->blue;
           pos = (y+gfi->top)*anim.width + x + gfi->left;
           if (gfi->img[y][x] != gfi->transparent) {
             //if (gfi->img[y][x] == gfi->transparent || gfi->img[y][x] == gfs->background) p = prefs.bgcolor;
             data[pos] = p;
             /*           } else if (i) {
                          data[pos] = anim.data[(i-1)*anim.width*anim.height+pos]; // pour grand%2Dpas.gif ..*/
           } else if (stack_mode) {
             data[pos] = 0xffffff00;
           }
         }
       }
       if (stack_mode == 0) {
         anim.imgs[i] = imlib_create_image_using_copied_data(anim.width, anim.height, data);
         anim.loopcnt = gfs->loopcount;
         anim.delay[i] = gfi->delay;
         if (disposal == GIF_DISPOSAL_NONE || disposal == GIF_DISPOSAL_ASIS)
           last = i;
       } else if (i == gfs->nimages-1) {
         anim.imgs[0] = imlib_create_image_using_copied_data(anim.width, anim.height*gfs->nimages, anim.data);
         anim.height *= gfs->nimages;
       }
       disposal = gfi->disposal;
       lastx = gfi->left; lasty = gfi->top; lastw = gfi->width; lasth = gfi->height;
     }
     //imlib_save_image("test.png");
     //progress(anim.imgs[0], 100, 0, 0, 1000, 1000);
     //data = imlib_image_get_data_for_reading_only();
   }
   if (anim.nimg == 0 || !anim.imgs[0]) {
     fprintf(stderr, "Image format not available\n");
     exit(1);
   } else if (query_mode) {
     printf("%dx%d\n", anim.width, anim.height);
     exit(0);
   }
   main_loop();

   return 0;
}
