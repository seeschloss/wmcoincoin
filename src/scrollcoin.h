#ifndef __SCROLLCOIN_H
#define __SCROLLCOIN_H

#include "raster.h"

#define SC_W 10

typedef struct _ScrollCoin ScrollCoin;

void scrollcoin_build(RGBAContext *rgbactx);
void scrollcoin_change_colors(ScrollCoin *sc, int opaque_color);
ScrollCoin *scrollcoin_create(int vmin, int vmax, int pos, int x, int y, int h, int opaque_color);
void scrollcoin_destroy(ScrollCoin *sc);
void scrollcoin_refresh(ScrollCoin *sc, Drawable d, int force);
int scrollcoin_get_vmax(const ScrollCoin *sc);
int scrollcoin_get_vmin(const ScrollCoin *sc);
void scrollcoin_setpos(ScrollCoin *sc, int new_pos);
int scrollcoin_read_requested_pos(ScrollCoin *sc, int *pos);
void scrollcoin_resize(ScrollCoin *sc, int xpos, int ypos, int h);
void scrollcoin_setbounds(ScrollCoin *sc, int vmin, int vmax);
int scrollcoin_handle_button_press(ScrollCoin *sc, XButtonEvent *ev, Drawable d);
int scrollcoin_handle_button_release(ScrollCoin *sc, XButtonEvent *ev, Drawable d);
int scrollcoin_handle_motion(ScrollCoin *sc, XMotionEvent *ev, Drawable d);
#endif
