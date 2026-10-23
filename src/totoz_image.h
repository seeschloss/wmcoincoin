/* Totoz pictures, decoded in-process.
 *
 * The pictures are decoded here with stb_image: GIF (animations included),
 * PNG and JPEG.  The file is recognised by its contents, not by its name
 * (totoz are saved as ".gif" whatever the server sent).
 */
#ifndef TOTOZ_IMAGE_H
#define TOTOZ_IMAGE_H

#include <cairo.h>

typedef struct TotozImage {
  int w, h;
  int nframes;
  cairo_surface_t **frames;   /* nframes ARGB32 surfaces of w*h pixels, each one the complete picture */
  int *delay_ms;              /* display time of each frame (meaningful if nframes > 1) */
} TotozImage;

/* Reads just the header: size and mime type ("image/gif", "image/png", "image/jpeg").  0 on success.
   Safe to call from any thread. */
int totoz_image_info(const char *path, int *w, int *h, const char **mime);

/* Decodes the whole picture; NULL if the file is not a supported/valid picture. */
TotozImage *totoz_image_load(const char *path);
void totoz_image_free(TotozImage *img);

#endif
