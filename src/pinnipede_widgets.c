#include "pinnipede.h"
#include "site.h"
#include "dock.h"

static char *dl_info_site = NULL;
static char *dl_info_what = NULL;

void pp_set_download_info(char *site, char *what) {
  if (dl_info_site) { free(dl_info_site); dl_info_site = NULL; }
  if (dl_info_what) { free(dl_info_what); dl_info_what = NULL; }
  dl_info_site = site ? strdup(site) : NULL;
  dl_info_what = what ? strdup(what) : NULL;
}

/* --------------------------- gestion des tabs ------------------- */
/* a appeler apres update fortune et modif minib*/
static void
pp_tabs_set_pos_vert(Pinnipede *pp) {
  int cnt, rest = 0;

  assert(pp->nb_tabs);

  pp->zmsg_y1 = 0; pp->zmsg_y2 = pp->win_height - 1 - (pp->use_minibar ? MINIB_H : 0);
  pp->zmsg_h = pp->zmsg_y2-pp->zmsg_y1+1;

  int tab_h = (pp->zmsg_h - (Prefs.auto_swallow ? 70 : 0))/pp->nb_tabs;
  tab_h = MIN(PPT_MAX_H,MAX(PPT_BASE_H,tab_h));

  if (Prefs.auto_swallow == 0 && tab_h < PPT_MAX_H) {
    rest = pp->zmsg_h - pp->nb_tabs*tab_h + 1;
    if (rest > pp->nb_tabs) rest = 0;
  }

  if (pp->tabs_pos == PPT_LEFT) {
    pp->zmsg_x1 = pp->tabs_w; 
    pp->tabs_x0 = 0; 
  } else {
    pp->zmsg_x1 = 0;
    pp->zmsg_x2 = pp->win_width - pp->tabs_w - 1 - (pp->sc ? SC_W : 0);
    pp->tabs_x0 = pp->win_width - pp->tabs_w;
  }
  pp->zmsg_w = pp->zmsg_x2 - pp->zmsg_x1 + 1;

  pp->mb_x0 = 0; // la minibar a toute la place

  if (pp->nb_tabs == 0 || pp->use_minibar == 0) return;
  
  pp->tabs_w = 80;
  pp->tabs_y0 = pp->win_height  - (pp->use_minibar ? MINIB_H : 0) - tab_h - (rest ? 1 : 0);
  pp->tabs_h = 0;
  for (cnt = 0; cnt < pp->nb_tabs; cnt++) {
    PinnipedeTab *pt = &pp->tabs[cnt];
    pt->x = pp->tabs_x0;
    pt->y = pp->tabs_y0;
    pt->w = pp->tabs_w;
    pt->h = tab_h + (rest ? 1 : 0); if (rest > 0) --rest;
    pp->tabs_y0 -= pt->h; pp->tabs_h += pt->h;
  }
  pp->tabs_y0 += tab_h;
}

static void
pp_tabs_set_pos_horiz(Pinnipede *pp)
{
  int cnt;
  
  int tab_max_w,tab_min_w, tab_w, tabs_min_w=-1, tabs_w=-1;
  int x,y;

  pp->zmsg_y1 = 0;
  pp->zmsg_y2 = pp->win_height - 1 - (pp->use_minibar ? MINIB_H : 0);
  pp->zmsg_h = pp->zmsg_y2-pp->zmsg_y1+1;
  pp->zmsg_x1 = 0; 
  // pp->zmsg_x2 = pp->win_width - 1;  gere par la scrollcoin
  pp->zmsg_w = pp->zmsg_x2-pp->zmsg_x1+1;
  

  if (pp->nb_tabs == 0 || pp->use_minibar == 0) return;

  tab_min_w = 40;
  tab_max_w = 100;
  tabs_min_w = pp->nb_tabs * tab_min_w;
  tab_w = MIN(MAX(pp->win_width / pp->nb_tabs, tab_min_w), tab_max_w);
  tabs_w = pp->nb_tabs * tab_w;

  pp->mb_x0 = 0;
  
  x = 0;
  if (pp->tabs_pos == PPT_DOWN) {
    y = pp->win_height - PPT_BASE_H;
    
    if (pp->use_minibar && tabs_min_w + pp->mb_min_width > pp->win_width) {
      y -= MINIB_H;
      tabs_w = pp->win_width+1;
    } else {
      /* "reduction" de la largeur pour loger dans la minibar */
      tab_w = MIN((pp->win_width - pp->mb_min_width) / pp->nb_tabs, tab_max_w);
      tabs_w = pp->nb_tabs * tab_w;
      pp->mb_x0 = tabs_w;
    }
    pp->zmsg_y2 = y-1;
  } else {
    y = 0;
    pp->zmsg_y1 += PPT_BASE_H;
  }

  for (cnt = 0; cnt < pp->nb_tabs; cnt++) {
    PinnipedeTab *pt = &pp->tabs[cnt];
    pt->x = (cnt * tabs_w) / pp->nb_tabs;
    pt->y = y;
    pt->w = ((cnt+1) * tabs_w) / pp->nb_tabs - x;
    pt->h = PPT_BASE_H;
    x += pt->w;
  }
  pp->tabs_w = MIN(x, pp->win_width);
  pp->tabs_x0 = 0; 
  pp->tabs_y0 = y;
  pp->tabs_h = PPT_BASE_H; 
  pp->zmsg_h = pp->zmsg_y2-pp->zmsg_y1+1;
}

static void
pp_tabs_set_pos(Pinnipede *pp) {
  if (pp->use_minibar) {
    if (pp->tabs_pos == PPT_LEFT || pp->tabs_pos == PPT_RIGHT)
      pp_tabs_set_pos_vert(pp);
    else 
      pp_tabs_set_pos_horiz(pp);
  } else {
    pp->zmsg_y1 = 0;
    pp->zmsg_y2 = pp->win_height - 1;
    pp->zmsg_h = pp->zmsg_y2-pp->zmsg_y1+1;
    pp->zmsg_x1 = 0; 
    pp->tabs_w = 0;
// pp->zmsg_x2 = pp->win_width - 1;  gere par la scrollcoin
    pp->zmsg_w = pp->zmsg_x2-pp->zmsg_x1+1;
  }
}

void
pp_tabs_set_visible_sites(Pinnipede *pp) {
  int i;
  for (i=0; i < MAX_SITES; i++) pp->filter.visible_sites[i] = 0;
  for (i=0; i < pp->nb_tabs; i++) {
    pp->filter.visible_sites[pp->tabs[i].site->site_id] = pp->tabs[i].selected; 
  }
}

void
pp_tabs_set_tmp_one_visible_site(Pinnipede *pp, int sid) {
  int i;
  for (i=0; i < MAX_SITES; i++) pp->filter.visible_sites[i] = 0;
  pp->filter.visible_sites[sid] = 1;
}

static void
pp_tabs_save_selected(Pinnipede *pp) {
  int i;
  for (i=0; i < pp->nb_tabs; i++) pp->tabs[i].was_selected = pp->tabs[i].selected;
}

static void
pp_tabs_restore_selected(Pinnipede *pp) {
  int i;
  for (i=0; i < pp->nb_tabs; i++) pp->tabs[i].selected = pp->tabs[i].was_selected;
}

static void
pp_tabs_swap_selected(Pinnipede *pp) {
  int i;
  for (i=0; i < pp->nb_tabs; i++) { 
    int tmp = pp->tabs[i].was_selected;
    pp->tabs[i].was_selected = pp->tabs[i].selected;
    pp->tabs[i].selected = tmp;;
  }
}

static void
pp_tabs_select_only_one(Pinnipede *pp, PinnipedeTab *pt) {
  int i;
  for (i=0; i < pp->nb_tabs; i++) pp->tabs[i].selected = (&pp->tabs[i] == pt) ? 1 : 0;
}


static int
pp_tabs_nb_selected(Pinnipede *pp) {
  int cnt = 0, i;
  for (i=0; i < pp->nb_tabs; i++) if (pp->tabs[i].selected) ++cnt;
  return cnt;
}

static void
pp_tabs_check_active(Pinnipede *pp) {
  int i;
  if (pp->active_tab == NULL || pp->active_tab->selected == 0) {
    pp->active_tab = NULL;
    for (i=0; i < pp->nb_tabs; i++)
      if (pp->tabs[i].selected && pp->tabs[i].site->board->auto_refresh) { 
        pp->active_tab = pp->tabs+i; break; 
      }
    if (pp->active_tab == NULL)
      for (i=0; i < pp->nb_tabs; i++)
        if (pp->tabs[i].selected) {
          pp->active_tab = pp->tabs+i; break; 
        }
    if (pp->active_tab == NULL && pp->nb_tabs > 0) {
      pp->active_tab = &pp->tabs[0]; pp->tabs[0].selected = 1;
    }
    assert(pp->active_tab);
  }
}

Site *
pp_tabs_get_main_site(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp->active_tab) {
    return pp->active_tab->site;
  } else return NULL;
}

void
pp_tabs_save_state(Dock *dock, FILE *f) 
{
  Pinnipede *pp = dock->pinnipede;
  int i;
  fprintf(f, "TABS-STATE:%d\n", pp->nb_tabs);
  for (i=0; i < pp->nb_tabs; ++i) {
    fprintf(f, "TAB:%s\nactive=%d:refresh=%d\n", pp->tabs[i].site->prefs->site_name,
            pp->tabs[i].selected + (pp->tabs+i == pp->active_tab ? 1 : 0),
            pp->tabs[i].site->board->auto_refresh);
  } 
}

