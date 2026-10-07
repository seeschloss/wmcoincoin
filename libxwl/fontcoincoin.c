/* fontcoincoin: thin font/colour layer for wmcoincoin.
 *
 * Wayland port: this replaces the Xft/Xrender implementation with
 * fontconfig + cairo-ft, drawing into libxwl surfaces.  The public API
 * (fontcoincoin.h) is unchanged.
 *
 * Text handling note: the rest of wmcoincoin stores messages as Latin-1
 * ("8" functions) or UTF-8 ("utf8" functions).  Cairo wants UTF-8, so the
 * 8-bit entry points transcode first.
 */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <fontconfig/fontconfig.h>
#include <cairo.h>
#include <cairo-ft.h>
#include "xwl.h"

#ifdef XWL_FONT_TEST
# define MIN(a,b) ((a)<(b)?(a):(b))
# define MAX(a,b) ((a)>(b)?(a):(b))
  static struct { int disable_xft_antialiasing; } Prefs;
#else
# include "coin_util.h"
# include "global.h"
#endif

#define NB_MAX_CC_FONTS 100
#define NB_MAX_CC_COLORS 300
#define CC_DPI 96.0

typedef int CCFontId;
typedef int CCColorId;

typedef struct CCFont {
  char *fontname;
  cairo_scaled_font_t *sf;
  cairo_font_extents_t fe;
  int ascent, descent, height;
  int refcnt;
} CCFont;

typedef struct CCColor {
  unsigned argb;
  double r, g, b, a;
  unsigned long pixel;
  int refcnt;
} CCColor;

static CCFont  *ccfonts[NB_MAX_CC_FONTS];
static CCColor *cccolors[NB_MAX_CC_COLORS];

static Display *display;
static cairo_surface_t *scratch_surf;   /* for measuring */
static cairo_t *scratch;

void ccfont_initialize(Display *display_, int screen_, Visual *visual_, Colormap colormap_, Drawable d)
{
  (void)screen_; (void)visual_; (void)colormap_; (void)d;
  display = display_;
  FcInit();
  scratch_surf = cairo_image_surface_create(CAIRO_FORMAT_A8, 1, 1);
  scratch = cairo_create(scratch_surf);
}

/* ---------------- Latin-1 -> UTF-8 ---------------- */
static char *latin1_to_utf8(const char *s, int len, char *out)
{
  char *o = out;
  for (int i = 0; i < len; i++) {
    unsigned char c = s[i];
    if (c < 0x80) *o++ = c;
    else { *o++ = 0xC0 | (c >> 6); *o++ = 0x80 | (c & 0x3F); }
  }
  *o = 0;
  return out;
}
/* utf8 inputs may be unterminated slices: copy to a NUL-terminated buffer */
static char *slice(const char *s, int len, char *out)
{ memcpy(out, s, len); out[len] = 0; return out; }

/* ---------------- fonts ---------------- */
void ccfont_release(CCFontId *pid)
{
  CCFontId id = *pid;
  if (id == -1) return;
  assert(ccfonts[id] && ccfonts[id]->refcnt > 0);
  if (--ccfonts[id]->refcnt == 0) {
    if (ccfonts[id]->sf) cairo_scaled_font_destroy(ccfonts[id]->sf);
    free(ccfonts[id]->fontname);
    free(ccfonts[id]);
    ccfonts[id] = NULL;
  }
  *pid = -1;
}

static cairo_scaled_font_t *open_font(const char *desc, int disable_aa)
{
  FcPattern *pat = FcNameParse((const FcChar8 *)desc);
  if (!pat) return NULL;
  FcConfigSubstitute(NULL, pat, FcMatchPattern);
  double dpi;
  if (FcPatternGetDouble(pat, FC_DPI, 0, &dpi) != FcResultMatch)
    FcPatternAddDouble(pat, FC_DPI, CC_DPI);
  FcDefaultSubstitute(pat);
  FcResult res;
  FcPattern *match = FcFontMatch(NULL, pat, &res);
  FcPatternDestroy(pat);
  if (!match) return NULL;

  double px = 12;
  FcPatternGetDouble(match, FC_PIXEL_SIZE, 0, &px);
  FcBool aa = FcTrue;
  FcPatternGetBool(match, FC_ANTIALIAS, 0, &aa);

  cairo_font_face_t *face = cairo_ft_font_face_create_for_pattern(match);
  cairo_matrix_t fm, ctm;
  cairo_matrix_init_scale(&fm, px, px);
  cairo_matrix_init_identity(&ctm);
  cairo_font_options_t *fo = cairo_font_options_create();
  cairo_font_options_set_antialias(fo, (aa && !disable_aa) ? CAIRO_ANTIALIAS_GRAY : CAIRO_ANTIALIAS_NONE);
  cairo_font_options_set_hint_metrics(fo, CAIRO_HINT_METRICS_ON);   /* integer advances, like Xft */
  cairo_scaled_font_t *sf = cairo_scaled_font_create(face, &fm, &ctm, fo);
  cairo_font_options_destroy(fo);
  cairo_font_face_destroy(face);
  FcPatternDestroy(match);
  if (cairo_scaled_font_status(sf) != CAIRO_STATUS_SUCCESS) { cairo_scaled_font_destroy(sf); return NULL; }
  return sf;
}

