#include "coincoin.h"
#include "coin_xutil.h"
#include "board_util.h"
#include "site.h"
#include "balltrap.h"
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <math.h>


#include "../xpms/duck.xpm"

#define DUCK_NCX 4
struct _Duck {
  id_type id;
  float x,y;
  float angle_ampli[DUCK_NCX], angle_phase[DUCK_NCX];
  float speed_ampli[DUCK_NCX], speed_phase[DUCK_NCX];
  int step, age, xiscreen;
  Window win;
  int shot;
  struct _Duck *next;
};

#define NB_DUCK_XPM_IMG 9
#define NB_DUCK_IMG (NB_DUCK_XPM_IMG*2)

struct DuckImgs {
  Pixmap masks[NB_DUCK_IMG];
  Pixmap pix;
} duck_img;

double wmcc_drand() {
  static int isinit = 0;
  if (!isinit) { srand((unsigned)time(NULL)); isinit = 1; }
  return rand()/(RAND_MAX+1.);
}

int wmcc_rand(int sup) {
  return (int)(sup*wmcc_drand());
}


void
balltrap_build(Dock *dock) {
  RGBAImage *rsrc,*rdst;
  int i,j,k;
  dock->nb_duck = 0; dock->duck_lst = NULL;
  rsrc = RGBACreateRImgFromXpmData(dock->rgba_context, duck_xpm);
  assert(rsrc->h == 32);
  rdst = RGBACreateImage(rsrc->w*2, rsrc->h);  
  for (j = 0; j < 32; ++j)
    for (k = 0; k < 32*NB_DUCK_XPM_IMG; ++k) {
      rdst->data[j][k] = rsrc->data[j][k];
      rdst->data[j][32*NB_DUCK_IMG - k - 1] = rsrc->data[j][k];
    }
  for (i = 0; i < NB_DUCK_IMG; ++i) {
    char xbm[32][32/8];
    memset(xbm, 0, sizeof xbm);
    for (j = 0; j < 32; ++j) 
      for (k = 0; k < 32; ++k) 
        if (rdst->data[j][k + i*32].v)
          xbm[j][k/8] |= (1<<(k % 8));
    duck_img.masks[i] = XCreateBitmapFromData(dock->display, dock->win, 
                                              (char*)xbm, 32, 32);
  }
  duck_img.pix = RGBAImage2Pixmap(dock->rgba_context, rdst);
  RGBADestroyImage(rsrc);
  RGBADestroyImage(rdst);
}

static int duck_find_screen(Dock *dock, float x, float y) {
  int i, ix=(int)x, iy=(int)y;
  for (i=0; i < dock->nb_xiscreen; ++i) {
    if (ix >= dock->xiscreen[i].x_org && iy >= dock->xiscreen[i].y_org &&
        ix <= dock->xiscreen[i].x_org + dock->xiscreen[i].width -20 && 
        iy <= dock->xiscreen[i].y_org + dock->xiscreen[i].height-20) {
      return i;
    }
  }
  return 0;
}

void
balltrap_add(Dock *dock, id_type id) {  
  if (dock->nb_duck < Prefs.hunt_max_duck) {
    Duck *d;
    XSetWindowAttributes wa;
    int i;

    ALLOC_OBJ(d,Duck);
    dock->nb_duck++;
    d->next = dock->duck_lst; dock->duck_lst = d;
    d->id = id;
    d->step = 0;
    d->age = 0;
    d->shot = 0;
    for (i=0; i < DUCK_NCX; ++i) { 
      d->speed_phase[i] = wmcc_drand()*M_PI*2;
      d->angle_phase[i] = wmcc_drand()*M_PI*2;
      d->speed_ampli[i] = wmcc_drand()*(DUCK_NCX-i);
      d->angle_ampli[i] = wmcc_drand()/1.6;
    }
    int dx,dy;
    get_window_pos_with_decor(dock->display, DOCK_WIN(dock), &dx, &dy);
    d->x = dx - 20 + wmcc_rand(40); d->y = dy - 20 + wmcc_rand(40);
    d->xiscreen = duck_find_screen(dock, dx, dy);
    d->win = XCreateSimpleWindow(dock->display, dock->rootwin, 0, 0, 32, 32, 0,
                                 cccolor_pixel(dock->white_color),
                                 cccolor_pixel(dock->black_color));
    wa.event_mask = ButtonPressMask | ButtonReleaseMask;
    wa.override_redirect = True;
    wa.save_under = True;
    wa.cursor = XCreateFontCursor(dock->display, XC_target); //XC_cross);
    XChangeWindowAttributes (dock->display, d->win,
                             CWCursor | CWSaveUnder | CWEventMask | CWOverrideRedirect, &wa);
    XMapRaised(dock->display, d->win);
  }
}