void
pp_tabs_restore_state(Dock *dock, FILE *f)
{
  Pinnipede *pp = dock->pinnipede;
  int nb, i, j;
  if (fscanf(f, "TABS-STATE:%d\n", &nb) == 1 && nb < 2000) {
    for (i=0; i < nb; ++i) {
      char *s = str_fget_line(f); str_trim(s);
      if (str_startswith(s, "TAB:")) {
        char *sname = s+4;
        int act, ref;
        if (fscanf(f, "active=%d:refresh=%d\n", &act, &ref) == 2) {
          //printf("pp_tabs_restore_state: got site '%s' act=%d ref=%d\n", sname, act, ref);
          Site *s = sl_find_site_by_name(dock->sites, sname);
          if (s && s->board) {
            s->board->auto_refresh = ref;
            for (j=0; j < pp->nb_tabs; j++) {
              if (s == pp->tabs[j].site) {
                //printf("pp_tabs_restore_state: restoring %s\n", sname);
                pp->tabs[j].selected = pp->tabs[j].was_selected = act ? 1 : 0;
                if (act > 1) pp->active_tab = pp->tabs+j;
              }
            }
          }
        }
      } //else         printf("fin du fichier ? %d '%s'\n", strlen(s), s);

      free(s);
    }
  } else {
    printf("pp_tabs_restore_state : failed\n"); 
    for (i=0; i < 10; ++i) printf("%s\n",str_fget_line(f));
    //exit(1);
  }
  pp_tabs_check_active(pp);
  pp_tabs_set_visible_sites(pp);
}

/* a appeler apres update fortune */
void
pp_tabs_build(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  Site *s;
  PinnipedeTab *pt;

  pp->tabs_pos = Prefs.pp_tabs_pos;
  pp->nb_tabs = 0; 
  for (s = dock->sites->list; s; s = s->next) {
    if (s->board) {
      pp->nb_tabs++;
    }
  }

  pp->active_tab = NULL;
  ALLOC_VEC(pp->tabs, pp->nb_tabs, PinnipedeTab);
  for (s = dock->sites->list, pt = pp->tabs; s; s = s->next) {
    if (s->board) {
      pt->site = s;
      pt->site_name = strdup(s->prefs->site_name);
      if (s->board->auto_refresh && pp->active_tab == NULL) {
        pt->selected = 1; pp->active_tab = pt;
      } else pt->selected = 0;
      pt->was_selected = pt->selected;
      pt->clicked = 0;
      pt->clign_decnt = 0;
      pt++;
    }
  }
  if (pp->active_tab == 0) { 
    pp->active_tab = &pp->tabs[0]; pp->tabs[0].selected = pp->tabs[0].was_selected = 1; 
  }
  pp->survol_tab = NULL;
  pp_tabs_set_pos(pp);
  pp_tabs_set_visible_sites(pp);
}

static void
pp_tabs_destroy_(PinnipedeTab **ptab, int *pnb) {
  int i;
  for (i=0; i < *pnb; i++) { free((*ptab)[i].site_name); }
  if (*ptab) {
    free(*ptab); *ptab = NULL; 
  }
  *pnb = 0;
}

void pp_tabs_destroy(Pinnipede *pp) {
  pp_tabs_destroy_(&pp->tabs, &pp->nb_tabs);
  pp->active_tab = NULL;
}  

void pp_tabs_rebuild(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  PinnipedeTab *old_tabs = pp->tabs;
  char *old_active_tab = pp->active_tab ? pp->active_tab->site_name : NULL;
  int old_nb_tabs = pp->nb_tabs, i;
  pp_tabs_build(dock);
  for (i = 0; i < pp->nb_tabs; ++i) {
    int j;
    for (j = 0; j < old_nb_tabs; ++j) {
      if (strcmp(pp->tabs[i].site_name, old_tabs[j].site_name) == 0) {
        pp->tabs[i].selected = old_tabs[j].selected;
        pp->tabs[i].was_selected = old_tabs[j].was_selected;
      }
      if (old_active_tab && strcmp(pp->tabs[i].site_name, old_active_tab) == 0) pp->active_tab = pp->tabs + i;
    }
  }
  pp_tabs_destroy_(&old_tabs, &old_nb_tabs);
  pp_tabs_set_visible_sites(pp);
}


int pp_tabs_is_site_visible(Dock *dock, Site *s) {
  Pinnipede *pp = dock->pinnipede;
  int i;
  for (i=0; i < pp->nb_tabs; i++) {
    if (pp->tabs[i].site == s && pp->tabs[i].selected) {
      return 1;
    }
  }
  return 0;
}

void
pp_tabs_set_flag_answer_to_me(Dock *dock, Site *s)
{
  Pinnipede *pp = dock->pinnipede;
  int i;
  for (i=0; i < pp->nb_tabs; i++) {
    if (pp->tabs[i].site == s) {
      pp->tabs[i].clign_decnt = wmcc_tic_cnt;
    }
  }
}


static unsigned long 
pp_tabs_bg_pixel_of_tab(Dock *dock, PinnipedeTab *pt) {
  Pinnipede *pp = dock->pinnipede;
  if (Prefs.pp_use_colored_tabs && pt->selected)
    return cccolor_pixel(pp->win_bgcolor[pt->site->site_id]);
  else
    return cccolor_pixel(pp->minib_color);
}

static XPoint * get_grip(int *nb_points, int which) {
  static XPoint *pts[2] = { NULL, NULL };
  static int cnt = 0;
  const char *grip[] = {"0001000000",
                        "0001100000",
                        "0001110000",
                        "1111111000",
                        "1111111100",
                        "1111111110",
                        "1111111111",
                        "1111111110",
                        "1111111100",
                        "1111111000",
                        "0001110000",
                        "0001100000",
                        "0001000000"};
  if (pts[which] == NULL) {
    int nx = strlen(grip[0]); 
    int ny = sizeof grip / sizeof(char*);
    int i,j, pi, pj;
    cnt = 0;
    for (i = 0; i < nx; ++i) 
      for (j = 0; j < ny; ++j) 
        if (grip[j][i] == '1') ++cnt;
    ALLOC_VEC(pts[which], cnt+1, XPoint);
    /* pour le CoordModePrevious -- man XDrawPoints */
    pi = nx/2; pj = ny/2;
    pts[which][0].x = pi; pts[which][0].y = pj;
    cnt = 1;
    for (i = 0; i < nx; ++i) 
      for (j = 0; j < ny; ++j) 
        if (grip[j][i] == '1') { 
          if (which == 0) {
            /* a gauche */
            pts[which][cnt].x = i-pi; pts[which][cnt].y = j-pj; 
          } else if (which == 1) {
            /* a droite */
            pts[which][cnt].x = pi-i; pts[which][cnt].y = j-pj; 
          }
          pi = i; pj = j;
          ++cnt;
        }
  }
  *nb_points = cnt;
  return pts[which];
}


