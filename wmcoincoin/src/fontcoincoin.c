#define NB_MAX_CC_FONTS 100
#define NB_MAX_CC_COLORS 300
#include <assert.h>
#include <stdlib.h>
#include <X11/Xft/Xft.h>
#include "coin_util.h"

#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library ft_library;

typedef int CCFontId;
typedef int CCColorId;

typedef struct CCFont {
  char *fontname;
  XftFont *xfn;
  int refcnt;
  FT_Face face;
} CCFont;


typedef struct CCColor {
  unsigned argb;
  XRenderColor xrc;
  XftColor xfc;
  int refcnt;
} CCColor;

CCFont  *ccfonts[NB_MAX_CC_FONTS] = {NULL,};
CCColor *cccolors[NB_MAX_CC_COLORS] = {NULL,};

XftDraw *xd = 0;
Display *display = 0;
unsigned screen;
Colormap colormap;
Visual *visual = 0;
Window root = None;

void ccfont_initialize(Display *display_, int screen_, Visual *visual_, Colormap colormap_, Drawable d) {
  display = display_;
  screen = screen_;
  visual = visual_;
  colormap = colormap_;
  root = RootWindow(display, screen);
  xd = XftDrawCreate(display, d, visual, colormap);

  FT_Init_FreeType(&ft_library);
}

void
ccfont_release(CCFontId *pid) {
  CCFontId id = *pid;
  if (id == -1) return;
  assert(ccfonts[id]);
  assert(ccfonts[id]->refcnt > 0);
  if (--(ccfonts[id]->refcnt) == 0) {
    if (ccfonts[id]->xfn) { XftFontClose(display, ccfonts[id]->xfn); ccfonts[id]->xfn = NULL; }
    if (ccfonts[id]->fontname) { free(ccfonts[id]->fontname); ccfonts[id]->fontname = NULL; }
    free(ccfonts[id]); ccfonts[id] = NULL;
  }
  *pid = -1;
}

CCFontId
ccfont_get(char *fontdesc_) {
  int i, ifn = -1;
  char *fontdesc;
  if (Prefs.disable_xft_antialiasing) /* xft roxor ! */
    fontdesc = str_printf("%s:antialias=no",fontdesc_);
  else fontdesc = fontdesc_;
  for (i = 0; i < NB_MAX_CC_FONTS; ++i) {
    if (ccfonts[i] == NULL && ifn == -1) ifn = i;
    else if (ccfonts[i] && strcmp(fontdesc, ccfonts[i]->fontname) == 0) {
      ccfonts[i]->refcnt++;
      if (fontdesc_ != fontdesc) free(fontdesc);
      return i;
    }
  }
  if (ifn >= 0) {
    CCFont *cf = ccfonts[ifn] = malloc(sizeof(CCFont)); assert(cf);
    cf->refcnt = 1;
    cf->fontname = strdup(fontdesc);
    cf->xfn = XftFontOpenName(display, screen, fontdesc);
    if (!cf->xfn) { 
      fprintf(stderr, "fontcoincoin could not find any font matching '%s'\n", fontdesc); 
      ccfont_release(&ifn); 
    }
  } else { 
    fprintf(stderr, "max fonts exhausted... leak ?\n");
    assert(0);
  }
  if (fontdesc_ != fontdesc) free(fontdesc);
  return ifn;
}

CCFontId ccfont_incref(CCFontId id) {
  assert(ccfonts[id]);
  assert(ccfonts[id]->refcnt >= 0);
  ccfonts[id]->refcnt++;
  return id;
}

CCColorId
cccolor_get(unsigned argb) {
  int i, ic = -1;
  for (i = 0; i < NB_MAX_CC_COLORS; ++i) {
    if (cccolors[i] == NULL && ic == -1) ic = i;
    else if (cccolors[i] && argb == cccolors[i]->argb) {
      cccolors[i]->refcnt++;
      return i;
    }
  }
  if (ic >= 0) {
    CCColor *cc = cccolors[ic] = malloc(sizeof(CCColor)); assert(cc);
    cc->refcnt = 1;
    cc->argb = argb; 
    cc->xrc.red = MIN(((argb >> 16) & 0xFF) * 256, 65535);
    cc->xrc.green = MIN(((argb >> 8) & 0xFF) * 256, 65535);
    cc->xrc.blue = MIN(((argb >> 0) & 0xFF) * 256, 65535);
    cc->xrc.alpha = 65535 - MIN(((argb >> 24) & 0xFF) * 256, 65535);
    XftColorAllocValue(display, visual, colormap, &cc->xrc,&cc->xfc);
  } else { 
    fprintf(stderr, "max colors exhausted... leak ?\n");
    assert(0);
  }
  return ic;
}

CCColorId 
cccolor_get_rgb(int r255, int g255, int b255) {
  r255 = MAX(0,MIN(255,r255));
  g255 = MAX(0,MIN(255,g255));
  b255 = MAX(0,MIN(255,b255));
  return cccolor_get(r255*65536 + g255*256 + b255);
}