static void
balltrap_remove(Dock *dock, Duck *d) {
  if (d == dock->duck_lst) {
    dock->duck_lst = d->next;
  } else {
    Duck *dd;
    for (dd = dock->duck_lst; dd->next != d; dd = dd->next) 
      assert(dd); 
    dd->next = d->next;
  }
  dock->nb_duck--;
  XDestroyWindow(dock->display, d->win);
  free(d);
}

enum {DUCK_GOING_LEFT0=0, DUCK_GOING_LEFT1=7,
      DUCK_GOING_RIGHT0=8, DUCK_GOING_RIGHT1=15,
      DUCK_TURNS_LHR0=16, DUCK_TURNS_LHR1=23,
      DUCK_TURNS_RHL0=24, DUCK_TURNS_RHL1=31,
      DUCK_DIES_LEFT0=32,DUCK_DIES_LEFT1=54,
      DUCK_DIES_RIGHT0=55,DUCK_DIES_RIGHT1=77};

enum {DUCKIMG_GOING_LEFT0=0, DUCKIMG_GOING_LEFT1=3,
      DUCKIMG_TURNS_LHR0=4, DUCKIMG_TURNS_LHR1=7,
      DUCKIMG_DIES_LEFT=8,
      DUCKIMG_DIES_RIGHT=9,
      DUCKIMG_TURNS_RHL0=10, DUCKIMG_TURNS_RHL1=13,
      DUCKIMG_GOING_RIGHT0=14, DUCKIMG_GOING_RIGHT1=17};


static int duck_dies_right(Duck *d) {
  return (d->step >= DUCK_DIES_RIGHT0 && d->step <= DUCK_DIES_RIGHT1);
}
static int duck_dies_left(Duck *d) { 
  return (d->step >= DUCK_DIES_LEFT0 && d->step <= DUCK_DIES_LEFT1);
}
static int duck_is_dead(Duck *d) {
  return duck_dies_left(d) || duck_dies_right(d);
}
static int duck_going_left(Duck *d) { 
  return (d->step >= DUCK_GOING_LEFT0 && d->step <= DUCK_GOING_LEFT1);
}
static int duck_going_right(Duck *d) { 
  return (d->step >= DUCK_GOING_RIGHT0 && d->step <= DUCK_GOING_RIGHT1);
}
static int duck_turns_lhr(Duck *d) { 
  return (d->step >= DUCK_TURNS_LHR0 && d->step <= DUCK_TURNS_LHR1);
}
static int duck_turns_rhl(Duck *d) { 
  return (d->step >= DUCK_TURNS_RHL0 && d->step <= DUCK_TURNS_RHL1);
}
static int duck_in_translation(Duck *d) { return duck_going_left(d) || duck_going_right(d); }
static int duck_in_rotation(Duck *d) { return duck_turns_rhl(d) || duck_turns_lhr(d); }

static int duck_current_img(Duck *d) {
  if (duck_going_left(d)) {
    return DUCKIMG_GOING_LEFT0 + 
      ((d->step - DUCK_GOING_LEFT0) * (DUCKIMG_GOING_LEFT1 - DUCKIMG_GOING_LEFT0 + 1))/(DUCK_GOING_LEFT1-DUCK_GOING_LEFT0 + 1);
  } else if (duck_going_right(d)) {
    return DUCKIMG_GOING_RIGHT0 + 
      ((d->step - DUCK_GOING_RIGHT0) * (DUCKIMG_GOING_RIGHT1 - DUCKIMG_GOING_RIGHT0 + 1))/(DUCK_GOING_RIGHT1-DUCK_GOING_RIGHT0 + 1);
  } else if (duck_turns_lhr(d)) {
    return DUCKIMG_TURNS_LHR0 + 
      ((d->step - DUCK_TURNS_LHR0) * (DUCKIMG_TURNS_LHR1 - DUCKIMG_TURNS_LHR0 + 1))/(DUCK_TURNS_LHR1-DUCK_TURNS_LHR0 + 1);
  } else if (duck_turns_rhl(d)) {
    return DUCKIMG_TURNS_RHL0 + 
      ((d->step - DUCK_TURNS_RHL0) * (DUCKIMG_TURNS_RHL1 - DUCKIMG_TURNS_RHL0 + 1))/(DUCK_TURNS_RHL1-DUCK_TURNS_RHL0 + 1);
  } else if (duck_is_dead(d)) {
    return (d->step >= DUCK_DIES_LEFT0 && d->step <= DUCK_DIES_LEFT1) ?
            DUCKIMG_DIES_LEFT : DUCKIMG_DIES_RIGHT;
  } else assert(0);
  return 1<<30;
}
/*static int duck_going_left(Duck *d) { return (d->step >= 0 && d->step <= 3); }
static int duck_going_right(Duck *d) { return (d->step >= 14 && d->step <= 17); }
static int duck_turns_lhr(Duck *d) { return (d->step >= 4 && d->step <= 7); }
static int duck_turns_rhl(Duck *d) { return (d->step >= 10 && d->step <= 13); }
*/

