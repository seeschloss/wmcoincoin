#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "pinnipede.h"
#include "coincoin.h"
#include <X11/extensions/shape.h>
#include <time.h>
#include "xwl.h"
#include "totoz_image.h"

typedef struct pp_totoz_img {
  int hash;
  int status; /* status: 0 = new, 1 = d/l en cours, 2 = notfound, 3 = found */
  char *name, *mime, *realname;
  int w,h;
} pp_totoz_img;

struct pp_totoz {
  Window win;
  int survol_hash;
  TotozImage *image;
  int frame, w, h;
  struct timespec next_frame;

  int nb_img, max_img;
  struct pp_totoz_img *img;
};

volatile int pp_totoz_state_cnt = 0; /* incrementé a chaque changement d'état d'une image */

static pp_totoz_img*
pp_totoz_find_img_(Pinnipede *pp, int hash, int *found, const char *name) {
  int i0, i1;
  *found = 0;
  if (pp->totoz->nb_img == 0) return pp->totoz->img;
  i0 = 0; i1 = pp->totoz->nb_img-1;
  if (hash < pp->totoz->img[i0].hash) return pp->totoz->img+i0;
  if (hash > pp->totoz->img[i1].hash) return pp->totoz->img+i1+1;
  while (hash > pp->totoz->img[i0].hash) {
    int i2 = (i0+i1)/2;
    if (hash < pp->totoz->img[i2].hash) i1 = i2-1; 
    else if (hash > pp->totoz->img[i2].hash) i0 = i2+1;
    else {i0 = i2; break; }
  }

  /* on gere les collisions a la truelle */
  while (i0 > 0 && pp->totoz->img[i0-1].hash == hash) --i0;
  int i;
  for (i = i0; (hash == pp->totoz->img[i].hash) && i < pp->totoz->nb_img; ++i) {
    if (strcmp(name, pp->totoz->img[i].name) == 0) {
      i0 = i; *found = 1;
    }
  }

  i = i0 + (hash <= pp->totoz->img[i0].hash?0:1);
  return pp->totoz->img + i;
}

/*
  status: 0 = new, 1 = d/l en cours, 2 = notfound, 3 = found
*/
pp_totoz_img*
pp_totoz_register_img(Pinnipede *pp, char *imgname, int status) {
  int hash = str_hache(imgname, strlen(imgname));
  pp_totoz_img *img;
  int found;
  img = pp_totoz_find_img_(pp,hash,&found,imgname);
  if (found) {
    /*if (strcmp(imgname, img->name) != 0) {
      BLAHBLAH(0,printf("ouiiiin collision de hache tout pourri: '%s' = '%s' -> "
                        "taper sur l'auteur pour qu'il cherche une vrai fonction de hachage\n", imgname, img->name));
                        }
                        PLUS POSSIOBLE.
    */
    assert(strcmp(imgname, img->name) == 0); 
    assert(img - pp->totoz->img < pp->totoz->nb_img);

    if (img->status != status) pp_totoz_state_cnt++;
    img->status = status;
  } else {
    int imgi = img - pp->totoz->img;
    if (++pp->totoz->nb_img > pp->totoz->max_img) {
      pp->totoz->max_img += 20;
      pp->totoz->img = realloc(pp->totoz->img, pp->totoz->max_img * sizeof(struct pp_totoz_img)); assert(pp->totoz->img);
      img = pp->totoz->img + imgi;
    }
    memmove(img+1,img, (pp->totoz->nb_img-1 - imgi) * sizeof(struct pp_totoz_img));
    img->hash = hash;
    img->status = status;
    img->name = strdup(imgname);
    img->realname = 0;
    img->mime = 0;
    img->w = img->h = 0;
    pp_totoz_state_cnt++;

    BLAHBLAH(1, myprintf("new image registered: '%<YEL %s>'\n", imgname));
    int i, cavachier = 0;
    for (i=0; i < pp->totoz->nb_img; ++i) {
      //printf(" %c %08x %s\n", (i == imgi) ? '!' : ' ', pp->totoz->img[i].hash, pp->totoz->img[i].name);
      if (i != imgi && strcmp(imgname, pp->totoz->img[i].name)==0) {
        cavachier = 1;
      }
    }
    if (cavachier) { printf("ca va chier\n"); assert(0); }
  }
  if (img > pp->totoz->img) assert(img->hash >= (img-1)->hash);
  if (img < pp->totoz->img+pp->totoz->nb_img-1) assert(img->hash <= (img+1)->hash);
  return img;
}

