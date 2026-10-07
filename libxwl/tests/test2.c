#define XLIB_ILLEGAL_ACCESS 1
#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "xwl.h"
typedef int CCFontId; typedef int CCColorId;
void ccfont_initialize(Display *, int, Visual *, Colormap, Drawable);
CCFontId ccfont_get(char *); CCColorId cccolor_get(unsigned); CCColorId cccolor_from_name(const char *);
unsigned long cccolor_pixel(CCColorId);
void ccfont_draw_string8(CCFontId, CCColorId, Drawable, int, int, char *, int);
void ccfont_draw_string_utf8(CCFontId, CCColorId, Drawable, int, int, char *, int);
int ccfont_text_xbox(CCFontId, const char *, int, short *, short *);
int ccfont_text_width_utf8(CCFontId, char *, int);
int ccfont_text_within_width8(CCFontId, const char *, int, int, int *);
int ccfont_ascent(CCFontId), ccfont_descent(CCFontId), ccfont_height(CCFontId);

int main(void)
{
  Display *dpy = XOpenDisplay(NULL); if (!dpy) return 1;
  Window w = XCreateSimpleWindow(dpy, RootWindow(dpy, 0), 0, 0, 480, 220, 0, 0, 0xffffff);
  XSelectInput(dpy, w, ExposureMask | StructureNotifyMask); XMapWindow(dpy, w);
  for (int i = 0, ok = 0; i < 100 && !ok; i++) { XEvent e; while (XPending(dpy)) { XNextEvent(dpy, &e); if (e.type == Expose) ok = 1; } usleep(20000); }
  ccfont_initialize(dpy, 0, DefaultVisual(dpy, 0), 0, w);
  const char *names[] = { "DejaVu Sans:size=10", "DejaVu Sans Mono:pixelsize=12", "DejaVu Sans:bold:size=12",
                          "DejaVu Sans:pixelsize=13:antialias=no", "nonexistent-font-xyz:size=9" };
  CCColorId col = cccolor_get(0x202020), red = cccolor_from_name("#c03030"), blu = cccolor_from_name("navy");
  printf("pixel(red)=%06lx pixel(navy)=%06lx\n", cccolor_pixel(red), cccolor_pixel(blu));
  char latin1[] = "\311t\351 \340 Gen\350ve: \347a marche \253ok\273";   /* Été à Genève: ça marche «ok» */
  int y = 20;
  for (int i = 0; i < 5; i++) {
    CCFontId f = ccfont_get((char *)names[i]);
    if (f < 0) { printf("font %d (%s): not found -> fallback match used\n", i, names[i]); continue; }
    short xs, xo; int wd = ccfont_text_xbox(f, latin1, -1, &xs, &xo);
    int fit, fw; fit = ccfont_text_within_width8(f, latin1, -1, 100, &fw);
    printf("%-40s asc=%d desc=%d h=%d  width=%d xstart=%d xoff=%d  fit100=%d(%dpx)\n", names[i],
           ccfont_ascent(f), ccfont_descent(f), ccfont_height(f), wd, xs, xo, fit, fw);
    ccfont_draw_string8(f, i == 2 ? red : col, w, 8, y, latin1, -1);
    y += ccfont_height(f) + 6;
  }
  CCFontId f = ccfont_get("DejaVu Sans:size=10");
  char u8[] = "UTF-8: \303\251\303\240 \342\202\254 \316\261\316\262\316\263";   /* é à € αβγ */
  ccfont_draw_string_utf8(f, blu, w, 8, y, u8, -1);
  printf("utf8 width=%d\n", ccfont_text_width_utf8(f, u8, -1));
  XFlush(dpy); XSync(dpy, False);
  xwl_write_png(dpy, w, "/tmp/test2.png");
  return 0;
}
