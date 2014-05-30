#ifndef FONTCOINCOIN_H
#define FONTCOINCOIN_H

/* petite couche entre le coincoin et Xft/Xrender */

typedef int CCFontId;
typedef int CCColorId;

void ccfont_initialize(Display *display_, int screen_, Visual *visual_, Colormap colormap_, Drawable d);
CCFontId ccfont_get(char *fontdesc);
CCFontId ccfont_incref(CCFontId id);
void ccfont_release(CCFontId *id);
CCColorId cccolor_get(unsigned argb);
CCColorId cccolor_get_rgb(int r255, int g255, int b255);
CCColorId cccolor_from_name(const char *name);
CCColorId cccolor_incref(CCColorId id);
void cccolor_release(CCColorId *id);
void cccolor_reset(CCColorId *pid, unsigned argb);
unsigned long cccolor_pixel(CCColorId cid);
void ccfont_draw_string8(CCFontId fid, CCColorId cid, Drawable d, int x, int y, char *str, int len);
int ccfont_text_xbox(CCFontId fid, const char *str, int len, short *pxstart, short *pxoff);
int ccfont_text_width8(CCFontId fid, const char *str, int len);
int ccfont_text_within_width8(CCFontId fid, const char *str, int len, int width, int *final_width);
int ccfont_ascent(CCFontId fid);
int ccfont_descent(CCFontId fid);
int ccfont_height(CCFontId fid);
#endif