int
pp_totoz_img_status(Pinnipede *pp, char *imgname) {
  int hash = str_hache(imgname, strlen(imgname));
  pp_totoz_img *img;
  int found;
  img = pp_totoz_find_img_(pp,hash,&found, imgname);

  //printf("pp_totoz_img_status '%s' : %d %d/%d\n",imgname, found, img - pp->totoz->img, pp->totoz->nb_img);
  if (found) { return img->status; }
  else { pp_totoz_register_img(pp, imgname, PP_TOTOZ_STATUS_UNKNOWN); return PP_TOTOZ_STATUS_UNKNOWN; }
}

void
pp_totoz_unmap(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  if (pp->totoz->win != None) {
    XDestroyWindow(dock->display, pp->totoz->win);
    pp->totoz->win = None;
  }
  if (pp->totoz->image) {
    totoz_image_free(pp->totoz->image);
    pp->totoz->image = NULL;
  }
}

void
pp_totoz_build(Dock *dock) {
  ALLOC_OBJ(dock->pinnipede->totoz, struct pp_totoz);
  dock->pinnipede->totoz->img = 0;
  pp_totoz_rebuild(dock);
}

void
pp_totoz_rebuild(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  assert(pp->totoz);
  pp_totoz_unmap(dock);
  pp->totoz->win = None;
  pp->totoz->survol_hash = -42;
  pp->totoz->image = NULL;
  if (pp->totoz->img) {
    int i;
    for (i = 0; i < pp->totoz->nb_img; ++i) FREE_STRING(pp->totoz->img[i].name);
    free(pp->totoz->img);
  }
  pp->totoz->nb_img = 0;
  pp->totoz->max_img = 20;
  ALLOC_VEC(pp->totoz->img, pp->totoz->max_img, struct pp_totoz_img);
}

void
pp_totoz_destroy(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  pp_totoz_unmap(dock);
  if (pp->totoz) {
    free(pp->totoz); pp->totoz = NULL;
  }
}

static void
pp_totoz_paint(Dock *dock) {
  struct pp_totoz *t = dock->pinnipede->totoz;
  cairo_surface_t *dst;
  cairo_t *cr;
  if (t->win == None || !t->image) return;
  dst = xwl_surface(dock->display, t->win);
  if (!dst) return;
  cr = cairo_create(dst);
  cairo_set_source_rgb(cr, 1, 0, 0);
  cairo_paint(cr);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_rectangle(cr, 2, 2, t->w, t->h);
  cairo_fill(cr);
  cairo_set_source_surface(cr, t->image->frames[t->frame], 2, 2);
  cairo_paint(cr);
  cairo_destroy(cr);
  xwl_damage(dock->display, t->win);
}

static void
pp_totoz_schedule_next(struct pp_totoz *t) {
  int d = t->image->delay_ms[t->frame];
  clock_gettime(CLOCK_MONOTONIC, &t->next_frame);
  t->next_frame.tv_nsec += (long)(d % 1000) * 1000000L;
  t->next_frame.tv_sec += d / 1000;
  if (t->next_frame.tv_nsec >= 1000000000L) { t->next_frame.tv_nsec -= 1000000000L; t->next_frame.tv_sec++; }
}