static void pp_tabs_draw_one_tab(Dock *dock, PinnipedeTab *pt, Drawable drawable, int d_x, int d_y)
{
  Pinnipede *pp = dock->pinnipede;
  int w = pt->w, h=pt->h;
  unsigned long bar_pixel = cccolor_pixel(pp->progress_bar_color), bgpixel;
  Board *board = pt->site->board;
  int fn_h = ccfont_height(pp->fn_minib);
  int draw_grip = 0; /*  draw_grip : la fleche bleu.  < 0 -> à gauche, > 0 -> à droite*/

  if (pt == pp->survol_tab && Prefs.pp_use_classical_tabs == 0) {
    if (pp->survol_tab_part == PPT_MAY_SET_MAIN_TAB) draw_grip = -1;
    else if (pp->survol_tab_part == PPT_MAY_UNSELECT_TAB) draw_grip = +1;
  }
  bgpixel = pp_tabs_bg_pixel_of_tab(dock,pt);

  if (pt->clign_decnt) {
    int decnt = wmcc_tic_cnt - pt->clign_decnt;
    float c = (decnt % 50)/(70.0);
    int b = 255*c + (bgpixel & 0x0000ff)*(1-c);
    int r = ((bgpixel & 0xff0000)>>16)*(1-c); 
    int g = ((bgpixel & 0x00ff00)>>8)*(1-c);
    
    if (decnt > FLAMOMETRE_TRIB_DUREE*(1000/WMCC_TIMER_DELAY_MS)) pt->clign_decnt = 0;
    r = MIN(r,255);
    g = MIN(g,255);
    b = MIN(b,255);
    XSetForeground(dock->display, dock->NormalGC, RGB2PIXEL(r,g,b));
    XFillRectangle(dock->display, drawable, dock->NormalGC, 
		   d_x, d_y+1, w-1, h-1);
  } else { //if (Prefs.pp_use_colored_tabs) {
    XSetForeground(dock->display, dock->NormalGC, bgpixel);    
    XFillRectangle(dock->display, drawable, dock->NormalGC, 
                   d_x, d_y+1, w-1, h-1);
  }

  if (pt->site->board->auto_refresh) {
    XSetForeground(dock->display, dock->NormalGC, 
                   cccolor_pixel(pp->minib_dark_color));
    XDrawLine(dock->display, drawable, dock->NormalGC, d_x+w-6, d_y+h-5, d_x+w-2, d_y+h-1);
    XDrawLine(dock->display, drawable, dock->NormalGC, d_x+w-6, d_y+h-1, d_x+w-2, d_y+h-5);
  }
  
  XSetForeground(dock->display, dock->NormalGC, bar_pixel);
  if (board->board_refresh_delay > 0 && board->auto_refresh) {
    int zw = ((w-7)*MIN(board->board_refresh_delay - board->board_refresh_decnt,board->board_refresh_delay))/board->board_refresh_delay;
    zw = (w-7 - zw);
    //	printf("zw=%d %d %d %d\n",zw,board->board_refresh_cnt,board->board_refresh_delay, pt->w);
    if (zw > 0) {
      XFillRectangle(dock->display, drawable, dock->NormalGC, d_x, d_y+h-4, zw, 4);
    }
  }

  /* les petites coches rouges */
  if (pt->site->http_recent_error_cnt) {
    int x0 = d_x+3;
    int j;
    XSetForeground(dock->display, dock->NormalGC, 
                   pt->selected ? 
                   IRGB2PIXEL(0xf04040) : IRGB2PIXEL(0xd0a0a0));
    for (j=0; j < 4; ++j)
      XDrawLine(dock->display, drawable, dock->NormalGC, 
                d_x,d_y+h-1-j,d_x+3-j,d_y+h-1-j);
    for (j=1; j < pt->site->http_recent_error_cnt && x0+10 < d_x+w; ++j) {
      XDrawLine(dock->display, drawable, dock->NormalGC, 
                x0,d_y+h-4,x0+3,d_y+h-1);
      XDrawLine(dock->display, drawable, dock->NormalGC, 
                x0+1,d_y+h-4,x0+4,d_y+h-1);
      x0 += 4;
    }
  }

  if (draw_grip) {
    int nb_pts;
    XPoint *gpts;
    if (draw_grip < 0) {
      gpts = get_grip(&nb_pts, 0);
      gpts[0].x = d_x + 6;
    } else {
      gpts = get_grip(&nb_pts, 1);
      gpts[0].x = d_x + pt->w - 8;
    }
    gpts[0].y = d_y + pt->h/2;
    XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0x106fe5));
    XDrawPoints(dock->display,  drawable, dock->NormalGC, gpts, nb_pts, CoordModePrevious);
  }
  {
    char *t; 
    int tw, tx, ty, tlen, clen;
    CCColorId fgcolor = (bgpixel != 0L ? dock->gray_colors[3] : dock->gray_colors[8]);
    int main_site = 0;
    int cnt_new_msg = 0, cnt_unviewed_msg = 0;
    int maxw = pt->w - (draw_grip ? 10 : 0), base_x = d_x + (draw_grip < 0 ? 10 : 0);
    char scnt[10];
    int cw, cx, cy;
    int count_below_title = (pt->h >= fn_h*2 ? 1 : 0);
    if (ccqueue_find(Q_BOARD_UPDATE, pt->site->site_id)) {
      int l = ABS((wmcc_tic_cnt % 23) - 11);//*10;
      t = ((wmcc_tic_cnt % 180) < 30) ? pt->site->prefs->site_name : "-queued-";
      fgcolor = dock->gray_colors[l];//0x303030 + (l<<16) + (l<<8) + l;
    } else {
      t = pt->site->prefs->site_name;
      if (pt == pp->active_tab) main_site = 1;
      if (!pt->selected) fgcolor = dock->gray_colors[9];//pixel = 0x909090;
      cnt_new_msg =  board->nb_msg_at_last_check;
      cnt_unviewed_msg = board->nb_msg_since_last_viewed;
    }
    //tlen = MIN((int)strlen(t), maxw / MINIB_FN_W);
    //tw = tlen * MINIB_FN_W;
    
    tlen = ccfont_text_within_width8(pp->fn_minib, t, -1, maxw, &tw);
    if (cnt_new_msg && wmcc_tic_cnt - board->wmcc_tic_cnt_last_check < 50) {
      snprintf(scnt, 10, "+%d", cnt_new_msg);
      clen = ccfont_text_within_width8(pp->fn_minib, scnt, -1, (count_below_title ? maxw : maxw - tw) - 2, &cw);
      /*clen = MIN((int)strlen(scnt), 
                 count_below_title ? (maxw - 2)/MINIB_FN_W : (maxw - tw-2)/MINIB_FN_W);
                 cw = clen * MINIB_FN_W;*/
    } else if (cnt_unviewed_msg) {
      snprintf(scnt, 10, "%d", cnt_unviewed_msg); //new_msg);
      clen = ccfont_text_within_width8(pp->fn_minib, scnt, -1, (count_below_title ? maxw : maxw - tw) - 2, &cw);
      /*clen = MIN((int)strlen(scnt), 
                 count_below_title ? (maxw - 2)/MINIB_FN_W : (maxw - tw-2)/MINIB_FN_W);
                 cw = clen * MINIB_FN_W;*/
    } else { cw = 0; clen = 0; }
    
    /* position du titre et du compteur */
    ty = d_y + ccfont_ascent(pp->fn_minib) + 1 + pt->clicked*2;
    if (pt->h <= 12) ty--;
    if (!count_below_title) {
      tx = base_x + MAX((maxw - tw - cw - 2)/2,0);
      cx = tx + tw + 4; 
      cy = ty;
    } else {
      tx = base_x + MAX((maxw - tw - 2)/2,0);
      cx = base_x + MAX((maxw - cw - 2)/2,0);
      cy = ty + fn_h;
      if (cw == 0) { // recentrage du titre
        ty = d_y + (pt->h + fn_h)/2 - 2 + pt->clicked*2; 
      }
    }
    if (!pt->selected) {
      XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(fgcolor));
    } else {
      XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(fgcolor));
      if (main_site && tlen && pp->tabs_pos != PPT_UP && pp->tabs_pos != PPT_DOWN) {
        //XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(fgpixel));//IRGB2PIXEL(0x80ff80));
        //XDrawString(dock->display, drawable, dock->NormalGC, tx+1, ty+1, t, tlen);
        XDrawLine(dock->display, drawable, dock->NormalGC, tx-1, ty+1, tx+tw, ty+1);
      }
    }
    //printf("t = %s, tw=%d tlen=%d tx=%d ww=%d\n", t, tw, tlen, tx, ccfont_text_width8(pp->fn_minib, t, -1));
    ccfont_draw_string8(pp->fn_minib, fgcolor, drawable, tx, ty, t, tlen); /* COUL */
    //XDrawString(dock->display, drawable, dock->NormalGC, tx, ty, t, strlen(t));
    if (cw && clen) {
      //XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(pt->selected ? 0x000080 : 0x8080a0)); /* COUL */
    ccfont_draw_string8(pp->fn_minib, dock->blue_color, drawable, cx, cy, scnt, clen);
    //XDrawString(dock->display, drawable, dock->NormalGC, cx, cy, scnt, clen);
    }
  }
}

void
pp_tabs_refresh(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp->nb_tabs == 0) return;
  //XSetFont(dock->display, dock->NormalGC, pp->fn_minib->fid);

  /* dessin general */
  //  XSetForeground(dock->display, dock->NormalGC, pp->minib_pixel);

  /* dessin des lignes */
  if (pp->tabs_pos == PPT_DOWN || pp->tabs_pos == PPT_UP) {
    int y1, y2,i;
    y1 = 0; y2 = pp->tabs[0].h - 1;
    if (pp->tabs_pos == PPT_UP) {
      int tmp = y1; y1 = y2; y2 = tmp; 
    }


    /* efface toute la ligne */
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_color));
    XFillRectangle(dock->display, pp->lpix, dock->NormalGC, 
		   0, 0, pp->win_width, pp->fn_h);

    /* dessine la ligne du haut */
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_dark_color));
    XDrawLine(dock->display, pp->lpix, dock->NormalGC, 0, y1, pp->tabs_w, y1); 


    for (i=0; i < pp->nb_tabs; i++) {
      PinnipedeTab *pt = &pp->tabs[i];
      /* le separateur à droite */
      XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_dark_color));
      XDrawLine(dock->display, pp->lpix, dock->NormalGC, pt->x+pt->w-1, 
                0, pt->x+pt->w-1, pt->h-1);

      pp_tabs_draw_one_tab(dock, pt, pp->lpix, pt->x, 0);
    }

    /* "ouvre" le tab active */
    XSetForeground(dock->display, dock->NormalGC, pp_tabs_bg_pixel_of_tab(dock,pp->active_tab));
    XDrawLine(dock->display, pp->lpix, dock->NormalGC, 
              pp->active_tab->x-1, y1, pp->active_tab->x + pp->active_tab->w-1, y1);

    /* et hop */
    XCopyArea(dock->display, pp->lpix, pp->win, dock->NormalGC,
              0,0,pp->tabs_w, pp->tabs[0].h, pp->tabs[0].x,pp->tabs[0].y);
  } else {
    int i;
    /* efface toute la ligne */
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_color));
    XFillRectangle(dock->display, pp->lpix, dock->NormalGC, 
                   0, 0, pp->tabs[0].w, pp->tabs[0].h);
    for (i=0; i < pp->nb_tabs; i++) {
      PinnipedeTab *pt = &pp->tabs[i];
      PinnipedeTab *npt = (i<pp->nb_tabs-1) ? &pp->tabs[i+1] : NULL;
      int xbord = (pp->tabs_pos == PPT_LEFT ?  pt->x+pt->w-1 : pt->x);
      //int dx = (pp->tabs_pos == PPT_LEFT ? -1 : +1);
      /* le contenu */
      pp_tabs_draw_one_tab(dock, &pp->tabs[i], pp->lpix, 0, 0);

      /* le separateur à droite */
      XSetForeground(dock->display, dock->NormalGC, pt != pp->active_tab ?
                     cccolor_pixel(pp->minib_dark_color) : pp_tabs_bg_pixel_of_tab(dock,pt));
      XDrawLine(dock->display, pp->lpix, dock->NormalGC, xbord, 
                0, xbord, pt->h);

      /* le point en haut à droite si le prochain tab est le principal
         pour l'effet "vaguement arondi" */
      if (npt == pp->active_tab) { 
        XSetForeground(dock->display, dock->NormalGC, pp_tabs_bg_pixel_of_tab(dock,npt));
        XDrawPoint(dock->display, pp->lpix, dock->NormalGC, xbord, 0);
      }

      /* la ligne du haut */
      if (pt == pp->active_tab || npt == NULL || npt == pp->active_tab) {
        XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_dark_color));
      } else if (!pt->selected && !npt->selected) {
        XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0xC0C0C0));
      } else {
        XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_dark_color));
      }
      XDrawLine(dock->display, pp->lpix, dock->NormalGC, 0, 0, pt->w-2, 0);
      /* la ligne a gauche pour le tab principal */
      /*      if (pt == pp->active_tab) {
        XDrawLine(dock->display, pp->lpix, dock->NormalGC, pt->x, 0, pt->x, pt->h-1);
        XDrawLine(dock->display, pp->lpix, dock->NormalGC, pt->x, pt->h-1, pt->w-2, pt->h-1);
        XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0xC0C0C0));
        XDrawLine(dock->display, pp->lpix, dock->NormalGC, pt->x, 1, pt->w-2, 1);
        }*/

      XCopyArea(dock->display, pp->lpix, pp->win, dock->NormalGC,
                0,0,pp->tabs[i].w, pp->tabs[i].h, pp->tabs[i].x + (pp->tabs_pos == PPT_RIGHT ? 1 : 0),pp->tabs[i].y);
    }
  }

  //  printf("tabs_w = %d, mb_w = %d\n", pp->tabs_width, pp->mb_buttonbar_width);
}

