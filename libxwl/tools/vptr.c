/* Build (from this directory):
 *   wayland-scanner client-header wlr-virtual-pointer-unstable-v1.xml vptr-proto.h
 *   wayland-scanner private-code  wlr-virtual-pointer-unstable-v1.xml vptr-proto.c
 *   cc -o vptr vptr.c vptr-proto.c $(pkg-config --cflags --libs wayland-client)
 *
 * test driver: keeps one virtual pointer alive and runs a script.
 *   a X Y      absolute move (1024x768 extent)
 *   b CODE 1|0 button (272=left 273=right 274=middle)
 *   s V        vertical scroll, value V
 *   w MS       wait
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wayland-client.h>
#include "vptr-proto.h"
static struct zwlr_virtual_pointer_manager_v1 *mgr; static struct wl_seat *seat;
static void g(void *d, struct wl_registry *r, uint32_t n, const char *i, uint32_t v)
{ (void)d;
  if (!strcmp(i, "zwlr_virtual_pointer_manager_v1")) mgr = wl_registry_bind(r, n, &zwlr_virtual_pointer_manager_v1_interface, 1);
  else if (!strcmp(i, "wl_seat") && !seat) seat = wl_registry_bind(r, n, &wl_seat_interface, 1); (void)v; }
static void rm(void *d, struct wl_registry *r, uint32_t n) { (void)d; (void)r; (void)n; }
static const struct wl_registry_listener rl = { g, rm };
int main(int argc, char **argv)
{
  struct wl_display *d = wl_display_connect(NULL); if (!d) return 1;
  wl_registry_add_listener(wl_display_get_registry(d), &rl, NULL); wl_display_roundtrip(d);
  if (!mgr) { fprintf(stderr, "no virtual pointer manager\n"); return 2; }
  struct zwlr_virtual_pointer_v1 *p = zwlr_virtual_pointer_manager_v1_create_virtual_pointer(mgr, seat);
  wl_display_roundtrip(d); usleep(200000);
  unsigned t = 1000;
  for (int i = 1; i < argc; i++) {
    char c = argv[i][0]; int x, y, v;
    if (c == 'a' && sscanf(argv[i], "a %d %d", &x, &y) == 2) { zwlr_virtual_pointer_v1_motion_absolute(p, t, x, y, 1024, 768); zwlr_virtual_pointer_v1_frame(p); }
    else if (c == 'b' && sscanf(argv[i], "b %d %d", &x, &y) == 2) { zwlr_virtual_pointer_v1_button(p, t, x, y); zwlr_virtual_pointer_v1_frame(p); }
    else if (c == 's' && sscanf(argv[i], "s %d", &v) == 1) {
      zwlr_virtual_pointer_v1_axis_source(p, 0); zwlr_virtual_pointer_v1_axis(p, t, 0, wl_fixed_from_int(v)); zwlr_virtual_pointer_v1_frame(p); }
    else if (c == 'w' && sscanf(argv[i], "w %d", &v) == 1) { wl_display_flush(d); usleep(v * 1000); }
    t += 20; wl_display_flush(d); usleep(30000);
  }
  wl_display_roundtrip(d); return 0;
}
