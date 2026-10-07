/* libxwl: wl_seat -> X events (keyboard via xkbcommon, pointer, wheel). */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <locale.h>
#include <linux/input-event-codes.h>
#include "xwl_priv.h"

static long now_ms(void)
{ struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts); return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L; }

static unsigned x_state(XwlDisplay *d)
{
  unsigned s = 0;
  struct xkb_state *k = d->xkb_st;
  if (k) {
    static const struct { const char *n; unsigned m; } mods[] = {
      {XKB_MOD_NAME_SHIFT, ShiftMask}, {XKB_MOD_NAME_CAPS, LockMask}, {XKB_MOD_NAME_CTRL, ControlMask},
      {XKB_MOD_NAME_ALT, Mod1Mask}, {XKB_MOD_NAME_NUM, Mod2Mask}, {XKB_MOD_NAME_LOGO, Mod4Mask},
    };
    for (unsigned i = 0; i < sizeof mods / sizeof *mods; i++)
      if (xkb_state_mod_name_is_active(k, mods[i].n, XKB_STATE_MODS_EFFECTIVE) > 0) s |= mods[i].m;
  }
  if (d->ptr_buttons & 1) s |= Button1Mask;
  if (d->ptr_buttons & 2) s |= Button2Mask;
  if (d->ptr_buttons & 4) s |= Button3Mask;
  return s;
}

static int button_of(uint32_t code)
{
  switch (code) { case BTN_LEFT: return Button1; case BTN_MIDDLE: return Button2;
                  case BTN_RIGHT: return Button3; default: return 0; }
}

/* ---------------- pointer ---------------- */
static void fill_xy(XEvent *e, XwlDisplay *d)
{
  e->xbutton.x = e->xbutton.x_root = d->ptr_x;   /* same layout for button/motion/crossing */
  e->xbutton.y = e->xbutton.y_root = d->ptr_y;
  e->xbutton.state = x_state(d);
  e->xbutton.time = d->last_time;
  e->xbutton.same_screen = True;
}