static PinnipedeTab *
pp_tabs_at_xy(Dock *dock, int x, int y, ppt_survol_actions *pi) {
  Pinnipede *pp = dock->pinnipede;
  int i;
  if (pi) *pi = PPT_NORMAL_ACTION;
  for (i=0; i < pp->nb_tabs; i++) {
    PinnipedeTab *pt = &pp->tabs[i];
    if (x >= pt->x && y >= pt->y && 
	x <= pt->x + pt->w-1 && 
	y <= pt->y + pt->h-1) {
      if (pi) {
        if (x - pt->x <= pt->w/4/* && pt != pp->active_tab*/) *pi = PPT_MAY_SET_MAIN_TAB;
        else if (pt->x + pt->w - x <= pt->w/4 && pt->selected/* && pt != pp->active_tab*/) *pi = PPT_MAY_UNSELECT_TAB;
      }
      return pt;
    }
  }
  return NULL;
}

static PinnipedeTab *
pp_tabs_pressed_tab(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  int i;

  for (i=0; i < pp->nb_tabs; i++) {
    if (pp->tabs[i].clicked)
      return &pp->tabs[i];
  }
  return NULL;
}

static void pp_tabs_handle_motion(Dock *dock, XMotionEvent *ev) {
  Pinnipede *pp = dock->pinnipede;
  PinnipedeTab *pt;
  /* int refresh = 0; */
  ppt_survol_actions survol_part;
  //static int flag = 0;
  pt = pp_tabs_at_xy(dock, ev->x, ev->y, &survol_part);
  /*if (pt && (ev->state & ShiftMask)) {
      if (flag != pt->site->site_id+1) { flag = pt->site->site_id + 1; refresh = 1; }
    pp_tabs_set_tmp_one_visible_site(pp,pt->site->site_id);
  } else if (flag) {
    pp_tabs_set_visible_sites(pp);
    flag = 0; refresh = 1;
    } */
  /* affichage des fleches bleues */
  if (pt != pp->survol_tab || survol_part != pp->survol_tab_part) {
    pp->survol_tab = pt; 
    pp->survol_tab_part = survol_part;
    pp_widgets_refresh(dock);
    /* refresh = 1; */
  }
  /*if (refresh) {
    pp_update_and_redraw(dock,get_last_id_filtered(dock->sites->boards, &pp->filter), 100,0,1);
    }*/
}

static int
pp_tabs_handle_button_press(Dock *dock, XButtonEvent *ev) {
  //  Pinnipede *pp = dock->pinnipede;
  PinnipedeTab *pt;
  pt = pp_tabs_at_xy(dock, ev->x, ev->y, NULL);
  if (pt) {
    pt->clicked = 1;
    pp_minib_refresh(dock);
    return 1;
  } else return 0;
}


void pp_tabs_switch_all_selected(Pinnipede *pp) {
  int i;
  if (pp_tabs_nb_selected(pp) == 1) {
    pp_tabs_save_selected(pp);
    for (i=0; i < pp->nb_tabs; i++) pp->tabs[i].selected = 1;
  } else {
    pp_tabs_save_selected(pp);
    for (i=0; i < pp->nb_tabs; i++) pp->tabs[i].selected = (i == pp->active_tab - pp->tabs) ? 1 : 0;
  }
}

/* effectue la rotation bizarre 1 tabs actif/tous les tabs/etc */
void pp_tabs_cliquouille(Pinnipede *pp, PinnipedeTab *pt, ppt_survol_actions survol_part) {
  int i, nb_selected = pp_tabs_nb_selected(pp);
  //printf("cliquouille %d, selected = %d, nb_sel=%d, part=%d\n", pt - pp->tabs, pt->selected, nb_selected, survol_part);
  if (Prefs.pp_use_classical_tabs) {
    pp->active_tab = pt;
    for (i=0; i < pp->nb_tabs; i++) 
      pp->tabs[i].selected = (pp->tabs+i == pt);
  } else if (survol_part == PPT_MAY_UNSELECT_TAB) {
    if (pt->selected && nb_selected > 1) { pt->selected = 0; pp_tabs_save_selected(pp); }
    else if (nb_selected == 1) pp_tabs_swap_selected(pp);
    else { pt->selected = 1; pp_tabs_save_selected(pp); }
  } else if (survol_part == PPT_MAY_SET_MAIN_TAB) {
    if (pt == pp->active_tab && nb_selected == 1) { pp_tabs_restore_selected(pp); }
    else { 
      //if (nb_selected > 1 || !pt->selected) pp_tabs_save_selected(pp);
      for (i=0; i < pp->nb_tabs; i++) {
        pp->tabs[i].selected = (pp->tabs+i == pt);
      }
      pp->active_tab = pt;
    }
  } else {
    if (!pt->selected) { pt->selected = 1; pp_tabs_save_selected(pp); }
    else if (pt != pp->active_tab) { pp->active_tab = pt; pp_tabs_save_selected(pp); }
    else { if (nb_selected == 1) pp_tabs_restore_selected(pp); else pp_tabs_select_only_one(pp,pp->active_tab); }
  }
  pp_tabs_check_active(pp);
}

static int
pp_tabs_handle_button_release(Dock *dock, XButtonEvent *event)
{
  Pinnipede *pp = dock->pinnipede;
  PinnipedeTab *pt;
  int i,ret = 0, need_refresh=0;
  ppt_survol_actions survol_part;
  pt = pp_tabs_at_xy(dock, event->x, event->y, &survol_part);
  if (pt && pt->clicked == 1) {
    Board *board = pt->site->board;
    if (event->button == Button1) {
      if ((event->state & (ControlMask | ShiftMask)) == 0 ) {
        pp_tabs_cliquouille(pp,pt,survol_part);
#if 0
        /*	if (event->x > pt->x + pt->w - 6 && event->y > pt->y + pt->h - 6) {
	  board->auto_refresh = 1-board->auto_refresh;
          } else {*/
	  /* clic 'classique sur une tab */
        //printf("clic : %d, %d , tab = %d,%d\n", event->x , event->y, pt->x, pt->y);
        if (Prefs.pp_use_classical_tabs || survol_part == PPT_MAY_SET_MAIN_TAB) {
          pp->active_tab = pt;
          for (i=0; i < pp->nb_tabs; i++) 
            pp->tabs[i].selected = (pp->tabs+i == pt);
          
        } else if (survol_part == PPT_MAY_UNSELECT_TAB) {
          assert(pp->active_tab != pt);
          pt->selected = 0;
        } else {	  /* clic plus tordu */
          pp_tabs_cliquouille(pp, pt);
        }
          /*	}*/
      } else if ((event->state & (ControlMask))) { /* ctrl-left clic */
	if (pt->selected) {
	  if (pt == pp->active_tab) {
	    for (i=0; i < pp->nb_tabs; ++i)
	      if (pt != pp->tabs+i && pp->tabs[i].selected) { pp->active_tab = pp->tabs+i; break; }
	  }
	  if (pt != pp->active_tab) pt->selected = 0;
	} else {
	  pt->selected = 1;
	}
#endif
      } else { /* shift-left clic */
        board->auto_refresh = 1-board->auto_refresh;
      }
    } else if (event->button == Button2) {
      ccqueue_push_board_update(pt->site->site_id);
      pt->site->board->board_refresh_decnt = pt->site->board->board_refresh_delay;
    } else if (event->button == Button3) {
      //if (pt->selected == 0) pt->selected = 1;
      if (editw_ismapped(dock->editw) == 0) {
	editw_show(dock, pt->site->prefs, 0);
      } else editw_change_current_site(dock, pt->site->site_id);
    }
    pp_tabs_changed(dock);
    ret = 1;
  }
  for (i=0; i < pp->nb_tabs; i++) {
    if (pp->tabs[i].clicked) need_refresh = 1;
    pp->tabs[i].clicked = 0;
  }

  if (need_refresh) pp_tabs_refresh(dock);
  return ret;
}

void pp_tabs_changed(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  if (pp->active_tab) {
    pp_tabs_set_visible_sites(pp);
    pp->flag_pp_update_request = 1;
    if (pp_tabs_nb_selected(pp) == 1)
      board_set_viewed(pp->active_tab->site->board, pp->active_tab->site->board->last_post_id);
  }  
}

void
pp_change_active_tab(Dock *dock, int dir) {
  Pinnipede *pp = dock->pinnipede;
  if (pp->active_tab) {
    pp->active_tab+=dir;
    if (pp->active_tab >= pp->tabs + pp->nb_tabs) pp->active_tab = pp->tabs;    
    else if (pp->active_tab < pp->tabs) pp->active_tab = pp->tabs + pp->nb_tabs -1;
  } else pp->active_tab = pp->tabs;
  pp_tabs_select_only_one(pp, pp->active_tab);
  pp_tabs_changed(dock);
}