static int duck_repousse(Dock *dock, Duck *d, float x, float y, float *pvx, float *pvy) {
  int i, ix=(int)x, iy=(int)y;
  //for (i=0; i < dock->nb_xiscreen; ++i) {
  i = d->xiscreen;
    if (ix <= dock->xiscreen[i].x_org + 20) { *pvx += 1; return duck_going_left(d); }
    if (ix <= dock->xiscreen[i].x_org + 10) { *pvx += 3; return duck_going_left(d); }
    if (ix >= dock->xiscreen[i].x_org + dock->xiscreen[i].width - 20) { *pvx -= 1; return duck_going_right(d); }
    if (ix >= dock->xiscreen[i].x_org + dock->xiscreen[i].width - 10) { *pvx -= 3; return duck_going_right(d); }
    if (iy <= dock->xiscreen[i].y_org + 20) *pvy += 1;
    if (iy <= dock->xiscreen[i].y_org + 10) *pvy += 3;
    if (iy >= dock->xiscreen[i].y_org + dock->xiscreen[i].height - 20) *pvy -= 1;
    if (iy >= dock->xiscreen[i].y_org + dock->xiscreen[i].height - 10) *pvy -= 3;
    //}
  return 0;
}

void balltrap_animate(Dock *dock) {
  Duck *d, *d_next;
  static float t = 0.;
  static float dt = 0.1;
  t += dt;
  for (d = dock->duck_lst; d; d = d_next) {
    int oldimg = duck_current_img(d);
    int destroy_it = 0;
    d_next = d->next;
    d->age++;
    if (!duck_is_dead(d)) {
      float angle = 0, speed = 3 /* vitess mini */, vx, vy;
      int k;
      for (k=0; k < DUCK_NCX; ++k) {
        angle += d->angle_ampli[k]*sin(d->angle_phase[k]+t*k);
        speed += d->speed_ampli[k]*(1.+sin(d->speed_phase[k]+t*(k+1)));
      }
      if (duck_going_left(d)) {
        speed = -speed;
      }
      if (duck_in_translation(d)) {
        vx = speed*cos(angle);
        vy = speed*sin(angle);
      }
      int tourne = 0;
      if (!duck_in_rotation(d)) {
        tourne = (wmcc_rand(200) == 0) ? 100 : 0;
        if (duck_repousse(dock, d, d->x, d->y, &vx, &vy)) tourne = 1;
      }
      //printf("angle = %f, speed = %+f, tourne = %d, vx=%+f, vy=%+f, step=%d", angle,speed,tourne,vx,vy,d->step);
      if (duck_going_left(d)) {
        d->x += vx; d->y += vy;
        if (!tourne) {
          if (++d->step > DUCK_GOING_LEFT1) d->step = DUCK_GOING_LEFT0;
        } else d->step = DUCK_TURNS_LHR0;
      } else if (duck_going_right(d)) {
        d->x += vx; d->y += vy;
        if (!tourne) {
          if (++d->step > DUCK_GOING_RIGHT1) d->step = DUCK_GOING_RIGHT0;
        } else d->step = DUCK_TURNS_RHL1;
      } else if (duck_turns_lhr(d)) {
        if (++d->step > DUCK_TURNS_LHR1) d->step = DUCK_GOING_RIGHT1;
      } else if (duck_turns_rhl(d)) {
        if (--d->step < DUCK_TURNS_RHL0) d->step = DUCK_GOING_LEFT0;
      }
      assert(!duck_is_dead(d));
    } else {
      if (d->shot) {
        d->shot = 0;
        board_msg_info *mi = boards_find_id(dock->sites->boards, d->id);
        if (mi && mi->is_my_message) {
          myprintf("proutch ... you killed yourself\n");
        } else if (mi && Prefs.site[id_type_sid(d->id)]->post_url) {
          char s[200], s_subts[3];
          Site *site = sl_find_site_id(dock->sites, id_type_sid(d->id)); assert(site);
          snprintf(s, sizeof s, "[%s]", Prefs.site[id_type_sid(d->id)]->site_name);
          pp_set_download_info(s, "PAN ! PAN !");
          s_subts[0] = s_subts[1] = s_subts[2] = 0;
          switch(mi->sub_timestamp) {
            case -1: break;
            case 0: s_subts[0] = '¹'; break;
            case 1: s_subts[0] = '²'; break;
            case 2: s_subts[0] = '³'; break;
            default: s_subts[0] = ':'; s_subts[1] = '1' + mi->sub_timestamp;
          }
          snprintf(s, sizeof s, "%02d:%02d:%02d%s %s", mi->hmsf[0], mi->hmsf[1], mi->hmsf[2], s_subts,
                   wmcc_rand(15) != 13 ? "pan ! pan !" : "cliclic (fusil enrayé)");;
          BLAHBLAH(1,myprintf("BALLTRAP : [%s] %s\n", Prefs.site[id_type_sid(d->id)]->site_name, s));
          ccqueue_push_board_post(site->site_id,  
                                  site->board->coin_coin_useragent, s);
        } else {
          myprintf("%<BLU clic ! clic !> you shot a plastic duck\n");
        }
      }
      if (duck_dies_left(d)) { d->x -= 1; d->step++; if (!duck_dies_left(d)) destroy_it = 1; }
      else { d->step++; if (!duck_dies_right(d)) destroy_it = 1; d->x += 1; }
      d->y += 3;      
    }
    if (destroy_it || d->age > 20000) {
      balltrap_remove(dock, d);
    } else {
      int newimg = duck_current_img(d);
      //printf("  --> x=%+f y=%+f, step=%d oldim=%d img=%d\n", d->x, d->y,d->step,oldimg, newimg);
      //XUnmapWindow(dock->display,d->win);
      XMoveWindow(dock->display, d->win, d->x, d->y);    
      //XMapRaised(dock->display,d->win);
      if (newimg != oldimg || d->age == 1 || d->step == DUCK_DIES_LEFT0+1 || d->step == DUCK_DIES_RIGHT0+1) {
        XShapeCombineMask(dock->display, d->win, ShapeBounding, 0, 0, duck_img.masks[newimg],
                          ShapeSet);
        XCopyArea(dock->display, duck_img.pix, d->win, dock->NormalGC, newimg * 32, 0, 32, 32, 0, 0);
      }
    }
  }
  //if ((++cnt % 10) == 0) XFlush(dock->display);
}