static void
pp_totoz_show(Dock *dock, int x_, int y_, TotozImage *image) {
  Pinnipede *pp = dock->pinnipede;
  XSetWindowAttributes wa;
  int x,y, w = image->w, h = image->h;
  pp_totoz_unmap(dock);
  pp->totoz->image = image;
  pp->totoz->frame = 0; pp->totoz->w = w; pp->totoz->h = h;

  x = MAX(x_ - w/2, 0);
  y = y_ - h - 20; /* dessus */
  if (y < 0) y = y_ + 20; /* dessous */
  if (y+h > pp->win_height) { /* a gauche ? */
    y = MAX(y_ - h/2, 0);
    x = x_ - w - 20;
    if (x < 0) x = x_ + 20;
    if (x+w > pp->win_width) { x = 0; }
  }

  pp->totoz->win = XCreateSimpleWindow (dock->display, pp->win, x, y, w+4, h+4, 0,
                                        IRGB2PIXEL(0xffffff), IRGB2PIXEL(0xff0000));
  wa.event_mask = ExposureMask;
  wa.override_redirect = True;
  XChangeWindowAttributes (dock->display, pp->totoz->win, CWEventMask | CWOverrideRedirect, &wa);

  {
    Pixmap shape_mask = XCreatePixmap(dock->display, dock->rootwin, w+4, h+4, 1);
    GC monoGC = XCreateGC(dock->display, shape_mask, 0, NULL);
    XSetForeground(dock->display, monoGC, 0); 
    XFillRectangle(dock->display, shape_mask, monoGC, 0, 0, w+4, h+4);
    XSetForeground(dock->display, monoGC, 1); 
    XFillRectangle(dock->display, shape_mask, monoGC, 2, 2, w, h);
    XDrawLine(dock->display, shape_mask, monoGC, w, 0, w+3, 0);
    XDrawLine(dock->display, shape_mask, monoGC, w+3, 0, w+3, 4);
    XDrawLine(dock->display, shape_mask, monoGC, 0, 0, 4, 0);
    XDrawLine(dock->display, shape_mask, monoGC, 0, 0, 0, 4);
    XDrawLine(dock->display, shape_mask, monoGC, 0, h, 0, h+3);
    XDrawLine(dock->display, shape_mask, monoGC, 0, h+3, 4, h+3);
    XDrawLine(dock->display, shape_mask, monoGC, w+3, h, w+3, h+3);
    XDrawLine(dock->display, shape_mask, monoGC, w, h+3, w+3, h+3);
    XShapeCombineMask(dock->display, pp->totoz->win, ShapeBounding, 0, 0, shape_mask,
                      ShapeSet);
    XFreePixmap(dock->display, shape_mask);
    XFreeGC(dock->display, monoGC);
  }

  pp_totoz_paint(dock);
  XMapRaised(dock->display, pp->totoz->win);
  if (image->nframes > 1) pp_totoz_schedule_next(pp->totoz);
  XSync(dock->display, False);
}

void
pp_totoz_tick(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  struct pp_totoz *t = pp ? pp->totoz : NULL;
  struct timespec now;
  int guard = 0;
  if (!t || t->win == None || !t->image || t->image->nframes < 2) return;
  clock_gettime(CLOCK_MONOTONIC, &now);
  if (now.tv_sec < t->next_frame.tv_sec ||
      (now.tv_sec == t->next_frame.tv_sec && now.tv_nsec < t->next_frame.tv_nsec)) return;
  t->frame = (t->frame + 1) % t->image->nframes;
  pp_totoz_paint(dock);
  pp_totoz_schedule_next(t);
  (void)guard;
}

int 
pp_totoz_dispatch_event(Dock *dock, XEvent *event)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp->totoz->win != None && event->xany.window == pp->totoz->win) {
    if (event->type == Expose) pp_totoz_paint(dock);
    return 1;
  }
  return 0;
}