/* ------------------- gestion de la barre de boutons -------------------------- */
static void
pp_minib_set_pos(Pinnipede *pp)
{
  int i;

  int x;

  x = pp->win_width; //(pp->sc ? SC_W : 0);
  i = 0;
  pp->mb[i].type = HELP;            pp->mb[i].w = SC_W-1; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = SCROLLBAR;       pp->mb[i].w = 12; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = BALLTRAP;        pp->mb[i].w = 22; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = TRANSPARENT;     pp->mb[i].w = 12; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = UA;              pp->mb[i].w = 12; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  // pp->mb[i].type = SECOND;          pp->mb[i].w = 12; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  //  pp->mb[i].type = TSCORE;          pp->mb[i].w = 12; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  //pp->mb[i].type = FORTUNE;         pp->mb[i].w = 12; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = FILTER;          pp->mb[i].w = 12; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = PLOPIFY;         pp->mb[i].w = 12; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = PREFS;           pp->mb[i].w = 40; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = MB_RSS;          pp->mb[i].w = 40; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = MB_BOARDS;       pp->mb[i].w = 40; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb[i].type = CANCEL;          pp->mb[i].w = 12; x -= pp->mb[i].w; pp->mb[i].x = x; i++;

  //  pp->mb[i].type = REFRESH_NEWS;    pp->mb[i].w = 60; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  //  pp->mb[i].type = REFRESH_TRIBUNE; pp->mb[i].w = 60; x -= pp->mb[i].w; pp->mb[i].x = x; i++;
  pp->mb_buttonbar_width = pp->win_width - x;

  assert(i == NB_MINIB);

  for (i=0; i < NB_MINIB; i++) {
    pp->mb[i].clicked = 0;
    pp->mb[i].y = pp->win_height - MINIB_H;
    pp->mb[i].h = MINIB_H;
  }
  pp->mb_min_width = MIN(pp->win_width, 230 + pp->mb_buttonbar_width);
}


/* a nice piece of shit, if i can say */
void
pp_minib_refresh(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  Site *main_site;
  int i,x,y;

  int x_minib;

  main_site = pp->active_tab->site;

  if (pp->use_minibar == 0) return;

  /* dessin general */
  XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_color));
  XFillRectangle(dock->display, pp->lpix, dock->NormalGC, 0, 1, pp->win_width, MINIB_H-1);
  XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_dark_color));
  XDrawLine(dock->display, pp->lpix, dock->NormalGC, 0, 0, pp->win_width, 0);

  x_minib = pp->mb[0].x;
  for (i=0; i < NB_MINIB; i++) x_minib = MIN(x_minib, pp->mb[i].x);

  //XSetFont(dock->display, dock->NormalGC, pp->fn_minib->fid);
  if (pp->filter.filter_mode) {
    char s_filtre[70];
    
    if (pp->filter.filter_name) {
      snprintf(s_filtre, 60, "%.40s [match:%d msg]", 
	       pp->filter.filter_name, count_all_id_filtered(boards, &pp->filter));
    } else {
      snprintf(s_filtre, 60, _("FILTER UNDEFINED"));
    }
    ccfont_draw_string8(pp->fn_minib, pp->minib_dark_color, pp->lpix, 5+pp->mb_x0, ccfont_ascent(pp->fn_minib), s_filtre, strlen(s_filtre));
    //XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_dark_color));
    //XDrawString(dock->display, pp->lpix, dock->NormalGC, 5+pp->mb_x0, pp->fn_minib->ascent+2, s_filtre, strlen(s_filtre));
  } else {
    /* affichage d'infos diverses */

    char s_site[20], s_what[60], s_http_stats[40], s_msg_cnt[30];
    int x, w;
    s_site[0] = 0; s_what[0] = 0; s_msg_cnt[0] = 0;
    if (dl_info_site) snprintf(s_site, sizeof s_site, "[%s]", dl_info_site);
    if (dl_info_what) snprintf(s_what, sizeof s_what, "[%s] (ESC to cancel)", dl_info_what);

    snprintf(s_http_stats, sizeof s_http_stats, "UP:%d, DL:%d", global_http_upload_cnt, global_http_download_cnt);
    snprintf(s_msg_cnt, sizeof s_msg_cnt, "%d msg", pp->non_filtered_message_count);
    
    x = 5+pp->mb_x0;
    w = ccfont_text_width8(pp->fn_minib, s_site, -1); //w = MINIB_FN_W*strlen(s_site);
    if (x+w < x_minib && w) {
      ccfont_draw_string8(pp->fn_minib, dock->blue_color, pp->lpix, x, (ccfont_ascent(pp->fn_minib)), s_site, strlen(s_site));
      x += w + 6;
    }

    w = ccfont_text_width8(pp->fn_minib, s_what, -1); //MINIB_FN_W*strlen(s_what);
    if (x+w < x_minib && w) {
      ccfont_draw_string8(pp->fn_minib, dock->blue_color, pp->lpix, x, (ccfont_ascent(pp->fn_minib)), s_what, strlen(s_what));
      x += w + 6;
    }

    XSetForeground(dock->display, dock->NormalGC, BlackPixel(dock->display, dock->screennum));

    if (strlen(s_http_stats)) {
      w = ccfont_text_width8(pp->fn_minib, s_http_stats, -1);
      if (x+w < x_minib) {
        ccfont_draw_string8(pp->fn_minib, pp->minib_dark_color, pp->lpix, x, (ccfont_ascent(pp->fn_minib)), s_http_stats, strlen(s_http_stats));
        x += w + 6;
      }
    }
    if (strlen(s_msg_cnt)) {
      w = ccfont_text_width8(pp->fn_minib, s_msg_cnt, -1);
      if (x+w < x_minib) {
        ccfont_draw_string8(pp->fn_minib, pp->minib_dark_color, pp->lpix, x, (ccfont_ascent(pp->fn_minib)), s_msg_cnt, strlen(s_msg_cnt));
      }
    }
  }

  /* dessin des boutons */
  for (i=0;i < NB_MINIB; i++) {
    int xc;

    x = pp->mb[i].x;
    xc = x + pp->mb[i].w / 2;
    y = 1;

    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_dark_color));

    if (pp->mb[i].type != CANCEL || dl_info_site)
      XDrawRectangle(dock->display, pp->lpix, dock->NormalGC, x, 0, pp->mb[i].w, pp->mb[i].h);


/*     if (pp->mb[i].clicked && pp->mb[i].type != REFRESH_TRIBUNE && pp->mb[i].type != REFRESH_NEWS) { */
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_color));
      XFillRectangle(dock->display, pp->lpix, dock->NormalGC, x+1, 1, pp->mb[i].w-2, pp->mb[i].h-2);
