#ifndef _PINNIPEDE_H
#define _PINNIPEDE_H

#include <libintl.h>
#define _(String) gettext (String)

#include "coincoin.h"
#include "board_util.h"
#include "time.h"
#include "scrollcoin.h"
#include "coin_xutil.h"
#include "fontcoincoin.h"

/* chuis con, les bitfields c pas pour les chiens */
#define PWATTR_BD 1
#define PWATTR_IT 2
#define PWATTR_U  4
#define PWATTR_S  8
#define PWATTR_LNK 16
#define PWATTR_TSTAMP 32
//#define PWATTR_UA 64
#define PWATTR_NICK 128
#define PWATTR_REF 256 /* reference vers un autre post */
#define PWATTR_TMP_EMPH 512
#define PWATTR_HAS_INITIAL_SPACE 1024 /* indique si un espace doit etre insere entre ce mot et le
			     suivant (utilise par justif */
#define PWATTR_TROLLSCORE 2048
#define PWATTR_LOGIN 4096
#define PWATTR_TT  8192
#define PWATTR_VISITED  (1<<14)
#define PWATTR_TOTOZ_UNKNOWN (1<<15) /* maaairde le short est plein à ras bord */
#define PWATTR_TOTOZ_DOWNLOADING (1<<16)
#define PWATTR_TOTOZ_NOTFOUND (1<<17)
#define PWATTR_TOTOZ_FOUND (1<<18)
#define PWATTR_TOTOZ (PWATTR_TOTOZ_UNKNOWN|PWATTR_TOTOZ_DOWNLOADING|PWATTR_TOTOZ_NOTFOUND|PWATTR_TOTOZ_FOUND)


#define PP_TOTOZ_STATUS_UNKNOWN 0
#define PP_TOTOZ_STATUS_DOWNLOADING 1
#define PP_TOTOZ_STATUS_NOTFOUND 2
#define PP_TOTOZ_STATUS_FOUND 3

typedef struct _PostVisual PostVisual;
typedef struct _PostWord PostWord;

/* une liste de mots avec leurs attributs */
struct _PostWord {
  unsigned char *w; /* non mallocé, stocke dans la même zone que cette structure */
  unsigned char *attr_s;
  unsigned char *raw; /* non mallocé, stocke dans la même zone que cette structure */
  unsigned int attr;
  short xpos, xwidth; /* boite englobante */
  short xdraw;        /* le x passé a xft */
  short ligne;
  struct _PostWord *next;
  struct _PostVisual *parent;
};

/* liste chainée de posts */
struct _PostVisual {
  id_type id;         // message id + site id
  PostWord *first; /* la liste des mots */
  time_t tstamp;
  signed char sub_tstamp; /* sous numerotation quand plusieurs posts ont le même timestamp */
  int nblig BITFIELD(12); // nombre de lignes necessaire pour afficher ce message
  int ref_cnt BITFIELD(9); // compteur de references

  int is_my_message BITFIELD(1);
  int is_answer_to_me BITFIELD(1);
  int is_hilight_key BITFIELD(4);
  int is_skipped_id BITFIELD(1); /* non nul si le message (id-1) n'existe pas */
  int is_plopified BITFIELD(3);
  /* non nul si le message a été plopifié
			  =1, le message apparait en gris, tags html enleves
			  =2, le message est plopifié (mots remplacés par plop, grouik..)
			  =3, le message est superplopifié (message remplacé par 'plop')
			*/
  struct _PostVisual *next;
};

struct _PinnipedeFilter {
  int filter_mode;
  char *filter_name; /* contient le 'nom' du filtre) 
		      */
  char *ua;
  char *login;
  char *word;
  //  int hms[3]; /* filtre sur les ref au msg posté à l'heure indiquée dans hms */
  id_type *id; int nid; /* liste des id des messages affichés dans le filtre de threads */
  char *anything; /* filtres sur les ua, mots et logins (activé par ctrl-s dans le pinni) */
  int filter_boitakon;
  char visible_sites[MAX_SITES]; /* indique les sites affichés
				    en simultané (maj d'après l'état des tabs du pinni) 
				    (ce filtre est tjs activé, il ne dépend pas de filter_mode)
				 */
};

typedef struct _PinnipedeLignesSel {
  int x0, x1;
  int trashed;
  PostWord *first_pw;
  PostWord *last_pw;
  int first_pw_pos, last_pw_pos;
} PinnipedeLignesSel;

typedef struct _PPMinib {
#define NB_MINIB 11
#define MINIB_H 12
  //#define MINIB_FN_W 6
#define MINIB_Y0 (pp->win_height - MINIB_H)
  enum { HELP, SCROLLBAR, BALLTRAP, /*REFRESH_TRIBUNE, REFRESH_NEWS,*/ UA, /* SECOND, TSCORE, FORTUNE, */ FILTER, PLOPIFY, TRANSPARENT, PREFS, MB_RSS, MB_BOARDS, CANCEL } type;
  int x, y, w, h;
  int clicked;
} PPMinib;
  
#define PPT_BASE_H 12
#define PPT_MAX_H 24 /* hauteur max d'un tab .. pp->lpix est suffisament haut */

/* renvoye par pp_tabs_at_xy */
typedef enum { PPT_MAY_SET_MAIN_TAB, PPT_MAY_UNSELECT_TAB, 
               PPT_NORMAL_ACTION } ppt_survol_actions;

typedef struct _PinnipedeTab {
  Site *site;
  char *site_name; /* utilise pour retrouver le site pendant le rebuild */
  int selected, was_selected;
  int x,y,w,h;
  int clicked;
  int clign_decnt;
} PinnipedeTab;

struct pp_totoz;

struct _Pinnipede {
  Window win;
  CCColorId win_bgcolor[MAX_SITES], timestamp_color[MAX_SITES], useragent_color[MAX_SITES], login_color[MAX_SITES], 
    lnk_color[MAX_SITES], visited_lnk_color[MAX_SITES], txt_color[MAX_SITES], 
    trollscore_color[MAX_SITES], popup_fgcolor, plopify_color, 
    totoz_unknown_color, totoz_downloading_color, totoz_found_color, minib_dark_color,
    emph_color, popup_bgcolor, sel_bgcolor,
    hilight_my_msg_color,hilight_answer_my_msg_color,hilight_keyword_color[NB_PP_KEYWORD_CATEG],    
    minib_color, minib_msgcnt_color, 
    minib_updlcnt_color, progress_bar_color;
  int mapped;
  int win_width, win_height, win_decor_xpos, win_decor_ypos, win_real_xpos, win_real_ypos;

  int zmsg_y1, zmsg_y2, zmsg_h, zmsg_x1, zmsg_x2, zmsg_w; /* zone d'affichage des messages */
  CCFontId fn_base, fn_it, fn_bd, fn_itbd, fn_mono;
  CCFontId fn_minib;
  int fn_base_space_w;
  CCColorId ccc_black;
  int fn_h;
  //  Pixmap minipix;

  PostVisual *pv;

  int nb_lignes;
  PostWord **lignes;

  ScrollCoin *sc;

  PinnipedeLignesSel *lignes_sel; /* utilisé uniquement pendant les selections */
  int sel_anchor_x, sel_anchor_y;
  int sel_head_x, sel_head_y;
  int sel_l0, sel_l1;
  time_t time_sel; /* pour la deselection automatique quand il est *vraiment* temps de rafraichir le pinnipede */

  char *last_selected_text; /* stockage temporaire à usage interne au pinnipede */

  id_type id_base;
  int decal_base;
  id_type last_post_id; /* utilise pour savoir si la tribune a ete mise a jour.. */

  int colle_en_bas; /* pour savoir si on scrolle lors de nouveaux messages */

  int flag_pp_update_request; /* si non nul, on fait pp_pv_destroy + pp_update_content(last_id_filtered) + pp_refresh
                                 des que possible */

  int flag_pp_refresh_request;

  int non_filtered_message_count; /* ~= count_all_id_filtered(boards, &pp->filter)
                                     maj par pp_scrollcoin_update_bounds (meme qd le scrollcoin est caché),
                                     ça n'est pas un truc fiable a 100% */

  id_type kbnav_current_id;
  int kbnav_current_tstamp; /* navigation au clavier : indique le message actuellement selectionné, et le numéro de l'horloge dans ce message qui est actuellement activée */

  //  int html_mode;
  int nick_mode; /* 0 : n'affiche rien, 
		    1:  affiche les useragent raccourcis, 
		    2: affiche les logins, 
		    3: affiche les deux, 
		    4: affiche useragent ou login */
  int show_sec_mode; /* supprime les secondes sur les posts ou c'est possible */
  int trollscore_mode;
  int survol_hash; /* pour determiner (a peu pres) si on affcihe une nouvelle info de survol... (apprixmatif...) */
  int disable_plopify;

  int hilight_my_message_mode;
  int hilight_answer_to_me_mode;
  int hilight_key_mode;

  
  volatile int flag_board_updated;
  int lpix_h0; /* hauteur de lpix reservee pour les operations diverses (en
                  general c'est la taille totale de lpix, sauf si un bg_pixmap
                  est utilisé et il est alors sauve en dessous) 
                  
                  ah ben merde c'etait juste marque en dessous..
               */
  Pixmap lpix; /* stocke aussi le bg_pixmap (après les lpix_h0 premieres
                  lignes) */

  struct _PinnipedeFilter filter;

  int transparency_mode;
  //  int selection_mode; /* non nul quand on est en train de selectionner du texte à copier dans le clipboard (en dragant avec la souris) */

  int use_minibar; /* les miniboutons sont-ils affichés ? */
  PPMinib mb[NB_MINIB];
  int minib_pressed; /* numero du minibouton enfonce, -1 si aucun */
  int mb_min_width; /* largeur minimale demandée par la minibar
		       (permet aux tabs de se placer avec la minibar, ou au-dessus) */
  int mb_buttonbar_width; /* largeur du bloc de boutons */
  int mb_x0;  /* position x de la minibar (non nul qd les tabs sont au même niveau, a gauche) */

  enum {PPT_UP, PPT_DOWN, PPT_LEFT, PPT_RIGHT} tabs_pos; /* position des tabs */
  int nb_tabs; /* == nb de sites avec une tribune */
  PinnipedeTab *tabs; /* tableau de tabs (une par site avec tribune) */
  PinnipedeTab *active_tab;  
  PinnipedeTab *survol_tab; ppt_survol_actions survol_tab_part;
  int tabs_x0, tabs_y0, tabs_w, tabs_h;

#define MAX_VISITED_LINKS 200
  int visited_links[MAX_VISITED_LINKS]; /* hash des urls deja visitées */
  int nb_visited_links;

  struct pp_totoz *totoz;  
  Pixmap miniduck_pixmap;
};

void ccfont_draw_string_utf8(CCFontId fid, CCColorId cid, Drawable d, int x, int y, char *str, int len);
int ccfont_text_width_utf8(CCFontId fid, char *str, int len);
int ccfont_text_xbox_utf8(CCFontId fid, char *str, int len, short *pxstart, short *pxoff);
int filter_msg_info(const board_msg_info *mi, const struct _PinnipedeFilter *filter);
int count_all_id_filtered(Boards *boards, struct _PinnipedeFilter *filter);
id_type get_first_id_filtered(Boards *boards, struct _PinnipedeFilter *filter);
id_type get_next_id_filtered(Boards *boards, id_type id, board_msg_info **nmi, struct _PinnipedeFilter *filter);
id_type get_nth_id_filtered(Boards *boards, struct _PinnipedeFilter *filter, int n);
id_type get_last_id_filtered(Boards *boards, struct _PinnipedeFilter *filter);
void pp_selection_unselect(Pinnipede *pp);
void pp_balloon_help(Dock *dock, int x, int y);
void pp_pv_destroy(Pinnipede *pp);
PostVisual *pp_pv_add(Pinnipede *pp, Boards *boards, id_type id);
void pp_refresh(Dock *dock, Drawable d, PostWord *pw_ref);
void pp_popup_show_txt(Dock *dock, unsigned char *txt);
void pp_change_transparency_mode(Dock *dock, int on_off);
void pp_update_bg_pixmap(Dock *dock);
unsigned long pp_get_win_bgcolor(Dock *dock);
void pp_clear_win_area(Dock *dock, int x, int y, int w, int h);
//void pp_minib_initialize(Pinnipede *pp);
void pp_minib_refresh(Dock *dock);
void pp_minib_show(Dock *dock);
void pp_minib_hide(Dock *dock);
void pp_tabs_build(Dock *dock);
void pp_tabs_rebuild(Dock *dock);
//void pp_tabs_set_position(Pinnipede *pp);
void pp_tabs_destroy(Pinnipede *pp);
void pp_tabs_save_state(Dock *dock, FILE *f);
void pp_tabs_restore_state(Dock *dock, FILE *f);
void pp_tabs_refresh(Dock *dock);
void pp_tabs_set_visible_sites(Pinnipede *pp);
void pp_tabs_cliquouille(Pinnipede *pp, PinnipedeTab *pt, ppt_survol_actions where);
void pp_tabs_switch_all_selected(Pinnipede *pp);
void pp_tabs_changed(Dock *dock);
void pp_change_active_tab(Dock *dock, int dir);
void pp_scrollcoin_update_bounds(Dock *dock);
void pp_scrollcoin_set(Dock *dock, int show_sc);
void pp_widgets_set_pos(Dock *dock);
void pp_widgets_refresh(Dock *dock);
int  pp_widgets_handle_button_press(Dock *dock, XButtonEvent *ev);
int  pp_widgets_handle_button_release(Dock *dock, XButtonEvent *event);
int  pp_widgets_handle_motion(Dock *dock, XMotionEvent *event);

void pp_update_content(Dock *dock, id_type id_base, int decal, int adjust, int update_scrollbar_bounds);
void pp_update_and_redraw(Dock *dock, id_type id_base, int decal, int adjust, int update_scrollbar_bounds);

int pp_boardshot_kikoooo(Dock *dock, int save_all, int overwrite, int use_js);

/* quelques fonctions de pp_totoz sont declarees dans coincoin.h */
void pp_totoz_build(Dock *dock);
void pp_totoz_rebuild(Dock *dock);
void pp_totoz_destroy(Dock *dock);
void pp_totoz_unmap(Dock *dock);
int pp_totoz_img_status(Pinnipede *pp, char *imgname);
//int pp_totoz_update_status_all(Dock *dock);
void pp_check_totoz(Dock *dock, PostWord *pw, int x_root, int y_root);
int  pp_totoz_dispatch_event(Dock *dock, XEvent *event);
void pp_totoz_download(Dock *dock, unsigned char *imgname);
//void pp_totoz_get_image(Dock *dock, unsigned char *imgname);
/* macros pour le calcul des differentes positions d'affichage des lignes */
#define LINEY0(l) (pp->zmsg_y2 - (pp->nb_lignes-l)*pp->fn_h-(pp->zmsg_h - pp->nb_lignes*pp->fn_h)/2)
#define LINEY1(l) (LINEY0(l)+pp->fn_h-1)
#define LINEBASE(l) (LINEY0(l) + pp->fn_base->ascent)

#endif