/*
  entree: "[:totoz plop]"
  sortie: "totoz%20plop"
*/
char *
pp_totoz_realfname(unsigned char *name, int with_path) {
  int i;
  unsigned char *realname;
  unsigned char *s = strdup(name);
  assert(s);
  assert(s[0] == '[' && s[1] == ':' && s[strlen(name)-1] == ']');
  s[strlen(s)-1] = 0;
  for (i=0; s[i]; ++i) if (s[i] < ' ') s[i] = ' ';
  realname = http_url_encode(s+2,0);  assert(realname);
  free(s);
  if (with_path) {
    s = realname;
    realname = str_printf("%s/.wmcoincoin/totoz/%s.desc", getenv("HOME"), s); free(s);
  }
  return realname;
}

static pp_totoz_img*
pp_totoz_update_status(Dock *dock, char *imgname) {
  Pinnipede *pp = dock->pinnipede;
  int status,tw,th;
  char s[512],  mimetype[100], realname[512];
  pp_totoz_img *img = NULL;

  
  if (pp_totoz_img_status(pp, imgname) == PP_TOTOZ_STATUS_DOWNLOADING) {
    img = pp_totoz_register_img(pp,imgname,PP_TOTOZ_STATUS_DOWNLOADING);
  }
  else {
    char *fname = pp_totoz_realfname(imgname, 1);
    FILE *f = fopen(fname, "rt");
        
    /* record de concentration de gotos ! */
    if (f == NULL) { 
      status = pp_totoz_img_status(pp, imgname); //PP_TOTOZ_STATUS_UNKNOWN;
    } else if (fgets(s, 512,f) == NULL) { 
      printf("pp_totoz_update_status: uuurg?\n");
      status = PP_TOTOZ_STATUS_NOTFOUND;
    } else if (strncmp(s, "NOTFOUND", 8) == 0) {
      status = PP_TOTOZ_STATUS_NOTFOUND;
    } else if (sscanf(s, " %511s %dx%d %99s", realname, &tw, &th, mimetype) != 4) {
      printf("pp_totoz_update_status: %s %s grunt?\n", imgname, realname);
      status = PP_TOTOZ_STATUS_NOTFOUND;    
    } else {
      status = PP_TOTOZ_STATUS_FOUND;
    }
    if (f) fclose(f);
    img = pp_totoz_register_img(pp,imgname,status);
    if (status == PP_TOTOZ_STATUS_FOUND) {
      img->w = tw; img->h = th;
      ASSIGN_STRING_VAL(img->mime, mimetype);
      ASSIGN_STRING_VAL(img->realname, realname);
    }
    free(fname);
  }
  return img;
}

static void
pp_totoz_update_status_all(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  int i;
  for (i = 0; i < pp->totoz->nb_img; ++i) {
    /*if (pp->totoz->img[i].status == PP_TOTOZ_STATUS_DOWNLOADING) {
      //pp_totoz_update_status(dock, pp->totoz->img[i].name);
      cnt++;
    } else */
    if (Prefs.board_auto_dl_pictures && pp->totoz->img[i].status == PP_TOTOZ_STATUS_UNKNOWN) {
      pp_totoz_update_status(dock, pp->totoz->img[i].name);
      if (pp->totoz->img[i].status == PP_TOTOZ_STATUS_UNKNOWN) pp_totoz_download(dock, pp->totoz->img[i].name);
    }
  }
}

void
pp_totoz_check_updates(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  if (pp_ismapped(dock)) {
    static int saved_pp_totoz_state_cnt = 0;
    pp_totoz_update_status_all(dock);
    //printf("pp_totoz_check_updates : pp_totoz_state_cnt = %d\n", pp_totoz_state_cnt);
    if (pp_totoz_state_cnt != saved_pp_totoz_state_cnt) {
      //printf("pp_totoz_check_updates : pp_totoz_state_cnt = %d - > pp_update_request\n", pp_totoz_state_cnt);
      //pp->flag_pp_update_request = 1;
      pp_pv_destroy(pp); 
      pp_update_content(dock, pp->id_base, pp->decal_base,0,0);
      pp_refresh(dock, pp->win, NULL);
    }
    saved_pp_totoz_state_cnt = pp_totoz_state_cnt;
  }
}