/*     }     */
    switch (pp->mb[i].type) {
      case SCROLLBAR:
        {
          XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->minib_dark_color));
          XDrawLine(dock->display, pp->lpix, dock->NormalGC, xc, 2, xc, pp->mb[i].h-2);
          XDrawLine(dock->display, pp->lpix, dock->NormalGC, xc-1, 3, xc-1, pp->mb[i].h-3);
          XDrawLine(dock->display, pp->lpix, dock->NormalGC, xc-2, 4, xc-2, pp->mb[i].h-4);
          XDrawLine(dock->display, pp->lpix, dock->NormalGC, xc+1, 3, xc+1, pp->mb[i].h-3);
          XDrawLine(dock->display, pp->lpix, dock->NormalGC, xc+2, 4, xc+2, pp->mb[i].h-4);
        } break;
      case BALLTRAP: 
        {
          XCopyArea(dock->display, pp->miniduck_pixmap, pp->lpix, dock->NormalGC, (Prefs.hunt_opened ? 0 : 19), 0, 19, 11, x+2, 2);
        } break;
      case TRANSPARENT:
        {
          int j;
          for (j=0; j < 7; j++) {
            XSetForeground(dock->display, dock->NormalGC, RGB2PIXEL((j*40),(6-j)*30,0));
            XDrawLine(dock->display, pp->lpix, dock->NormalGC, xc+j-3, 3, xc+j-3, pp->mb[i].h-3);
          }
        } break;
      case MB_RSS:
      case MB_BOARDS:
        {
          char *s = (i == MB_RSS ? "feeds" : "boards");
          int w = ccfont_text_width8(pp->fn_minib, s, -1);
          //XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0x303030)); 
          //XDrawString(dock->display, pp->lpix, dock->NormalGC, xc-(MINIB_FN_W*strlen(s))/2, pp->fn_minib->ascent+2, s, strlen(s));
          ccfont_draw_string8(pp->fn_minib, dock->gray_colors[3], pp->lpix, xc-w/2, ccfont_ascent(pp->fn_minib), s, -1);
        } break;
      case PREFS:
        {
          char *s;
          CCColorId cid;
          if (ccqueue_find(Q_PREFS_UPDATE, -1) == NULL) {
            s = "wmc³";
            //XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0x303030)); /* COUL */
            cid = dock->gray_colors[3];
          } else {
            if ((wmcc_tic_cnt % 24) < 12) s = "updt!";
            else s = "wait";
            //XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0xff3000 + (wmcc_tic_cnt & 0xff))); /* COUL */
            cid = dock->gray_colors[3 + (wmcc_tic_cnt/16)%12];
          }
          //XDrawString(dock->display, pp->lpix, dock->NormalGC, xc-(MINIB_FN_W*strlen(s))/2, pp->fn_minib->ascent+2, s, strlen(s));
          int w = ccfont_text_width8(pp->fn_minib, s, -1);
          ccfont_draw_string8(pp->fn_minib, cid, pp->lpix, xc-w/2, ccfont_ascent(pp->fn_minib), s, -1);
        } break;
        
      case HELP:
        {
          //XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0x303030)); /* COUL */
          //XDrawString(dock->display, pp->lpix, dock->NormalGC, xc-MINIB_FN_W/2+1, pp->fn_minib->ascent+1, "?", 1);
          char *s = "?";
          int w = ccfont_text_width8(pp->fn_minib, s, -1);
          ccfont_draw_string8(pp->fn_minib, dock->gray_colors[3], pp->lpix, xc-w/2, ccfont_ascent(pp->fn_minib), s, -1);
        } break;
        /*
          case SECOND:
          {
          int rx, rw, ry, rh;
          
          rx = x + 3; ry  = y+2; rw = pp->mb[i].w-6; rh = pp->mb[i].h-6;
          XSetForeground(dock->display, dock->NormalGC, pp->timestamp_pixel[main_site->site_id]);
          
          if (pp->show_sec_mode == 0) {
	  XFillRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw+1, rh+1);
          } else {
	  XDrawRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw, rh);
          }
          } break;
          case TSCORE:
          {
	int rx, rw, ry, rh;

	rx = x + 3; ry  = y+2; rw = pp->mb[i].w-6; rh = pp->mb[i].h-6;
	XSetForeground(dock->display, dock->NormalGC, pp->trollscore_pixel[main_site->site_id]);
	
	if (pp->trollscore_mode) {
	  XFillRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw+1, rh+1);
	} else {
	  XDrawRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw, rh);
	}
      } break;
      */
      /*case FORTUNE:
      {
	int rx, rw, ry, rh;

	rx = x + 3; ry  = y+2; rw = pp->mb[i].w-6; rh = pp->mb[i].h-6;
	XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0x8f8060));
	
	if (pp->fortune_mode) {
	  XFillRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw+1, rh+1);
	} else {
	  XDrawRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw, rh);
	}
      } break;
      */
      case FILTER:
        {
          int rx, rw, ry, rh;
          
          rx = x + 3; ry  = y+2; rw = pp->mb[i].w-6; rh = pp->mb[i].h-6;
          XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->lnk_color[main_site->site_id]));
          
          if (pp->filter.filter_mode) {
            XFillRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw+1, rh+1);
          } else {
            XDrawRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw, rh);
          }
        } break;
      case PLOPIFY:
        {
          int rx, rw, ry, rh;
          
          rx = x + 3; ry  = y+2; rw = pp->mb[i].w-6; rh = pp->mb[i].h-6;
          XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->plopify_color));
          
          if (pp->disable_plopify) {
            XFillRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw+1, rh+1);
          } else {
            XDrawRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw, rh);
          }
        } break;
      case UA:
        {
          int rx, rw, ry, rh;
          
          rx = x + 3; ry  = y+2; rw = pp->mb[i].w-6; rh = pp->mb[i].h-6;
          XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->useragent_color[main_site->site_id]));
          
          XDrawRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw, rh);
          if (pp->nick_mode == 3) {
            XFillRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw, rh);
          } else if (pp->nick_mode == 1) {
            XFillRectangle(dock->display, pp->lpix, dock->NormalGC, rx,ry,xc-rx+1,rh);
          } else if (pp->nick_mode == 2) {
            XFillRectangle(dock->display, pp->lpix, dock->NormalGC, xc,ry,xc-rx+1,rh);
          } else if (pp->nick_mode == 4) {
            XDrawLine(dock->display, pp->lpix, dock->NormalGC, xc, ry, xc, ry+rh-1);
          }
        } break;
      case CANCEL:
        if (dl_info_site) {
          int rx, rw, ry, rh;
          rx = x + 3; ry  = y+2; rw = pp->mb[i].w-5; rh = pp->mb[i].h-5;
          XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0x8080ff));
          if (flag_cancel_task == 0) {
            XFillRectangle(dock->display, pp->lpix, dock->NormalGC, rx, ry, rw, rh);
            XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0xb0b0ff));
            XDrawLine(dock->display, pp->lpix, dock->NormalGC, rx + (wmcc_tic_cnt/5)%rw, ry,
                      rx + (wmcc_tic_cnt/5)%rw, ry+rh-1);
            XDrawLine(dock->display, pp->lpix, dock->NormalGC, rx, ry + (wmcc_tic_cnt/5)%rh, 
                      rx+rw-1, ry + (wmcc_tic_cnt/5)%rh);
          } else {
            XDrawLine(dock->display, pp->lpix, dock->NormalGC, 
                      x+1,y+1,x+pp->mb[i].w-2, y+pp->mb[i].h-2);
            XDrawLine(dock->display, pp->lpix, dock->NormalGC, 
                      x+1,y+pp->mb[i].h-2,x+pp->mb[i].w-2, y+1);
          }
        } break;
      default:
        abort(); break;
    }
  }

  /* et hop */
  XCopyArea(dock->display, pp->lpix, pp->win, dock->NormalGC,
	    pp->mb_x0,0,pp->win_width - pp->mb_x0, MINIB_H, 
	    pp->mb_x0, MINIB_Y0);

}


static PPMinib *
pp_minib_get_button(Dock *dock, int x, int y)
{
  Pinnipede *pp = dock->pinnipede;
  int i;

  for (i=0; i < NB_MINIB; i++) {
    if (x >= pp->mb[i].x && x < pp->mb[i].x+pp->mb[i].w && 
	y >= pp->mb[i].y && y < pp->mb[i].y+pp->mb[i].h) {
      if (pp->mb[i].type != CANCEL || dl_info_site)
	return &pp->mb[i];
    }
  }
  return NULL;
}

static PPMinib *
pp_minib_pressed_button(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  int i;

  for (i=0; i < NB_MINIB; i++) {
    if (pp->mb[i].clicked)
      if (pp->mb[i].type != CANCEL || dl_info_site)
	return &pp->mb[i];
  }
  return NULL;
}

void
pp_minib_show(Dock *dock) 
{
  Pinnipede *pp = dock->pinnipede;
  pp->use_minibar = 1;
  pp_tabs_set_pos(pp);
}

void 
pp_minib_hide(Dock *dock) 
{
  Pinnipede *pp = dock->pinnipede;
  pp->use_minibar = 0;
  pp_tabs_set_pos(pp);
}



static int
pp_minib_handle_button_press(Dock *dock, XButtonEvent *ev) {
  Pinnipede *pp = dock->pinnipede;
  PPMinib *mb;
  mb = pp_minib_get_button(dock, ev->x, ev->y);
  if (mb && ev->button == Button1) {
    mb->clicked = 1;
    pp_minib_refresh(dock);
    return 1;
  } if (mb && ev->button == Button3) {
    /* menu popup contenant la liste des mots plop */
    if (mb->type == PLOPIFY && Prefs.plopify_key_list) {
      KeyList *hk;
      int cnt;
      plopup_unmap(dock);
      for (hk = Prefs.plopify_key_list, cnt=0; hk && cnt < 30; hk = hk->next, cnt++) {
	char *splop[4] = {"plopify list", "superplopify list", "boitakon", "hungry boitakon"};    
	char s[512];
	//	if (hk->type != HK_
	snprintf(s, 512, _("remove %s:'<font color=blue>%.15s</font>' from %s"), key_list_type_name(hk->type), hk->key, splop[hk->num]);
	plopup_pushentry(dock, s, cnt);	
      }
      cnt = plopup_show_modal(dock, ev->x_root, ev->y_root);
      if (cnt >= 0) {
	for (hk = Prefs.plopify_key_list; hk && cnt > 0; hk = hk->next, cnt--) {
	  /* plop*/
	}
	assert(hk);
	Prefs.plopify_key_list = key_list_remove(Prefs.plopify_key_list, hk->key, hk->type);
	boards_update_boitakon(dock->sites->boards);        
	/* force le rafraichissement complet */
	pp_update_and_redraw(dock, pp->id_base, pp->decal_base,0,0);
      }
    }
    return 1;
  } else return 0;
}


