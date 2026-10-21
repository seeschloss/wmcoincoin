/* Totoz pictures, decoded in-process: see totoz_image.h.
 *
 * The decoding is done by stb_image (public domain, vendored as stb_image.h): JPEG (baseline and
 * progressive), PNG, and GIF including animations.  Everything else (WebP, APNG -which shows its
 * first frame as a PNG-, ...) is not supported.  stb_image is restricted to these three formats to
 * keep it small; it is fed from memory, after the file size and the picture dimensions have been
 * checked, because a file fetched from a web server is not to be trusted.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_GIF
#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_MAX_DIMENSIONS 2048
#if defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
#include "stb_image.h"
#if defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

#include "totoz_image.h"

#define MAX_DIM      2048          /* refuse anything bigger: these are smileys */
#define MAX_PIXELS   (64u * 1024 * 1024 / 4)   /* total over all frames: 64 MB */
#define MAX_FILE     (16u * 1024 * 1024)
#define DEFAULT_DELAY_MS 100

static unsigned char *slurp(const char *path, size_t *len)
{
  FILE *f = fopen(path, "rb");
  unsigned char *buf; long sz;
  if (!f) return NULL;
  if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) <= 0 || (unsigned long)sz > MAX_FILE
      || fseek(f, 0, SEEK_SET) != 0) { fclose(f); return NULL; }
  buf = malloc(sz);
  if (!buf || fread(buf, 1, sz, f) != (size_t)sz) { free(buf); fclose(f); return NULL; }
  fclose(f);
  *len = sz;
  return buf;
}

/* the type is recognised by the contents, never by the file name */
static const char *sniff(const unsigned char *b, size_t n)
{
  if (n >= 6 && (memcmp(b, "GIF87a", 6) == 0 || memcmp(b, "GIF89a", 6) == 0)) return "image/gif";
  if (n >= 8 && memcmp(b, "\x89PNG\r\n\x1a\n", 8) == 0) return "image/png";
  if (n >= 3 && b[0] == 0xFF && b[1] == 0xD8 && b[2] == 0xFF) return "image/jpeg";
  return NULL;
}

/* Counts the pictures of a GIF by walking its blocks, so that the memory an animation needs is known
   before decoding it (stb_image decodes all the frames, each one a full canvas). -1 if malformed. */
static int gif_count_frames(const unsigned char *b, size_t n)
{
  size_t p = 13;
  int frames = 0;
  if (n < 13) return -1;
  if (b[10] & 0x80) p += 3u << ((b[10] & 7) + 1);
  while (p < n) {
    unsigned char t = b[p++];
    if (t == 0x3B) return frames;                       /* trailer */
    if (t == 0x2C) {                                    /* image descriptor */
      if (p + 9 > n) return frames;
      { unsigned char fl = b[p + 8]; p += 9; if (fl & 0x80) p += 3u << ((fl & 7) + 1); }
      p++;                                              /* LZW minimum code size */
      frames++;
    } else if (t == 0x21) p++;                          /* extension: label, then sub-blocks */
    else return -1;
    while (p < n && b[p]) p += 1u + b[p];               /* sub-blocks */
    p++;
    if (frames > 4096) return -1;
  }
  return frames;
}

int totoz_image_info(const char *path, int *w, int *h, const char **mime)
{
  size_t n = 0;
  unsigned char *b = slurp(path, &n);
  int comp, ok;
  if (!b) return -1;
  *mime = sniff(b, n);
  ok = *mime && stbi_info_from_memory(b, (int)n, w, h, &comp)
       && *w > 0 && *h > 0 && *w <= MAX_DIM && *h <= MAX_DIM;
  free(b);
  return ok ? 0 : -1;
}

void totoz_image_free(TotozImage *img)
{
  int i;
  if (!img) return;
  if (img->frames) for (i = 0; i < img->nframes; i++) if (img->frames[i]) cairo_surface_destroy(img->frames[i]);
  free(img->frames); free(img->delay_ms); free(img);
}

/* RGBA (straight alpha, as stb_image gives it) -> cairo ARGB32 (premultiplied) */
static cairo_surface_t *to_surface(const unsigned char *rgba, int w, int h)
{
  cairo_surface_t *s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
  unsigned char *d; int stride, x, y;
  if (cairo_surface_status(s) != CAIRO_STATUS_SUCCESS) { cairo_surface_destroy(s); return NULL; }
  d = cairo_image_surface_get_data(s); stride = cairo_image_surface_get_stride(s);
  cairo_surface_flush(s);
  for (y = 0; y < h; y++) {
    uint32_t *row = (uint32_t *)(d + (size_t)y * stride);
    const unsigned char *p = rgba + (size_t)y * w * 4;
    for (x = 0; x < w; x++, p += 4) {
      unsigned a = p[3];
      row[x] = (a << 24) | (((p[0] * a + 127) / 255) << 16) | (((p[1] * a + 127) / 255) << 8)
               | ((p[2] * a + 127) / 255);
    }
  }
  cairo_surface_mark_dirty(s);
  return s;
}

TotozImage *totoz_image_load(const char *path)
{
  size_t n = 0;
  unsigned char *b = slurp(path, &n), *pix = NULL;
  const char *mime;
  TotozImage *img = NULL;
  int w, h, comp, i, nframes = 1, *delays = NULL;

  if (!b) return NULL;
  mime = sniff(b, n);
  if (!mime || !stbi_info_from_memory(b, (int)n, &w, &h, &comp)
      || w <= 0 || h <= 0 || w > MAX_DIM || h > MAX_DIM) goto done;

  if (strcmp(mime, "image/gif") == 0) {
    int count = gif_count_frames(b, n);
    if (count < 1 || (unsigned)w * (unsigned)h * (unsigned)count > MAX_PIXELS) goto done;
    pix = stbi_load_gif_from_memory(b, (int)n, &delays, &w, &h, &nframes, &comp, 4);
  } else {
    if ((unsigned)w * (unsigned)h > MAX_PIXELS) goto done;
    pix = stbi_load_from_memory(b, (int)n, &w, &h, &comp, 4);
  }
  if (!pix || nframes < 1) goto done;

  img = calloc(1, sizeof *img);
  if (!img) goto done;
  img->w = w; img->h = h; img->nframes = nframes;
  img->frames = calloc(nframes, sizeof *img->frames);
  img->delay_ms = calloc(nframes, sizeof *img->delay_ms);
  if (!img->frames || !img->delay_ms) { totoz_image_free(img); img = NULL; goto done; }
  for (i = 0; i < nframes; i++) {
    img->frames[i] = to_surface(pix + (size_t)i * w * h * 4, w, h);
    if (!img->frames[i]) { totoz_image_free(img); img = NULL; goto done; }
    /* browsers treat 0 / tiny delays as 100ms; do the same, or the animation would spin */
    img->delay_ms[i] = (delays && delays[i] > 10) ? delays[i] : (nframes > 1 ? DEFAULT_DELAY_MS : 1000);
  }
done:
  if (pix) stbi_image_free(pix);
  if (delays) stbi_image_free(delays);
  free(b);
  return img;
}