static void
pp_totoz_request(Dock *dock, char *imgname, int x, int y) {
  Pinnipede *pp = dock->pinnipede;
  char *urlencod_img = pp_totoz_realfname(imgname, 0);
  char info[1024];

  int old_status = pp_totoz_img_status(pp, imgname);
  pp_totoz_img *img = pp_totoz_update_status(dock,imgname); assert(img);  
  switch (img->status) {
  case PP_TOTOZ_STATUS_UNKNOWN: 
    snprintf(info,1024, "This picture [%s] has not been downloaded, use the popup menu (right clic) "
             "to get it (and wait a little..)", urlencod_img);
    break;
  case PP_TOTOZ_STATUS_DOWNLOADING:
    snprintf(info,1024, "The marmotte is downloading the picture [%s], be patient", urlencod_img);
    break;
  case PP_TOTOZ_STATUS_NOTFOUND:
    snprintf(info,1024, "desolai but according to [~/.wmcoincoin/totoz/%s.desc] this picture does "
             "not exist (like the cabale)", urlencod_img);
    break;
  case PP_TOTOZ_STATUS_FOUND:
    snprintf(info,1024, "%s [%s], size %dx%d", img->realname, img->mime, img->w,img->h);
    break;
  default:
    snprintf(info,1024, "%s BUUUUUUUUUUUUGGG !!!", urlencod_img);
    break;
  }
  if (old_status != img->status) { /* pour permettre le changement d'apparence */
    pp_pv_destroy(pp); 
    pp_update_content(dock, pp->id_base, pp->decal_base,0,0);
    pp_refresh(dock, pp->win, NULL);
  }
  pp_popup_show_txt(dock, info);
  if (img->status == PP_TOTOZ_STATUS_FOUND) {
    char *sfile = str_printf("%s/.wmcoincoin/totoz/%s", getenv("HOME"), img->realname);
    TotozImage *pic = totoz_image_load(sfile);
    if (pic) pp_totoz_show(dock, x, y, pic);
    else {
      snprintf(info, 1024, "%s [%s]: cannot decode this picture",
               img->realname, img->mime);
      pp_popup_show_txt(dock, info);
    }
    free(sfile);
  }
  free(urlencod_img);
}

void
pp_check_totoz(Dock *dock, PostWord *pw, int x_root, int y_root) {
  Pinnipede *pp = dock->pinnipede;
  int survol_hash = 0;
  if (pw) survol_hash = str_hache(pw->w,50);
  /*if (pw && (pw->attr & PWATTR_TOTOZ))
    printf("%s : %d %d %08lx\n", pw->w, pp->totoz->survol_hash, pp->survol_hash, pp->totoz->win);*/
  if (pp->totoz->survol_hash != survol_hash) {
    if (pp->totoz->win != None) {
      pp_totoz_unmap(dock); pp->totoz->survol_hash = 0;
    }
    if (pw && (pw->attr & PWATTR_TOTOZ)) {
      pp_totoz_request(dock, pw->w, x_root, y_root);
    }
  }
  pp->totoz->survol_hash = survol_hash;
}

void
pp_totoz_download(Dock *dock, unsigned char *imgname) {  
  pp_totoz_register_img(dock->pinnipede, imgname, PP_TOTOZ_STATUS_DOWNLOADING);
  ccqueue_push_smiley_dl(imgname);
  BLAHBLAH(0, myprintf("Download of picture %<MAG %s> queued!\n", imgname));
}