static int
pp_minib_handle_button_release(Dock *dock, XButtonEvent *event)
{
  Pinnipede *pp = dock->pinnipede;

  PPMinib *mb;
  int i, need_refresh, ret = 0;

  //SitePrefs *main_prefs = pp->active_tab->site->prefs;

  pp_selection_unselect(pp);
  if (event->button != Button1) return 0; /* anti boulay du genre shift qui donne de grands coup de roulette (== button4/5) sur le bouton wmccc */

  mb = pp_minib_get_button(dock, event->x, event->y);
  if (event->type == ButtonRelease  && mb && mb->clicked == 1) {
    switch (mb->type) {
      
/*     case REFRESH_TRIBUNE: */
/*       { */
/* 	dock->board_update_request = 1; pp_minib_refresh(dock); */
/*       } break; */

/*     case REFRESH_NEWS: */
/*       { */
/* 	dock->news_update_request = 1; pp_minib_refresh(dock); */
/*       } break; */
      
      case HELP:
        {
          pp_minib_refresh(dock);
          pp_balloon_help(dock, mb->x-20, mb->y-20);
        } break;
      case UA:
        {
          pp->nick_mode = (pp->nick_mode + 1) % 5;
          pp_update_and_redraw(dock, pp->id_base, pp->decal_base,0,1);
        } break;
      case FILTER:
        {
          pp_change_filter_mode(dock, 1 - pp_get_filter_mode(dock));
        } break;
      case PLOPIFY:
        {
          pp->disable_plopify = (1-pp->disable_plopify);
          pp->filter.filter_boitakon = 1-pp->disable_plopify;
          pp_update_and_redraw(dock, pp->id_base, pp->decal_base,0,1);
        } break;
      case SCROLLBAR:
        {
          pp_scrollcoin_set(dock,pp->sc == NULL);
          pp_update_and_redraw(dock, pp->id_base, pp->decal_base,0,1);
        } break;
      case BALLTRAP:
        {
          Prefs.hunt_opened = 1 - Prefs.hunt_opened;
        } break;
      case TRANSPARENT:
        {
          if (Prefs.use_fake_real_transparency) {
            pp_unmap(dock); XFlush(dock->display); 
            usleep(300000); /* pour laisser le temps aux autres applis de se refresher
                               on atteint des sommets de laideur
                               pas sur que c'était une bonne idée cette option use_fake_real_transparency
                            */
          }
          pp_change_transparency_mode(dock, 1-pp->transparency_mode);
          if (Prefs.use_fake_real_transparency) 
            pp_show(dock);
          else {
            pp_update_bg_pixmap(dock);
          }
          pp_refresh(dock, pp->win, NULL);
        } break;
      case MB_RSS:
      case MB_BOARDS:
        {
          int i, feed_cnt = 0, board_cnt = 0, feed_selected_cnt = 0, board_selected_cnt = 0;
          for (i = 0; i < pp->nb_tabs; ++i) {
            if (board_is_rss_feed(pp->tabs[i].site->board)) {
              feed_cnt++; if (pp->tabs[i].selected) feed_selected_cnt++;
            }
            if (board_is_regular_board(pp->tabs[i].site->board)) {
              board_cnt++; if (pp->tabs[i].selected) board_selected_cnt++;
            }
          }
          if ((feed_cnt == 0 && mb->type == MB_RSS) ||
              (board_cnt == 0 && mb->type == MB_BOARDS))
            break;
          if ((mb->type == MB_BOARDS && board_cnt == board_selected_cnt && feed_selected_cnt == 0) ||
              (mb->type == MB_RSS && feed_cnt == feed_selected_cnt && board_selected_cnt == 0)) {
            pp_tabs_restore_selected(pp);pp_tabs_check_active(pp);
          } else {
            pp_tabs_save_selected(pp);
            pp->active_tab = NULL;
            for (i = 0; i < pp->nb_tabs; ++i) {
              if (mb->type == MB_RSS) {
                pp->tabs[i].selected =  board_is_rss_feed(pp->tabs[i].site->board) ? 1 : 0;
              } else pp->tabs[i].selected =  board_is_regular_board(pp->tabs[i].site->board) ? 1 : 0;
              //if (mb->type == MB_BOARDS) pp->tabs[i].selected = 1 - pp->tabs[i].selected;
              if (pp->tabs[i].selected && !pp->active_tab) pp->active_tab = pp->tabs+i;
            }
            if (pp->active_tab == NULL) {
              pp->active_tab = pp->tabs; pp->tabs[0].selected = 1;
            }
          }
          pp_tabs_changed(dock);
        } break;
      case PREFS:
        {
          launch_wmccc(dock, NULL);
        } break;
      case CANCEL:
        {
          flag_cancel_task = 1;
        } break;
      default:
        assert(0); 
    }
    ret = 1;
  }
  for (i=0, need_refresh=0; i < NB_MINIB; i++) {
    if (pp->mb[i].clicked) {
      pp->mb[i].clicked = 0; need_refresh++;
    }
  }
  if (need_refresh) { pp_minib_refresh(dock); }
  return ret;
}




/* ---------------------- gestion des fortunes ------------------- */

/* redessine la fortune */
#if 0
void
pp_refresh_fortune(Dock *dock, Drawable d)
{
  Pinnipede *pp = dock->pinnipede;
  Pixmap fpix;

  if (pp->fortune_h > 0) {
    int x;
    assert(pp->fortune_h < 10000); /* tout de meme */

    fpix = XCreatePixmap(dock->display, pp->win, pp->win_width, pp->fortune_h, 
			 DefaultDepth(dock->display,dock->screennum));
    XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(Prefs.pp_fortune_bgcolor));
    XFillRectangle(dock->display, fpix, dock->NormalGC, 0, 0, pp->win_width, pp->fortune_h);

    assert(!picohtml_isempty(pp->ph_fortune));

    x = (pp->win_width - pp->fortune_w)/2;
    picohtml_render(dock, pp->ph_fortune, fpix, dock->NormalGC, x, 0);
    XCopyArea(dock->display, fpix, d, dock->NormalGC, 0, 0, pp->win_width, pp->fortune_h-1, 0, 0);
    XFreePixmap(dock->display, fpix);
    XSetForeground(dock->display, dock->NormalGC, RGB2PIXEL(100,100,100));

    XDrawLine(dock->display, d, dock->NormalGC, 0, pp->fortune_h-1, pp->win_width - (pp->sc ? SC_W : 0), pp->fortune_h-1);
  } else { /* nettoyage ligne du haut */
    assert(LINEY0(0)>0);
    pp_clear_win_area(dock, 0, 0, pp->win_width, pp->zmsg_y1-1);
    pp_clear_win_area(dock, 0, pp->zmsg_y1-1, pp->win_width- (pp->sc ? SC_W : 0), 1);
    /*
    pp_clear_win_area(dock, 0, 0, pp->win_width- (pp->sc ? SC_W : 0), LINEY0(0));
    if (pp->sc) {
      pp_clear_win_area(dock, pp->win_width-SC_W, 0, SC_W, pp->zmsg_y1-2);
    }
    */
  }
}

/* a appeler quand la fortune est changée */
void
pp_update_fortune(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  Site *s;
  int nb_fortunes = 0, fortune_num;
  static char *fortune = NULL;
  static int wmcc_tic_cnt_last_chg = 0;

  if (flag_updating_comments) return;
  if (fortune == NULL || (wmcc_tic_cnt - wmcc_tic_cnt_last_chg) > 45*(1000/WMCC_TIMER_DELAY_MS)) {
    for (s = dock->sites->list; s; s = s->next)
      nb_fortunes += (s->fortune && strlen(s->fortune)) ? 1 : 0;
    if (nb_fortunes > 0) {
      fortune_num = (rand() % nb_fortunes) + 1;
      for (s = dock->sites->list; s; s = s->next) {
	fortune_num -= (s->fortune  && strlen(s->fortune)) ? 1 : 0;
	if (fortune_num == 0) break;
      }
    }
    if (s && s->fortune) {
      //      printf("choosing fortune of %s: '%s'\n", s->prefs->site_name, fortune);
      if (fortune) free(fortune);
      fortune = strdup(s->fortune);
      wmcc_tic_cnt_last_chg = wmcc_tic_cnt;
    }
  }
  if (!picohtml_isempty(pp->ph_fortune)) {
    picohtml_freetxt(pp->ph_fortune);
  }
  pp->fortune_h = 0; /* quand pp->fortune_h != 0 => il y a une fortune à afficher */
  pp->fortune_w = 0;

  if (pp->fortune_mode) {
    if (fortune == NULL) fortune = strdup(_("<folop> I have a 61MB file in /proc, is it normal ?<br>"));
    picohtml_parse(dock, pp->ph_fortune, fortune, pp->win_width - 6);
    picohtml_gettxtextent(pp->ph_fortune, &pp->fortune_w, &pp->fortune_h);
    if (!picohtml_isempty(pp->ph_fortune)) { /* on s'arrête si la fortune est vide (s == "" par ex..)
						(ça peut arriver et ça declenche le assert(!isempty) de refresh_fortune) */
      pp->fortune_h += 3; 
      pp->fortune_h = MIN(pp->fortune_h, pp->win_height/2); /* faut pas exagerer */
    }
  }
}
#endif
static const char *backend_flavour_name(backend_flavour_enum f) {
  switch (f) {
    case BACKEND_FLAVOUR_UNENCODED: return "raw tags";
    case BACKEND_FLAVOUR_ENCODED: return "htmlentitized tags";
    case BACKEND_FLAVOUR_NO_PANTS: return "no pants";
  }
  assert(0); return NULL;
}