void balltrap_airstrike(Dock *dock) {
  Duck *d;
  for (d = dock->duck_lst; d; d = d->next) 
    if (!duck_is_dead(d)) {
      if (duck_going_left(d) || duck_turns_rhl(d)) {
        d->step = DUCK_DIES_LEFT0;
      } else d->step = DUCK_DIES_RIGHT0;
    }
}

void balltrap_armageddon(Dock *dock) {
  while (dock->duck_lst) balltrap_remove(dock, dock->duck_lst);
}

int
balltrap_dispatch_event(Dock *dock, XEvent *ev) {
  switch (ev->type) {
    case ButtonPress: {
      if (ev->xbutton.button == Button1) {
        Duck *d;      
        for (d = dock->duck_lst; d; d = d->next) {
          if (ev->xany.window == d->win) {
            if (!duck_is_dead(d)) {
              if (duck_going_left(d) || duck_turns_rhl(d)) {
                d->step = DUCK_DIES_LEFT0;
              } else d->step = DUCK_DIES_RIGHT0;
              d->shot = 1;
            }
            return 1;
          }
        }
      }
    } break;
    default: break;
  }
  return 0;
}


static int queue_sz = 0;
#define MAX_QUEUE 50
static id_type queue[MAX_QUEUE];

static void pan_pan(Dock *dock, board_msg_info *mi) {
  int i, j;
  /* tri des canards morts dans le duck-lancher */
  for (i = 0, j = 0; i < queue_sz; ++i) {
    if (!id_type_eq(queue[i],mi->id)) {
      queue[j++] = queue[i];
    }
  }
  queue_sz = j;
  /* et on liquide les canards tirés en plein vol */
  Duck *d, *d_next;
  for (d = dock->duck_lst; d; d = d_next) {
    d_next = d->next;
    if (id_type_eq(d->id, mi->id)) {
      if (!duck_is_dead(d)) {
        if (duck_going_left(d) || duck_turns_rhl(d)) {
              d->step = DUCK_DIES_LEFT0;
            } else d->step = DUCK_DIES_RIGHT0;
        break; /* un de moins (si plusieurs ont été laches par un meme post, il faudra plusieurs coups de fusil) */
      }
      //balltrap_remove(dock, d);
    }
  }
}