CCFontId ccfont_get(char *fontdesc_)
{
  char *fontdesc = fontdesc_;
  int ifn = -1, i;
  char buf[600];
  if (Prefs.disable_xft_antialiasing) {
    snprintf(buf, sizeof buf, "%s:antialias=no", fontdesc_);
    fontdesc = buf;
  }
  for (i = 0; i < NB_MAX_CC_FONTS; ++i) {
    if (ccfonts[i] == NULL) { if (ifn == -1) ifn = i; }
    else if (strcmp(fontdesc, ccfonts[i]->fontname) == 0) { ccfonts[i]->refcnt++; return i; }
  }
  if (ifn < 0) { fprintf(stderr, "max fonts exhausted... leak ?\n"); assert(0); }
  CCFont *cf = ccfonts[ifn] = calloc(1, sizeof *cf);
  cf->refcnt = 1;
  cf->fontname = strdup(fontdesc);
  cf->sf = open_font(fontdesc, Prefs.disable_xft_antialiasing);
  if (!cf->sf) {
    fprintf(stderr, "fontcoincoin could not find any font matching '%s'\n", fontdesc);
    ccfont_release(&ifn);
    return ifn;
  }
  cairo_scaled_font_extents(cf->sf, &cf->fe);
  cf->ascent  = (int)ceil(cf->fe.ascent);
  cf->descent = (int)ceil(cf->fe.descent);
  cf->height  = (int)ceil(cf->fe.height);
  return ifn;
}

CCFontId ccfont_incref(CCFontId id)
{ assert(ccfonts[id]); ccfonts[id]->refcnt++; return id; }

/* ---------------- colours ---------------- */
CCColorId cccolor_get(unsigned argb)
{
  int i, ic = -1;
  for (i = 0; i < NB_MAX_CC_COLORS; ++i) {
    if (cccolors[i] == NULL) { if (ic == -1) ic = i; }
    else if (argb == cccolors[i]->argb) { cccolors[i]->refcnt++; return i; }
  }
  if (ic < 0) { fprintf(stderr, "max colors exhausted... leak ?\n"); assert(0); }
  CCColor *cc = cccolors[ic] = calloc(1, sizeof *cc);
  cc->refcnt = 1; cc->argb = argb;
  cc->r = ((argb >> 16) & 0xFF) / 255.0;
  cc->g = ((argb >> 8) & 0xFF) / 255.0;
  cc->b = (argb & 0xFF) / 255.0;
  /* historical quirk: the top byte is *transparency* (0 = opaque) */
  cc->a = 1.0 - ((argb >> 24) & 0xFF) / 255.0;
  cc->pixel = argb & 0xFFFFFF;               /* libxwl pixel format */
  return ic;
}

CCColorId cccolor_get_rgb(int r255, int g255, int b255)
{
  r255 = MAX(0, MIN(255, r255)); g255 = MAX(0, MIN(255, g255)); b255 = MAX(0, MIN(255, b255));
  return cccolor_get(r255 * 65536 + g255 * 256 + b255);
}

CCColorId cccolor_from_name(const char *name)
{
  XColor xc;
  if (XParseColor(display, 0, name, &xc))
    return cccolor_get((((int)xc.red / 256) << 16) + (((int)xc.green / 256) << 8) + ((int)xc.blue / 256));
  return -1;
}

CCColorId cccolor_incref(CCColorId id)
{ assert(cccolors[id]); cccolors[id]->refcnt++; return id; }