CCColorId
cccolor_from_name(const char *name) {
  XColor xc;
  if (XParseColor(display, colormap, name, &xc)) {
    int rgb = 
      ((((int)xc.red  )/256) << 16) + 
      ((((int)xc.green)/256) << 8) + 
      ((((int)xc.blue )/256));
    return cccolor_get(rgb);
  } else return -1;
}

CCColorId cccolor_incref(CCColorId id) {
  assert(cccolors[id]);
  assert(cccolors[id]->refcnt >= 0);
  cccolors[id]->refcnt++;
  return id;
}

void 
cccolor_release(CCColorId *pid) {
  CCColorId id = *pid;
  if (id == -1) return;
  assert(cccolors[id]);
  assert(cccolors[id]->refcnt > 0);
  if (--(cccolors[id]->refcnt) == 0) {
    XftColorFree(display, visual, colormap, &cccolors[id]->xfc);
    free(cccolors[id]); cccolors[id] = NULL;
  }
  *pid = -1;
}

void 
cccolor_reset(CCColorId *pid, unsigned argb) {
  cccolor_release(pid);
  *pid = cccolor_get(argb);
}

unsigned long cccolor_pixel(CCColorId cid) {
  return cccolors[cid]->xfc.pixel;
}

void
ccfont_draw_string8(CCFontId fid, CCColorId cid, Drawable d, int x, int y, char *str, int len) {
  assert(str);
  assert(ccfonts[fid] && ccfonts[fid]->xfn); 
  assert(cccolors[cid]);
  if (len == -1) len = strlen(str);
  XftDrawChange(xd, d);
  XftDrawString8(xd, &cccolors[cid]->xfc, ccfonts[fid]->xfn, x, y, str, len);
  XftDrawChange(xd, root);
}

void
ccfont_draw_string_utf8(CCFontId fid, CCColorId cid, Drawable d, int x, int y, char *str, int len) {
  assert(str);
  assert(ccfonts[fid] && ccfonts[fid]->xfn); 
  assert(cccolors[cid]);
  if (len == -1) len = strlen(str);
  XftDrawChange(xd, d);
  XftDrawStringUtf8(xd, &cccolors[cid]->xfc, ccfonts[fid]->xfn, x, y, str, len);
  XftDrawChange(xd, root);
}

/* extrait de: http://www.keithp.com/~keithp/render/Xft.tutorial

 (..) to compute the rectangle covered by a single glyph rendered at
x,y:

	top = y - glyphInfo.y;
	left = x - glyphInfo.x;
	bottom = top + glyphInfo.height;
	right = left + glyphInfo.width;

And to compute the normal location for the next glyph:

	x = x + glyphInfo.xOff;
	y = y + glyphInfo.yOff;

*/
int
ccfont_text_xbox(CCFontId fid, char *str, int len, short *pxstart, short *pxoff) {
  XGlyphInfo ext;
  if (len == -1) len = strlen(str);
  XftTextExtents8(display, ccfonts[fid]->xfn, str, len, &ext);
  if (pxstart) *pxstart = ext.x;
  if (pxoff) *pxoff = ext.xOff;
  return ext.width;
}

int
ccfont_text_xbox_utf8(CCFontId fid, char *str, int len, short *pxstart, short *pxoff) {
  XGlyphInfo ext;
  if (len == -1) len = strlen(str);
  XftTextExtentsUtf8(display, ccfonts[fid]->xfn, str, len, &ext);
  if (pxstart) *pxstart = ext.x;
  if (pxoff) *pxoff = ext.xOff;
  return ext.width;
}

int ccfont_text_width8(CCFontId fid, char *str, int len) {
  return ccfont_text_xbox(fid,str,len,NULL,NULL);
}

int ccfont_text_width_utf8(CCFontId fid, char *str, int len) {
  XGlyphInfo ext;
  if (len == -1) len = strlen(str);
  XftTextExtents8(display, ccfonts[fid]->xfn, str, len, &ext);
  XftTextExtentsUtf8(display, ccfonts[fid]->xfn, str, len, &ext);
  return ext.width;
}

int ccfont_text_within_width8(CCFontId fid, char *str, int len, int width, int *final_width) {
  XGlyphInfo ext;
  int w = 0, i = 0;
  if (width > 0) {
    while (str[i] && (i < len || len == -1)) {
      XftTextExtents8(display, ccfonts[fid]->xfn, str+i, 1, &ext);
      //printf("i=%d str[i]='%c', w ?= %d+%d : %d\n",i,str[i], w, ext.width, width); 
      if (w + ext.width <= width)
        w += ext.xOff - ext.x - 1;//width; /* le calcul est pas clair clair .. */
      else break;
      ++i;
    }
  }
  if (final_width) *final_width = w;
  return i;
}

int ccfont_ascent(CCFontId fid) {
  return ccfonts[fid]->xfn->ascent;
}

int ccfont_descent(CCFontId fid) {
  return ccfonts[fid]->xfn->descent;
}

int ccfont_height(CCFontId fid) {
  return ccfonts[fid]->xfn->height;
}
