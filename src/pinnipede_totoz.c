#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "pinnipede.h"
#include "coincoin.h"
#include <X11/extensions/shape.h>

typedef struct pp_totoz_img {
  int hash;
  int status; /* status: 0 = new, 1 = d/l en cours, 2 = notfound, 3 = found */
  char *name, *mime, *realname;
  int w,h;
} pp_totoz_img;

struct pp_totoz {
  Window win, subwin;
  int survol_hash;
  pid_t animate_pid;

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
  if (pp->totoz->animate_pid) {
    kill(pp->totoz->animate_pid, SIGKILL); /* animate est une grosse bouse pas capable de mourir comme un grand quand 
                                             sa fenetre est détruite. Je suis déçu déçu. C'est pourquoi il se prend 
                                             un gros SIGKILL dans la face. Dis bonjour au ramasse-zombie
                                          */
    pp->totoz->animate_pid = 0;
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
  pp->totoz->animate_pid = 0;
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
pp_totoz_show(Dock *dock, int x_, int y_, int w, int h) {
  Pinnipede *pp = dock->pinnipede;
  XSetWindowAttributes wa;
  int x,y;
  pp_totoz_unmap(dock);

  x = MAX(x_ - w/2, 0);
  y = y_ - h - 20; /* dessus */
  if (y < 0) y = y_ + 20; /* dessous */
  if (y+h > pp->win_height) { /* à gauche ? */
    y = MAX(y_ - h/2, 0);
    x = x_ - w - 20;
    if (x < 0) x = x_ + 20;
    if (x+w > pp->win_width) { x = 0; }
  }

  pp->totoz->win = XCreateSimpleWindow (dock->display, pp->win, /*RootWindow(dock->display,dock->screennum), */
                                       x, y, w+4, h+4, 0,
                                       IRGB2PIXEL(0xffffff), IRGB2PIXEL(0xff0000));
  pp->totoz->subwin = XCreateSimpleWindow (dock->display, pp->totoz->win,
                                           2, 2, w, h, 0,
                                           IRGB2PIXEL(0xffffff), IRGB2PIXEL(0xffffff));
  wa.event_mask =
    ButtonPressMask | 
    ButtonReleaseMask | 
    ExposureMask;
  wa.override_redirect = False /*True*/ ;
  wa.save_under = True;
  XChangeWindowAttributes (dock->display, pp->totoz->win,
			   CWSaveUnder | CWEventMask | CWOverrideRedirect, &wa);
  /*XSetTransientForHint(dock->display, pp->totoz->win, pp->win);
  set_borderless_window_hints(dock->display, pp->totoz->win);
  set_window_class_hint(dock->display, pp->totoz->win, "wmcoincoin", "totoz");
  set_window_pos_hints(dock->display, pp->totoz->win, x, y);
  XSetWindowBorderWidth(dock->display, pp->totoz->win, 0);*/

  
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

  XMapRaised(dock->display, pp->totoz->win);
  XMapWindow(dock->display, pp->totoz->subwin);
  XSync(dock->display, False);
}

int 
pp_totoz_dispatch_event(Dock *dock, XEvent *event)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp->totoz->win != None && event->xany.window == pp->totoz->win) {
    switch (event->type) {
      //    case ButtonPress:
    case ButtonRelease:
      {
	pp_totoz_unmap(dock);
      } break;
    }
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
    pp_totoz_show(dock, x, y, img->w, img->h);
    /* rajouter un check anti-fork bomb */
    switch ((pp->totoz->animate_pid = fork())) {
    case -1: /* arrrrg */
      fprintf(stderr, _("Fork failed...(%s)\n"), strerror(errno)); pp->totoz->animate_pid = 0; break;
    case 0: {
      char *swin = str_printf("0x%08lx",(unsigned long)pp->totoz->subwin);
      char *sfile= str_printf("%s/.wmcoincoin/totoz/%s",getenv("HOME"),img->realname); 
      int ret;
      assert(sfile); assert(swin);
      sfile = shell_quote(sfile); assert(sfile);  /* et une deuxième couche, a priori parfaitement inutile */
      //    int ret = execlp("/usr/bin/animate", "/usr/bin/animate", "-virtual-pixel", "Constant", "-background", "white", "-window", swin, sfile, NULL, NULL);
      /*
        if (use_gifview) {
        BLAHBLAH(0,myprintf("exec gifview +e -a --bg white -w %s %<YEL %s>\n", swin, sfile));
        ret = execlp("/usr/bin/gifview", "/usr/bin/gifview", "+e", "-a", "--bg", "white", "-w", swin, sfile, NULL, NULL);
        } else {
        BLAHBLAH(0,myprintf("exec animate -window %s %<YEL %s>\n", swin, sfile));
        ret = execlp("/usr/bin/animate", "/usr/bin/animate", 
        "-background", "white", "-window", swin, sfile, NULL, NULL);
        }
      */
      BLAHBLAH(1,myprintf("exec wmcoincoin_player %<YEL %s> %s\n", sfile, swin));
      ret = execlp("wmcoincoin_player", "wmcoincoin_player", sfile, swin, NULL, NULL);
      if( ret==-1 ) {
        fprintf(stderr, _("Exec failed...(%s)\n"), strerror(errno));
      }
      exit(ret);
    } break;
    default:
      break;
    }
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

/*void
pp_totoz_download(Dock *dock, unsigned char *imgname) {
  char *realname = pp_totoz_realfname(imgname,0);
  char *cmd = NULL;
  pp_totoz_register_img(dock->pinnipede, imgname, PP_TOTOZ_STATUS_DOWNLOADING);
  //cmd = str_printf("%s/wmcoincoin/wmcc/scripts/wmcoincoin-totoz-get %s&", getenv("HOME"), realname);
  cmd = str_printf("wmcoincoin-totoz-get %s&",realname);
  BLAHBLAH(0, myprintf("Trying to get a new picture [%s] ! executing '%<GRN %s>'\n", realname, cmd));
  system(cmd);
  free(cmd);
  free(realname);
}*/

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
  char *imgurl=NULL, *pathimg=NULL, *pathdesc=NULL, *cmd;
  int is_found=0, i=0, lu;
  char *siteroot;
  if (Prefs.board_sfw_pictures) {
    siteroot = "http://totoz.eu";
  } else {
    siteroot = "http://nsfw.totoz.eu";
  }
  static char *extlist[]={"gif", "png", NULL};
  static char *mimelist[]={"image/gif", "image/png", NULL};
  char buf[1500];
  FILE *f;

  //printf("pp_totoz_get_image(%s) : status = %d\n", imgname, pp_totoz_img_status(pp,imgname));

  char *imgfname = pp_totoz_realfname(imgname,0);  
  pathdesc = str_printf("%s/.wmcoincoin/totoz/%s.desc", getenv("HOME"), imgfname);
  while( !is_found && extlist[i] ) {
    imgurl = str_printf("%s/%s.%s", siteroot, imgfname, extlist[i]);
    wmcc_init_http_request(&r, dock->sites->list->prefs, imgurl);
    /* Triton> Et hop, on supporte le header Accept: image/gif pour les images \o/ */
    r.accept = strdup(mimelist[i]);
    http_request_send(&r);

    //printf("pp_totoz_get_image(%s,%s) : status = %d\n", imgname, extlist[i], pp_totoz_img_status(pp,imgname));
    
    if (http_is_ok(&r)) {
      is_found = 1;
      pathimg  = str_printf("%s/.wmcoincoin/totoz/%s.%s", getenv("HOME"), imgfname, extlist[i]);
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
          cmd = str_printf("echo \"%s.%s\" `wmcoincoin_player -i \"%s\"` \"%s\" > \"%s\"", imgfname, extlist[i], pathimg, mimelist[i], pathdesc);
          if (system(cmd) == -1)
            myfprintf(stderr, "%s failed\n", cmd);
          free(cmd);
          pp_totoz_register_img(dock->pinnipede, imgname, PP_TOTOZ_STATUS_FOUND);
          pp_totoz_update_status(dock, imgname);
        }
      }
      FREE_STRING(pathimg);
      //printf("pp_totoz_get_image(%s,%s) / http_is_ok : status = %d\n", imgname, extlist[i], pp_totoz_img_status(pp,imgname));

    } else pp_totoz_register_img(dock->pinnipede, imgname, PP_TOTOZ_STATUS_NOTFOUND);
    printf("pp_totoz_get_image(%s,%s) / http_is_not_ok : status = %d\n", imgname, extlist[i], pp_totoz_img_status(pp,imgname));
    http_request_close(&r);
    i++;
    free(imgurl);
  }
  if (pp_totoz_img_status(pp, imgname) == PP_TOTOZ_STATUS_NOTFOUND) {
    printf("writing NOTFOUND to %s\n", pathdesc);
    f = fopen(pathdesc, "w"); if (f) { fprintf(f, "NOTFOUND\n"); fclose(f); }
  }
  FREE_STRING(pathdesc);
  free(imgfname);
}