static void p_enter(void *data, struct wl_pointer *p, uint32_t serial, struct wl_surface *s,
                    wl_fixed_t x, wl_fixed_t y)
{
  XwlDisplay *d = data; (void)p;
  d->last_serial = serial;
  d->ptr_win = s ? wl_surface_get_user_data(s) : NULL;
  d->ptr_x = wl_fixed_to_int(x); d->ptr_y = wl_fixed_to_int(y);
  if (d->ptr_win && xwl_wants((Window)(uintptr_t)d->ptr_win, EnterWindowMask)) {
    XEvent *e = xwl_push_event(d, EnterNotify, (Window)(uintptr_t)d->ptr_win);
    fill_xy(e, d); e->xcrossing.mode = NotifyNormal; e->xcrossing.detail = NotifyNonlinear;
  }
}
static void p_leave(void *data, struct wl_pointer *p, uint32_t serial, struct wl_surface *s)
{
  XwlDisplay *d = data; (void)p; (void)s; d->last_serial = serial;
  if (d->ptr_win && xwl_wants((Window)(uintptr_t)d->ptr_win, LeaveWindowMask)) {
    XEvent *e = xwl_push_event(d, LeaveNotify, (Window)(uintptr_t)d->ptr_win);
    fill_xy(e, d); e->xcrossing.mode = NotifyNormal; e->xcrossing.detail = NotifyNonlinear;
  }
  d->ptr_win = NULL;
}
static void p_motion(void *data, struct wl_pointer *p, uint32_t t, wl_fixed_t x, wl_fixed_t y)
{
  XwlDisplay *d = data; (void)p;
  d->last_time = t; d->ptr_x = wl_fixed_to_int(x); d->ptr_y = wl_fixed_to_int(y);
  if (!d->ptr_win) return;
  Window w = (Window)(uintptr_t)d->ptr_win;
  long need = PointerMotionMask;
  if (d->ptr_buttons) need |= ButtonMotionMask;
  if (d->ptr_buttons & 1) need |= Button1MotionMask;
  if (d->ptr_buttons & 2) need |= Button2MotionMask;
  if (d->ptr_buttons & 4) need |= Button3MotionMask;
  if (!xwl_wants(w, need)) return;
  XEvent *e = xwl_push_event(d, MotionNotify, w);
  fill_xy(e, d); e->xmotion.is_hint = NotifyNormal;
}
static void send_button(XwlDisplay *d, int b, int press, uint32_t t)
{
  if (!d->ptr_win) return;
  Window w = (Window)(uintptr_t)d->ptr_win;
  if (!xwl_wants(w, press ? ButtonPressMask : ButtonReleaseMask)) return;
  XEvent *e = xwl_push_event(d, press ? ButtonPress : ButtonRelease, w);
  fill_xy(e, d); e->xbutton.button = b; e->xbutton.time = t;
}
static void p_button(void *data, struct wl_pointer *p, uint32_t serial, uint32_t t, uint32_t b, uint32_t st)
{
  XwlDisplay *d = data; (void)p;
  int xb = button_of(b); if (!xb) return;
  d->last_serial = serial; d->last_time = t;
  unsigned bit = 1u << (xb - 1);
  int press = st == WL_POINTER_BUTTON_STATE_PRESSED;
  if (press) { send_button(d, xb, 1, t); d->ptr_buttons |= bit; }
  else { d->ptr_buttons &= ~bit; send_button(d, xb, 0, t); }
  /* X reports modifier state *before* the event; for release, re-add the bit */
  if (!press && d->evtail) d->evtail->ev.xbutton.state |= (Button1Mask << (xb - 1));
}
static void p_axis(void *data, struct wl_pointer *p, uint32_t t, uint32_t axis, wl_fixed_t v)
{
  XwlDisplay *d = data; (void)p;
  if (axis != WL_POINTER_AXIS_VERTICAL_SCROLL) return;
  d->axis_acc += wl_fixed_to_double(v);
  while (d->axis_acc <= -10 || d->axis_acc >= 10) {
    int b = d->axis_acc < 0 ? Button4 : Button5;
    d->axis_acc += d->axis_acc < 0 ? 10 : -10;
    send_button(d, b, 1, t); send_button(d, b, 0, t);
  }
}
static void p_frame(void *d, struct wl_pointer *p) { (void)d; (void)p; }
static void p_axis_source(void *d, struct wl_pointer *p, uint32_t s) { (void)d; (void)p; (void)s; }
static void p_axis_stop(void *d, struct wl_pointer *p, uint32_t t, uint32_t a) { (void)d; (void)p; (void)t; (void)a; }
static void p_axis_discrete(void *d, struct wl_pointer *p, uint32_t a, int32_t v) { (void)d; (void)p; (void)a; (void)v; }
static const struct wl_pointer_listener pointer_listener = {
  p_enter, p_leave, p_motion, p_button, p_axis, p_frame, p_axis_source, p_axis_stop, p_axis_discrete
};

/* ---------------- keyboard ---------------- */
static void k_keymap(void *data, struct wl_keyboard *k, uint32_t fmt, int32_t fd, uint32_t size)
{
  XwlDisplay *d = data; (void)k;
  if (fmt != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) { close(fd); return; }
  char *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  if (map == MAP_FAILED) return;
  struct xkb_keymap *km = xkb_keymap_new_from_string(d->xkb_ctx, map, XKB_KEYMAP_FORMAT_TEXT_V1, 0);
  munmap(map, size);
  if (!km) return;
  if (d->xkb_st) xkb_state_unref(d->xkb_st);
  if (d->xkb_map) xkb_keymap_unref(d->xkb_map);
  d->xkb_map = km; d->xkb_st = xkb_state_new(km);
}
static void k_enter(void *data, struct wl_keyboard *k, uint32_t serial, struct wl_surface *s, struct wl_array *keys)
{
  XwlDisplay *d = data; (void)k; (void)keys; d->last_serial = serial;
  if (xwl_debug_on()) fprintf(stderr, "xwl: keyboard enter surface=%p\n", (void *)s);
  d->kbd_win = s ? wl_surface_get_user_data(s) : NULL;
  if (d->kbd_win && xwl_wants((Window)(uintptr_t)d->kbd_win, FocusChangeMask)) {
    XEvent *e = xwl_push_event(d, FocusIn, (Window)(uintptr_t)d->kbd_win);
    e->xfocus.mode = NotifyNormal; e->xfocus.detail = NotifyNonlinear;
  }
}
static void k_leave(void *data, struct wl_keyboard *k, uint32_t serial, struct wl_surface *s)
{
  XwlDisplay *d = data; (void)k; (void)s; d->last_serial = serial; d->rep_active = 0;
  if (d->kbd_win && xwl_wants((Window)(uintptr_t)d->kbd_win, FocusChangeMask)) {
    XEvent *e = xwl_push_event(d, FocusOut, (Window)(uintptr_t)d->kbd_win);
    e->xfocus.mode = NotifyNormal; e->xfocus.detail = NotifyNonlinear;
  }
  d->kbd_win = NULL;
}

