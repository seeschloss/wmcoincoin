#define XLIB_ILLEGAL_ACCESS 1
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "xwl.h"
int main(int argc, char **argv)
{
  int secs = argc > 1 ? atoi(argv[1]) : 6;
  Display *dpy = XOpenDisplay(NULL); if (!dpy) return 1;
  Window w = XCreateSimpleWindow(dpy, RootWindow(dpy, 0), 0, 0, 300, 200, 0, 0, 0xddeedd);
  XSelectInput(dpy, w, ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask |
      KeyReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask);
  XStoreName(dpy, w, "xwl-input-test"); XMapRaised(dpy, w);
  GC gc = XCreateGC(dpy, w, 0, NULL);
  time_t end = time(NULL) + secs; setbuf(stdout, NULL);
  while (time(NULL) < end) {
    while (XPending(dpy)) {
      XEvent e; XNextEvent(dpy, &e);
      switch (e.type) {
      case Expose: XSetForeground(dpy, gc, 0x3060c0); XFillRectangle(dpy, w, gc, 20, 20, 100, 60); printf("Expose\n"); break;
      case ConfigureNotify: printf("Configure %dx%d\n", e.xconfigure.width, e.xconfigure.height); break;
      case FocusIn: printf("FocusIn\n"); break; case FocusOut: printf("FocusOut\n"); break;
      case EnterNotify: printf("Enter %d,%d\n", e.xcrossing.x, e.xcrossing.y); break;
      case LeaveNotify: printf("Leave\n"); break;
      case MotionNotify: printf("Motion %d,%d state=%x\n", e.xmotion.x, e.xmotion.y, e.xmotion.state); break;
      case ButtonPress: printf("ButtonPress %d at %d,%d state=%x\n", e.xbutton.button, e.xbutton.x, e.xbutton.y, e.xbutton.state); break;
      case ButtonRelease: printf("ButtonRelease %d state=%x\n", e.xbutton.button, e.xbutton.state); break;
      case KeyPress: case KeyRelease: { char buf[16] = {0}; KeySym ks;
        int n = XLookupString(&e.xkey, buf, 15, &ks, NULL);
        printf("%s keycode=%u sym=0x%lx text='%s'(%d) state=%x\n", e.type == KeyPress ? "KeyPress" : "KeyRelease",
               e.xkey.keycode, (long)ks, buf, n, e.xkey.state); } break;
      case ClientMessage: printf("ClientMessage (close)\n"); return 0;
      }
    }
    usleep(20000);
  }
  return 0;
}