void
pp_check_balloons(Dock *dock, int x, int y)
{
  Pinnipede *pp = dock->pinnipede;
  int i;

  if (pp->use_minibar) {
    for (i=0; i < NB_MINIB; i++) {
      char *msg = NULL;
      switch (pp->mb[i].type) {
      case HELP: msg = _("Bring some help"); break;
      case SCROLLBAR: msg = _("Bring/hide the scrollcoin"); break;
        case BALLTRAP: msg = _("Enable/disable the balltrap<br>(note that you can also kill "
                               "all flying ducks with a right clic on the upper part of the dock applet<br>"
                               "You can also use the wmccc to disable duck hunt on some sites (for example"
                               "<font color=blue><tt>woof.lu</tt></font>, since the webserver does not enjoy receiving hundreds of <tt>pan!pan!</tt>"
                               "in the same second)"); break;
      case TRANSPARENT: msg = _("Activate/deactivate the pseudo-transparency (may not work)."); break;
      case UA: msg = _("Change the display mode of the logins/useragents (5 different modes)"); break;
	//      case SECOND: msg = _("Bring/hide the seconds (when there are less than two messages in the same minute)"); break;
	//      case TSCORE: msg = _("Bring/hide the troll score (the numbers on the left of some messages)"); break;
        //case FORTUNE: msg = _("Bring/hide the fortune (if appropriate)"); break;
      case FILTER: msg = _("Activate/deactivate the <b>filter</b>. To filter the messages, do a <font color=#0000ff>Right clic</font> on a word/login/useragent or a clock (to display a thread). You can also use  <font color=#0000ff>CTRL-S</font> to perform incremental search on the board content. To remove the filter, just click here"); break;
      case PLOPIFY: msg = _("Change the plopification type (beware, you will also see the messages in the boitakon!). <p> To plopify a message, <font color=#0000ff>Right click</font> on a word/login/useragent/clock (or the zone on the left of the clock to plopify a thread). To unplopify (or let someone out of the boitakon), just click on the same place OR right clic on this button and you will have a list of boitakoned elements.<br>"); break;
      case MB_RSS: msg = _("show only RSS feeds in the pinnipede"); break;
      case MB_BOARDS: msg = _("show only regular boards in the pinnipede"); break;
      case PREFS: msg = _("Launch wmccc (wmcoincoin configuration)"); break;
/*       case REFRESH_NEWS: msg = _("Click here to force the refresh of the news, messages, fortune and XP"); break; */
/*       case REFRESH_TRIBUNE: msg = _("Click here to force the refresh of the board"); break; */
      case CANCEL: msg = _("clic here to cancel the current download"); break;
      default: assert(0);
      }
      balloon_test(dock, x, y, pp->win_real_xpos, pp->win_real_ypos-15, 0, 
		   pp->mb[i].x, MINIB_Y0, 
		   pp->mb[i].w, MINIB_H, msg);
    }
    for (i=0; i < pp->nb_tabs; ++i) {
      if (balloon_test_nomsg(dock, x, y, 0, 
                             pp->tabs[i].x, pp->tabs[i].y,
                             pp->tabs[i].w, pp->tabs[i].h)) {
        Site *s = pp->tabs[i].site;
        Board *board = pp->tabs[i].site->board;
        char *msg;
        if (board_is_rss_feed(board)) {
          msg = str_printf("RSS Feed: <b>%s</b><br>",
                           board->rss_title ? board->rss_title : s->prefs->site_name);
        } else {
          msg = str_printf("Regular board: <b>%s</b>"
                           "time shift: <font color=blue>%+02d:%02d:%02d</font><br><br>", 
                           s->prefs->site_name,
                           (int)(board->time_shift/3600), 
                           (int)((abs(board->time_shift)/60)%60), 
                           (int)(abs(board->time_shift)%60));
        }
        msg = str_cat_printf(msg, "backend: <font color=blue>%s</font><br>"
                             "   (flavour=<font color=blue>%s</font>, "
                             "quality = <font color=blue>%1.1f</font>, "
                             "ping=<font color=blue>%4.0fms</font>) <br>"
                             "auto_refresh: %s (use ctrl-clic to switch)<br>"
                             "refresh frequency: <font color=blue>%d</font> sec<br>",
                             s->prefs->backend_url, 
                             backend_flavour_name(s->prefs->backend_flavour), 
                             http_stats_site_quality(s), s->http_ping_stat*1000.,
                             board->auto_refresh ? "<font color=blue>yes</font>" : "<font color=red>no</font><br>",
                             board->board_refresh_delay / (1000/WMCC_TIMER_DELAY_MS));
        if (!str_is_empty(s->prefs->post_url)) {
          msg = str_cat_printf(msg, "post: <font color=blue>%s</font> ; <font color=blue>%s</font><br>",
                               s->prefs->post_url, s->prefs->post_template);
        } else msg = str_cat_printf(msg, "this board is <font color=blue>read-only</font><br>");
        if (!str_is_empty(s->prefs->user_cookie)) {
          msg = str_cat_printf(msg, "cookie: <font color=blue>%.20s%s</font><br>", s->prefs->user_cookie, (strlen(s->prefs->user_cookie)>20) ? "(...)" : "");
        }
        balloon_test(dock, x, y, pp->win_real_xpos, pp->win_real_ypos-10, 0, 
                     pp->tabs[i].x, pp->tabs[i].y,
                     pp->tabs[i].w, pp->tabs[i].h, msg);
        free(msg);
      }
    }
  }
}

/* ---------------------------------------- scrollcoin ----------------------- */

/* lecture de la scrollbar, avec un refresh legerement differé pour éviter de trop charger... */
void
pp_check_scroll_pos(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  
  static int refresh_requested = 0;
  static int refresh_nb_delayed = 0;
  int new_pos;
 
  if (pp->sc == NULL) return;
  if (scrollcoin_read_requested_pos(pp->sc, &new_pos)) {
    id_type id;
    /* petit bricolage pas beau pour que ça affiche completement le premier post: 
       update_content rescrollera juste ce qu'il faut */
    if (new_pos == scrollcoin_get_vmin(pp->sc)) new_pos--;

    id = get_nth_id_filtered(boards, &pp->filter, new_pos);
    //    myprintf("scroll pos = %<MAG %d>, --> id_base = %d\n", new_pos, id);
    

    pp_update_content(dock, id, 0,0,0);
    refresh_requested = 2;
  }
  if (refresh_requested == 1 || refresh_nb_delayed >= 0) {
    pp_refresh(dock, pp->win, NULL);
    refresh_nb_delayed = 0; 
    refresh_requested = 0;
  } else if (refresh_requested > 0) {
    refresh_requested--; refresh_nb_delayed++;
  }
}

/* positionnenement de la scrollbar */
/*static void
pp_scrollcoin_move_resize(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  scrollcoin_resize(pp->sc, pp->win_width - SC_W+1, pp->zmsg_y1-1, pp->zmsg_h+2);
}
*/

/* mise a jour de l'existence + position de la scrollcoin
   + mise à jour de pp->zmsg_x2 & pp->zmsg_w */
void pp_scrollcoin_set(Dock *dock, int show_sc) {
  Pinnipede *pp = dock->pinnipede;
  int width =  pp->win_width - (pp->tabs_pos == PPT_RIGHT ? pp->tabs_w : 0);
  if (!show_sc) {
    if (pp->sc) scrollcoin_destroy(pp->sc); pp->sc = NULL;
    pp->zmsg_x2 = width - 1;
  } else {
    pp->zmsg_x2 = width - SC_W;
    if (!pp->sc)  
      pp->sc = scrollcoin_create(1,1,1,pp->zmsg_x2+1, pp->zmsg_y1-1, pp->zmsg_h+2, pp->transparency_mode);
    else scrollcoin_resize(pp->sc, pp->zmsg_x2+1, pp->zmsg_y1-1, pp->zmsg_h+2);
    pp->zmsg_x2 = width - SC_W;
  }
  pp->zmsg_w = pp->zmsg_x2 - pp->zmsg_x1 + 1;
}

/*
  ça tient du vilain bricolage.. mais bon: la scrollbar doit connaitre ses bornes
  le chiffre qu'elle renvoie correspond à l'id du message affiché sur la dernière ligne du pinnipede
  
  le problème est alors la borne vmin: c'est l'id du dernier message de la première page,
  ce qui demande un certain nombre de manip pour le determiner

  (si scroll_coin avait travaillé par ligne plutot que par id, ça aurait été encore pire)
*/

void
pp_scrollcoin_update_bounds(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  int lcnt;
  int vmin, vmax;
  board_msg_info *mi;

  vmax = count_all_id_filtered(boards, &pp->filter);
  pp->non_filtered_message_count = vmax;

  if (pp->sc == NULL) return;
  
  //  myprintf("%<YEL pp_scrollcoin_update_bounds>\n");
  vmin = 0;
  lcnt = 0;

  mi = boards->first;
  while (lcnt < pp->nb_lignes && mi) {
    if (filter_msg_info(mi,&pp->filter)) {
      PostVisual *pv;
      pv = pp_pv_add(pp, boards, mi->id); 
      if (pv) { lcnt += pv->nblig; vmin++; }
    }
    mi = mi->g_next;
  }
  scrollcoin_setbounds(pp->sc, vmin, vmax);
  pp_scrollcoin_set(dock, 1);
}

/* ---------------------- fonctions qui regroupent tout ------------------*/

void
pp_widgets_set_pos(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  pp_minib_set_pos(pp);
  pp_tabs_set_pos(pp);
  pp_scrollcoin_set(dock, pp->sc != NULL);
  if (pp->tabs_pos == PPT_LEFT || pp->tabs_pos == PPT_RIGHT)
    sw_layout_dockapps(dock, pp->tabs_x0, 0, pp->tabs_x0 + pp->tabs_w-1, pp->tabs_y0-1);
  else if (pp->tabs_pos == PPT_DOWN)
    sw_layout_dockapps(dock, -2, -2, 62, 62);
  else sw_layout_dockapps(dock, -2, pp->tabs_y0, 62, pp->tabs_y0+62);
}

void
pp_widgets_refresh(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp->use_minibar) {
    pp_tabs_refresh(dock);
    pp_minib_refresh(dock);
  }
  if (pp->sc) { scrollcoin_refresh(pp->sc, pp->win, 1); }
  sw_redraw(dock);
}

int
pp_widgets_handle_button_press(Dock *dock, XButtonEvent *event)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp->sc && scrollcoin_handle_button_press(pp->sc, event, pp->win))
    return 1;
  else if (pp->use_minibar) {  
    if (pp_minib_handle_button_press(dock,event)) return 1;
    else if (pp_tabs_handle_button_press(dock,event)) return 1;
  }
  return 0;
}

int
pp_widgets_handle_button_release(Dock *dock, XButtonEvent *event)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp->sc && scrollcoin_handle_button_release(pp->sc, event, pp->win)) {
    pp_check_scroll_pos(dock); return 1;
  } else if (pp->use_minibar) {
    if (pp_minib_handle_button_release(dock,event)) { return 1;}
    else if (pp_tabs_handle_button_release(dock,event)) { return 1; }
  }
  return 0;
}

int 
pp_widgets_handle_motion(Dock *dock, XMotionEvent *event)
{
  Pinnipede *pp = dock->pinnipede;
  pp_tabs_handle_motion(dock,event);
  if (pp->sc && scrollcoin_handle_motion(pp->sc, event, pp->win)) {
    pp_check_scroll_pos(dock); return 1;
  } else if (pp_minib_pressed_button(dock)) {
	/* ben oui, on s'en fout */
    return 1;
  } else if (pp_tabs_pressed_tab(dock)) {
    return 1;
  } return 0;
}