static void emit_key(XwlDisplay *d, uint32_t key, int press, uint32_t t)
{
  if (xwl_debug_on()) fprintf(stderr, "xwl: key %u press=%d kbd_win=%p wants=%d\n", key, press, (void *)d->kbd_win, d->kbd_win ? xwl_wants((Window)(uintptr_t)d->kbd_win, KeyPressMask) : -1);
  if (!d->kbd_win || !d->xkb_st) return;
  Window w = (Window)(uintptr_t)d->kbd_win;
  if (!xwl_wants(w, press ? KeyPressMask : KeyReleaseMask)) return;
  xkb_keycode_t kc = key + 8;
  XEvent *e = xwl_push_event(d, press ? KeyPress : KeyRelease, w);
  e->xkey.keycode = kc; e->xkey.state = x_state(d); e->xkey.time = t;
  e->xkey.x = d->ptr_x; e->xkey.y = d->ptr_y; e->xkey.same_screen = True;
  XwlKeyInfo *ki = &d->keyinfo[e->xany.serial % 64];
  ki->serial = (uint32_t)e->xany.serial;
  ki->sym = xkb_state_key_get_one_sym(d->xkb_st, kc);
  ki->len = xkb_state_key_get_utf8(d->xkb_st, kc, ki->utf8, sizeof ki->utf8);
  if (ki->len < 0 || ki->len >= (int)sizeof ki->utf8) ki->len = 0;
  /* dead keys / Compose sequences (replaces what XIM did on X11) */
  if (press && d->compose_st && ki->sym != XKB_KEY_NoSymbol) {
    xkb_compose_state_feed(d->compose_st, ki->sym);
    switch (xkb_compose_state_get_status(d->compose_st)) {
    case XKB_COMPOSE_COMPOSING:
      ki->sym = NoSymbol; ki->len = 0; break;                       /* swallow, like XLookupNone */
    case XKB_COMPOSE_COMPOSED:
      ki->sym = xkb_compose_state_get_one_sym(d->compose_st);
      ki->len = xkb_compose_state_get_utf8(d->compose_st, ki->utf8, sizeof ki->utf8);
      if (ki->len < 0 || ki->len >= (int)sizeof ki->utf8) ki->len = 0;
      xkb_compose_state_reset(d->compose_st); break;
    case XKB_COMPOSE_CANCELLED:
      ki->sym = NoSymbol; ki->len = 0; xkb_compose_state_reset(d->compose_st); break;
    default: break;
    }
  }
  /* Ctrl+letter: xkb already yields the control char in utf8 */
}
static void k_key(void *data, struct wl_keyboard *k, uint32_t serial, uint32_t t, uint32_t key, uint32_t st)
{
  XwlDisplay *d = data; (void)k;
  d->last_serial = serial; d->last_time = t;
  int press = st == WL_KEYBOARD_KEY_STATE_PRESSED;
  emit_key(d, key, press, t);
  if (press && d->xkb_map && xkb_keymap_key_repeats(d->xkb_map, key + 8) && d->rep_rate > 0) {
    d->rep_key = key; d->rep_active = 1; d->rep_next = now_ms() + d->rep_delay;
  } else if (!press && d->rep_key == key) d->rep_active = 0;
}
static void k_mods(void *data, struct wl_keyboard *k, uint32_t serial, uint32_t dep, uint32_t lat,
                   uint32_t lock, uint32_t grp)
{
  XwlDisplay *d = data; (void)k; (void)serial;
  if (d->xkb_st) xkb_state_update_mask(d->xkb_st, dep, lat, lock, 0, 0, grp);
}
static void k_repeat(void *data, struct wl_keyboard *k, int32_t rate, int32_t delay)
{ XwlDisplay *d = data; (void)k; d->rep_rate = rate; d->rep_delay = delay; }
static const struct wl_keyboard_listener keyboard_listener = {
  k_keymap, k_enter, k_leave, k_key, k_mods, k_repeat
};

void xwl_input_pump(XwlDisplay *d)
{
  if (!d->rep_active || d->rep_rate <= 0) return;
  long n = now_ms();
  int guard = 0;
  while (n >= d->rep_next && guard++ < 5) {
    emit_key(d, d->rep_key, 1, d->last_time);
    d->rep_next += 1000 / d->rep_rate;
  }
  if (guard >= 5) d->rep_next = n + 1000 / d->rep_rate;
}