/* appele depuis ccqueue_loop */
void
pp_totoz_get_image(Dock *dock, unsigned char *imgname) {
  Pinnipede *pp = dock->pinnipede;
  HttpRequest r;
  char *imgurl=NULL, *pathimg=NULL, *pathdesc=NULL;
  int is_found=0, i=0, lu;
  char *siteroot;
  siteroot = strdup(Prefs.board_totoz_server);
  char buf[1500];
  FILE *f;

  //printf("pp_totoz_get_image(%s) : status = %d\n", imgname, pp_totoz_img_status(pp,imgname));

  char *imgfname = pp_totoz_realfname(imgname,0);  
  pathdesc = str_printf("%s/.wmcoincoin/totoz/%s.desc", getenv("HOME"), imgfname);

  if (strstr(siteroot, "%s") != NULL) {
    imgurl = str_printf(siteroot, imgfname);
  } else {
    imgurl = str_printf("%s/%s.gif", siteroot, imgfname);
  }

  wmcc_init_http_request(&r, dock->sites->list->prefs, imgurl);
  /* Triton> Et hop, on supporte le header Accept: image/gif pour les images \o/ */
  r.accept = strdup("image/gif, image/png, image/jpeg");
  http_request_send(&r);

  //printf("pp_totoz_get_image(%s,%s) : status = %d\n", imgname, extlist[i], pp_totoz_img_status(pp,imgname));
  
  if (http_is_ok(&r)) {
    is_found = 1;
    pathimg  = str_printf("%s/.wmcoincoin/totoz/%s.gif", getenv("HOME"), imgfname);
    wmcc_log_http_request(dock->sites->list, &r);
    
    f = fopen(pathimg, "w");
    if (f == NULL) {
      char *ptotoz = str_printf("%s/.wmcoincoin/totoz", getenv("HOME")); mkdir(ptotoz, -1);
      f = fopen(pathimg, "w");
    }
    if (f == NULL) {
      myfprintf(stderr, "pp_totoz_get_image : Impossible d'ouvrir/creer %s : %s\n", pathimg, strerror(errno));
      pp_totoz_register_img(dock->pinnipede, imgname, PP_TOTOZ_STATUS_NOTFOUND);
    } else {
      fwrite(r.response_data, r.response_size, 1, f);
      if ( ferror(f) ) {
        fclose(f);
        myfprintf(stderr, "Erreur lors du telechargement du fichier %s\n", pathimg);
        pp_totoz_register_img(dock->pinnipede, imgname, PP_TOTOZ_STATUS_NOTFOUND);
      } else {
        fclose(f);
        /* write the description file: "<file> <w>x<h> <mime>" */
        int tw, th; const char *tmime;
        if (totoz_image_info(pathimg, &tw, &th, &tmime) != 0) {
          myfprintf(stderr, "pp_totoz_get_image : %s is not a GIF, PNG or JPEG picture\n", pathimg);
          pp_totoz_register_img(dock->pinnipede, imgname, PP_TOTOZ_STATUS_NOTFOUND);
        } else {
          FILE *fd = fopen(pathdesc, "w");
          if (fd) { fprintf(fd, "%s.gif %dx%d %s\n", imgfname, tw, th, tmime); fclose(fd); }
          pp_totoz_register_img(dock->pinnipede, imgname, PP_TOTOZ_STATUS_FOUND);
          pp_totoz_update_status(dock, imgname);
        }
      }
    }
    FREE_STRING(pathimg);
    //printf("pp_totoz_get_image(%s,%s) / http_is_ok : status = %d\n", imgname, extlist[i], pp_totoz_img_status(pp,imgname));

  } else pp_totoz_register_img(dock->pinnipede, imgname, PP_TOTOZ_STATUS_NOTFOUND);
  BLAHBLAH(1, printf("pp_totoz_get_image(%s) : status = %d\n", imgname, pp_totoz_img_status(pp,imgname)));
  http_request_close(&r);
  i++;
  free(imgurl);

  if (pp_totoz_img_status(pp, imgname) == PP_TOTOZ_STATUS_NOTFOUND) {
    printf("writing NOTFOUND to %s\n", pathdesc);
    f = fopen(pathdesc, "w"); if (f) { fprintf(f, "NOTFOUND\n"); fclose(f); }
  }
  FREE_STRING(pathdesc);
  free(imgfname);
}