void balltrap_check_message(id_type id, const unsigned char *msg) {
  int coin_found = 0, pan_found = 0;
  if (!Prefs.hunt_opened || get_dock()->horloge_mode) return;
  if (id_type_sid(id) >= 0 && !Prefs.site[id_type_sid(id)]->hunt_opened_on_site) return;
  board_msg_info *mi = boards_find_id(get_dock()->sites->boards, id);
  if (mi == NULL) return;
  
  if (1) { //!mi->is_my_message) {    
    /* on compte le nombre de ascii-canards */
    const char *p, *q, **ad;
    const char *ascii_ducks[] = {"\\_o<", "\\_O<", "\\_0<", "\\_°<", ">o_/", ">O_/", ">0_/", ">°_/", NULL};
    for (ad = ascii_ducks; *ad; ++ad) {
      p = msg;
      while ((q=strstr(p, *ad))) {
        coin_found++; p = q+strlen(*ad);
      }
    }
    coin_found = MIN(coin_found, 4); /* faut pas exagerer non plus! */

    /* et les coin ! coin ! */
    {
      char *pattern = "(.*[^A-Za-z0-9_]|^)([cC][oO0][iI1][nN] *! *[cC][oO0][iI1][nN] *!|[Pp][oO]*[uU]+[lL]+[eE]? *!)([^A-Za-z0-9_].*|$)";
      static regex_t re;
      static int isinit = 0;
      if (!isinit) {
        if (regcomp(&re, pattern, REG_EXTENDED | REG_NOSUB)) assert(0);
        isinit = 1;
      }
      if (regexec(&re, msg, 0, NULL, 0) == 0) {
        //printf("hunt candidat : %s\n", msg);
        coin_found++;
      }
      /*if (strstr(msg, "coin !")) {
        printf("COIN candidate : %s -> coin_found=%d\n", msg, coin_found);
        }*/
    }
    if (mi->in_boitakon) coin_found = 0;
  }

  if (!mi->is_my_message) {
    char *pattern = "(.*[^A-Za-z0-9_]|^)(([pP][aA4]+[nNfF]+[gG]?)|([bB]+[lL]+[aA4]+[mM]+)|([bB]+[oO]+[uU]+[mM]+))([^A-Za-z0-9_].*|$)";
    static regex_t re;
    static int isinit = 0;
    if (!isinit) {
      if (regcomp(&re, pattern, REG_EXTENDED | REG_NOSUB)) assert(0);
      isinit = 1;
    }
    //printf("test: %s\n", msg);
    /*if (strstr(msg, "pan")) {
      printf("pan candidate: %s\n", msg);
      }*/
    if (regexec(&re, msg, 0, NULL, 0) == 0) {
      BLAHBLAH(1, printf("PAN FOUND! %s\n", msg));
      pan_found = 1;
    }
  }

  while (coin_found && queue_sz < MAX_QUEUE) {
    queue[queue_sz++] = id;
    BLAHBLAH(1, myprintf("duck hunt is opened ! prepare your guns!\n -> [%s] msg=%s\n", Prefs.site[id_type_sid(id)]->site_name, msg));
    coin_found--;
    //balltrap_add(get_dock(), id);
  }
  if (pan_found) {
    if (mi) {
      int i;
      for (i = 0; i < mi->nb_refs; ++i) {
        if (mi->refs[i].mi) pan_pan(get_dock(), mi->refs[i].mi);
      }
    }
  }
}

/* on randomise un peu le lacher de canard pour les grandes occasions genre ouverture de la chasse */
void balltrap_launch() {
  while (queue_sz) {
    int i = wmcc_rand(queue_sz);   assert(i < queue_sz);
    balltrap_add(get_dock(), queue[i]);
    BLAHBLAH(1,printf("new duck launched : %s\n", boards_find_id(get_dock()->sites->boards, queue[i])->msg));
    memmove(queue+i, queue+i+1, (queue_sz - i - 1)*sizeof(queue[0]));
    queue_sz--;
  }
}