/* ---------------- seat ---------------- */
static void seat_caps(void *data, struct wl_seat *s, uint32_t caps)
{
  XwlDisplay *d = data;
  int has_ptr = caps & WL_SEAT_CAPABILITY_POINTER, has_kbd = caps & WL_SEAT_CAPABILITY_KEYBOARD;
  if (has_ptr && !d->pointer) {
    d->pointer = wl_seat_get_pointer(s); wl_pointer_add_listener(d->pointer, &pointer_listener, d);
  } else if (!has_ptr && d->pointer) {
    if (wl_seat_get_version(s) >= 3) wl_pointer_release(d->pointer); else wl_pointer_destroy(d->pointer);
    d->pointer = NULL; d->ptr_win = NULL; d->ptr_buttons = 0;
  }
  if (has_kbd && !d->keyboard) {
    d->keyboard = wl_seat_get_keyboard(s); wl_keyboard_add_listener(d->keyboard, &keyboard_listener, d);
  } else if (!has_kbd && d->keyboard) {
    if (wl_seat_get_version(s) >= 3) wl_keyboard_release(d->keyboard); else wl_keyboard_destroy(d->keyboard);
    d->keyboard = NULL; d->kbd_win = NULL; d->rep_active = 0;
  }
}
static void seat_name(void *d, struct wl_seat *s, const char *n) { (void)d; (void)s; (void)n; }
static const struct wl_seat_listener seat_listener = { seat_caps, seat_name };

void xwl_input_seat_bound(XwlDisplay *d) { wl_seat_add_listener(d->seat, &seat_listener, d); }
void xwl_input_init(XwlDisplay *d)
{
  d->xkb_ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  d->rep_rate = 25; d->rep_delay = 400;
  { /* first non-empty of LC_ALL / LC_CTYPE / LANG (an *empty* variable counts as unset) */
    const char *loc = NULL, *vars[] = { "LC_ALL", "LC_CTYPE", "LANG" };
    for (int k = 0; k < 3 && !loc; k++) { const char *v = getenv(vars[k]); if (v && *v) loc = v; }
    if (!loc) loc = "C";
    xkb_context_set_log_level(d->xkb_ctx, XKB_LOG_LEVEL_CRITICAL);   /* a missing Compose table is not an error */
    d->compose_tbl = xkb_compose_table_new_from_locale(d->xkb_ctx, loc, XKB_COMPOSE_COMPILE_NO_FLAGS);
    if (!d->compose_tbl && xwl_debug_on()) fprintf(stderr, "xwl: no Compose table for locale '%s': dead keys will not compose\n", loc);
    if (d->compose_tbl) d->compose_st = xkb_compose_state_new(d->compose_tbl, XKB_COMPOSE_STATE_NO_FLAGS); }
  wl_display_roundtrip(d->wl);   /* get capabilities */
  wl_display_roundtrip(d->wl);   /* get keymap / repeat info */
}

/* ---------------- Xlib keyboard API ---------------- */
int XLookupString(XKeyEvent *ev, char *buf, int n, KeySym *ks, XComposeStatus *st)
{
  XwlDisplay *d = (XwlDisplay *)ev->display; (void)st;
  XwlKeyInfo *ki = &d->keyinfo[ev->serial % 64];
  if (ki->serial != (uint32_t)ev->serial) { if (ks) *ks = NoSymbol; return 0; }
  if (ks) *ks = ki->sym;
  int len = ki->len < n ? ki->len : n;
  memcpy(buf, ki->utf8, len);
  if (len < n) buf[len] = 0;
  return len;
}
KeySym XLookupKeysym(XKeyEvent *ev, int idx)
{ XwlDisplay *d = (XwlDisplay *)ev->display; (void)idx;
  XwlKeyInfo *ki = &d->keyinfo[ev->serial % 64];
  return ki->serial == (uint32_t)ev->serial ? ki->sym : NoSymbol; }
int XSetInputFocus(Display *dpy, Window w, int rev, Time t) { (void)dpy; (void)w; (void)rev; (void)t; return 0; }
int XGetInputFocus(Display *dpy, Window *w, int *rev)
{ XwlDisplay *d = (XwlDisplay *)dpy; *w = (Window)(uintptr_t)d->kbd_win; if (rev) *rev = RevertToNone; return 0; }