void cccolor_release(CCColorId *pid)
{
  CCColorId id = *pid;
  if (id == -1) return;
  assert(cccolors[id] && cccolors[id]->refcnt > 0);
  if (--cccolors[id]->refcnt == 0) { free(cccolors[id]); cccolors[id] = NULL; }
  *pid = -1;
}

void cccolor_reset(CCColorId *pid, unsigned argb)
{ cccolor_release(pid); *pid = cccolor_get(argb); }

unsigned long cccolor_pixel(CCColorId cid) { return cccolors[cid]->pixel; }

/* ---------------- text drawing ---------------- */
static void draw_utf8(CCFontId fid, CCColorId cid, Drawable d, int x, int y, const char *u)
{
  assert(ccfonts[fid] && ccfonts[fid]->sf && cccolors[cid]);
  cairo_surface_t *s = xwl_surface(display, d);
  if (!s) return;
  cairo_t *cr = cairo_create(s);
  CCColor *c = cccolors[cid];
  cairo_set_scaled_font(cr, ccfonts[fid]->sf);
  cairo_set_source_rgba(cr, c->r, c->g, c->b, c->a);
  cairo_move_to(cr, x, y);
  cairo_show_text(cr, u);
  cairo_destroy(cr);
  xwl_damage(display, d);
}

void ccfont_draw_string8(CCFontId fid, CCColorId cid, Drawable d, int x, int y, char *str, int len)
{
  assert(str);
  if (len == -1) len = strlen(str);
  char *tmp = malloc(2 * (size_t)len + 1);
  draw_utf8(fid, cid, d, x, y, latin1_to_utf8(str, len, tmp));
  free(tmp);
}

void ccfont_draw_string_utf8(CCFontId fid, CCColorId cid, Drawable d, int x, int y, char *str, int len)
{
  assert(str);
  if (len == -1) len = strlen(str);
  char *tmp = malloc((size_t)len + 1);
  draw_utf8(fid, cid, d, x, y, slice(str, len, tmp));
  free(tmp);
}

/* ---------------- text metrics ---------------- */
static int extents_utf8(CCFontId fid, const char *u, short *pxstart, short *pxoff)
{
  cairo_text_extents_t te;
  cairo_set_scaled_font(scratch, ccfonts[fid]->sf);
  cairo_text_extents(scratch, u, &te);
  /* Xft convention: x = offset of the origin from the left of the ink box */
  if (pxstart) *pxstart = (short)floor(-te.x_bearing);
  if (pxoff) *pxoff = (short)floor(te.x_advance + 0.5);
  /* whitespace has no ink: Xft then reports the advance (pinnipede relies on width("  ") for word gaps) */
  if (te.width <= 0.0) return (int)floor(te.x_advance + 0.5);
  return (int)ceil(te.width);
}

int ccfont_text_xbox(CCFontId fid, const char *str, int len, short *pxstart, short *pxoff)
{
  if (len == -1) len = strlen(str);
  char *tmp = malloc(2 * (size_t)len + 1);
  int w = extents_utf8(fid, latin1_to_utf8(str, len, tmp), pxstart, pxoff);
  free(tmp); return w;
}

int ccfont_text_xbox_utf8(CCFontId fid, char *str, int len, short *pxstart, short *pxoff)
{
  if (len == -1) len = strlen(str);
  char *tmp = malloc((size_t)len + 1);
  int w = extents_utf8(fid, slice(str, len, tmp), pxstart, pxoff);
  free(tmp); return w;
}

int ccfont_text_width8(CCFontId fid, const char *str, int len) { return ccfont_text_xbox(fid, str, len, NULL, NULL); }

int ccfont_text_width_utf8(CCFontId fid, char *str, int len)
{ return ccfont_text_xbox_utf8(fid, str, len, NULL, NULL); }

/* How many chars of `str` fit in `width` pixels (advance-based). */
int ccfont_text_within_width8(CCFontId fid, const char *str, int len, int width, int *final_width)
{
  int w = 0, i = 0;
  if (width > 0) {
    while (str[i] && (i < len || len == -1)) {
      char one[3]; short off;
      latin1_to_utf8(str + i, 1, one);
      extents_utf8(fid, one, NULL, &off);
      if (w + off <= width) w += off; else break;
      ++i;
    }
  }
  if (final_width) *final_width = w;
  return i;
}

int ccfont_ascent(CCFontId fid)  { return ccfonts[fid]->ascent; }
int ccfont_descent(CCFontId fid) { return ccfonts[fid]->descent; }
int ccfont_height(CCFontId fid)  { return ccfonts[fid]->height; }
