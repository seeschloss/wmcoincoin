#define XLIB_ILLEGAL_ACCESS 1
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include "xwl.h"

int main(void)
{
  Display *dpy = XOpenDisplay(NULL);
  if (!dpy) { fprintf(stderr, "no display\n"); return 1; }
  int scr = DefaultScreen(dpy);
  printf("depth=%d vis red_mask=%06lx screen=%dx%d\n", DefaultDepth(dpy, scr),
         DefaultVisual(dpy, scr)->red_mask, DisplayWidth(dpy, scr), DisplayHeight(dpy, scr));
  Window w = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 0, 0, 400, 200, 0, 0, 0xf0f0e0);
  XSelectInput(dpy, w, ExposureMask | StructureNotifyMask | ButtonPressMask | KeyPressMask |
               PointerMotionMask | EnterWindowMask | LeaveWindowMask);
  XStoreName(dpy, w, "xwl test");
  XMapRaised(dpy, w);

  GC gc = XCreateGC(dpy, w, 0, NULL);
  Pixmap pm = XCreatePixmap(dpy, w, 100, 50, DefaultDepth(dpy, scr));
  XSetForeground(dpy, gc, 0x2040a0); XFillRectangle(dpy, pm, gc, 0, 0, 100, 50);
  XSetForeground(dpy, gc, 0xffd000); XDrawRectangle(dpy, pm, gc, 2, 2, 95, 45);
  XDrawLine(dpy, pm, gc, 2, 2, 97, 47);

  int exposed = 0;
  for (int i = 0; i < 100 && !exposed; i++) {
    XEvent ev;
    while (XPending(dpy)) {
      XNextEvent(dpy, &ev);
      printf("event type=%d\n", ev.type);
      if (ev.type == Expose) exposed = 1;
      if (ev.type == ConfigureNotify) printf("  configure %dx%d\n", ev.xconfigure.width, ev.xconfigure.height);
    }
    if (!exposed) usleep(20000);
  }
  if (!exposed) { printf("FAIL: never exposed\n"); return 2; }

  XSetForeground(dpy, gc, 0xc03030); XFillRectangle(dpy, w, gc, 10, 10, 60, 40);
  XSetForeground(dpy, gc, 0x000000); XDrawRectangle(dpy, w, gc, 10, 10, 60, 40);
  XDrawLine(dpy, w, gc, 0, 100, 399, 100);
  XSetLineAttributes(dpy, gc, 1, LineOnOffDash, CapButt, JoinMiter);
  XCopyArea(dpy, pm, w, gc, 0, 0, 100, 50, 150, 20);
  for (int i = 0; i < 20; i++) XDrawPoint(dpy, w, gc, 300 + i, 30 + i);
  XFlush(dpy); XSync(dpy, False);
  for (int i = 0; i < 5; i++) { XPending(dpy); usleep(20000); }
  printf("png: %d\n", xwl_write_png(dpy, w, "/tmp/test1.png"));
  XDestroyWindow(dpy, w); XCloseDisplay(dpy);
  puts("OK");
  return 0;
}
