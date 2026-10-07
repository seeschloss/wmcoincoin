/* role "copy": on key 'c' own PRIMARY, answer SelectionRequests like palmipede does.
 * role "paste": on key 'v' request PRIMARY and print what arrives. */
#define XLIB_ILLEGAL_ACCESS 1
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "xwl.h"
int main(int argc, char **argv)
{
  const char *role = argv[1]; int secs = atoi(argv[2]);
  static const char text[] = "Cop\303\251 depuis xwl: h\303\251llo w\303\266rld \342\202\254";   /* Copé depuis xwl: héllo wörld € */
  Display *dpy = XOpenDisplay(NULL); if (!dpy) return 1;
  Window w = XCreateSimpleWindow(dpy, RootWindow(dpy, 0), 0, 0, 200, 100, 0, 0, strcmp(role, "copy") ? 0xddddff : 0xffdddd);
  XSelectInput(dpy, w, ExposureMask | StructureNotifyMask | KeyPressMask | FocusChangeMask);
  XStoreName(dpy, w, role); XMapRaised(dpy, w); setbuf(stdout, NULL);
  Atom utf8 = XInternAtom(dpy, "UTF8_STRING", False), targets = XInternAtom(dpy, "TARGETS", False);
  time_t end = time(NULL) + secs;
  while (time(NULL) < end) {
    while (XPending(dpy)) {
      XEvent e; XNextEvent(dpy, &e);
      if (e.type == FocusIn) printf("[%s] FocusIn\n", role);
      if (e.type == KeyPress) {
        char b[8]; KeySym ks; XLookupString(&e.xkey, b, 8, &ks, NULL);
        printf("[%s] key %lx\n", role, (long)ks);
        if (ks == 'c') { XSetSelectionOwner(dpy, XA_PRIMARY, w, CurrentTime);
          printf("[copy] owner now %s\n", XGetSelectionOwner(dpy, XA_PRIMARY) == w ? "me" : "someone else"); }
        if (ks == 'v') { printf("[paste] owner=%lx\n", XGetSelectionOwner(dpy, XA_PRIMARY));
          XConvertSelection(dpy, XA_PRIMARY, utf8, XInternAtom(dpy, "VT_SELECTION", False), w, CurrentTime); }
      }
      if (e.type == SelectionRequest) {            /* same logic as palmipede.c */
        XSelectionRequestEvent *rq = &e.xselectionrequest; XEvent ev; memset(&ev, 0, sizeof ev);
        printf("[copy] SelectionRequest target=%s\n", rq->target == utf8 ? "UTF8_STRING" : "other");
        ev.xselection.type = SelectionNotify; ev.xselection.property = None; ev.xselection.display = rq->display;
        ev.xselection.requestor = rq->requestor; ev.xselection.selection = rq->selection;
        ev.xselection.target = rq->target; ev.xselection.time = rq->time;
        if (rq->target == utf8) {
          XChangeProperty(dpy, rq->requestor, rq->property, rq->target, 8, PropModeReplace, (unsigned char *)text, strlen(text));
          ev.xselection.property = rq->property;
        } else if (rq->target == targets) { }
        XSendEvent(dpy, rq->requestor, 0, 0, &ev);
      }
      if (e.type == SelectionClear) printf("[copy] SelectionClear\n");
      if (e.type == SelectionNotify) {
        unsigned long n, after; unsigned char *data = NULL; Atom t; int f;
        XGetWindowProperty(dpy, e.xselection.requestor, e.xselection.property, 0, 256, True, AnyPropertyType, &t, &f, &n, &after, &data);
        printf("[paste] SelectionNotify prop=%lu got %lu bytes: '%s'\n", e.xselection.property, n, data ? (char *)data : "(none)");
      }
    }
    usleep(20000);
  }
  return 0;
}
