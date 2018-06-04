#ifndef COINCOIN_H
#define COINCOIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <regex.h>
#include <ctype.h>
#include "http.h"
#include "global.h"
#include "raster.h"
#include "coin_util.h"
#include "myprintf.h"
#include "fontcoincoin.h"
#ifndef WMCCDATADIR
#define WMCCDATADIR "."
#endif

/* tentative de compilation avec _XOPEN_SOURCE, y'a plein de warnings */
#ifdef _XOPEN_SOURCE
#ifndef _GNU_SOURCE
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
char *strdup(const char *s);
int snprintf(char *str, size_t size, const char *format, ...);
void usleep(unsigned long usec);
#endif
#endif

#define APPNAME "wmcoincoin"

typedef struct _PicoHtmlItem PicoHtmlItem;
typedef struct _PicoHtml PicoHtml;

typedef long long int64;
typedef unsigned long long uint64;

typedef struct Board_ Board;
typedef struct Boards_ Boards;
typedef struct Site_ Site;
typedef struct SiteList_ SiteList;

#define trollo_log_extent 5 /* minutes */

/* #define TRIBUNE_MAX_MSG 400 */ /* nb max de messages gardés en mémoire */

/* la tribune load affiche les messages sur les 15 dernieres minutes */
#define TROLLOSCOPE_NB_MINUTES 11
/* sur une minute, elle peut stocker 5 messages simultanes */
#define TROLLOSCOPE_MAX_SIMUL 5

#define TROLLOSCOPE_WIDTH TROLLOSCOPE_NB_MINUTES*5
/* le -2 c fait expres -> quand il y a + de 5 msg dans
   la meme minute, on voit le 5eme sans pouvoir l'identifier vraiment
   c mieux que de prendre une hauteur de exactement 4 formes */
#define TROLLOSCOPE_HEIGHT TROLLOSCOPE_MAX_SIMUL*5-4
#define TROLLOSCOPE_X 4
#define TROLLOSCOPE_Y 25


#define MINIUA_SZ 20
typedef struct _MiniUA {
  unsigned char R,G,B;
  unsigned char symb; /* numero du symbole */
  char name[MINIUA_SZ];
} MiniUA;

typedef struct _board_msg_info board_msg_info;

typedef struct _board_msg_ref {
  unsigned int h:5;
  unsigned int m:8;
  int s:9;    /* si positif, les secondes sont indiquées dans la ref */
  int num:5;  /* si positif, le sous-numéro (pour les post multiples dans une même seconde) est indiqué dans la ref */
  unsigned int nbmi:6; /* nb de messages consecutifs pointés (généralement 1 sauf si la ref désigne plusieurs messages, forcement consecutifs)
			  0 => ref non determinee (ipot, mauvaise reference, le message n'existe plus..)
			  
			  _ATTENTION_: quand on boucle sur cette serie de message, il faut
			  utiliser le pointeur 'intra-site' mi->next et pas 
			  le pointeur 'cross-sites' mi->g_next
		       */
  board_msg_info *mi; /* non mallocé, forcément, et mis à jour lors de la destruction des messages trop vieux 
			  peut être NULL (ipot, message effacé..)

			  remarque: les refs DOIVENT être vers des messages du passe (ipot interdit)
			  c'est necessaire au bon fonctionnement de la construction des threads du pinnipede
			*/
} board_msg_ref;

/* liste de message ID qui peuvent etre identifiés par une url
   (en gros les messages provenant de rss) 
   ne jamais deplacer les instances de cette structure, les board_msg_info pointent dessus
*/
typedef struct RSSBonusInfo_ {
  md5_byte_t linkmd5[16]; /* c'est la mode du md5 j'en met partout */
  char *link; /* l'url en question */
  id_type id;
  md5_byte_t md5[16]; /* pour une "regular board", le md5 est calculé sur la concatenation de
                           stimestamp + useragent + login + msg
                         pour un feed rss, il est la concat de
                           title + link + description
                      */  
} RSSBonusInfo;


/* ne pas déplacer ce genre de structure après son allocation
   --> pas de realloc la dessus !
   (à cause des board_msg_ref qui les relient entre elles)
*/
struct _board_msg_info {
  id_type id;
  time_t timestamp;
  signed char sub_timestamp; /* sous numerotation quand plusieurs posts ont le même timestamp 
			 (-1 -> pas (encore) d'autre post avec le meme tstamp)
		       */
  RSSBonusInfo *ri; /* non-nul si le message provient d'un rss */
  /* (year-2000)|month|day|h|m|s */
  signed char hmsf[4]; /* heure, minute, seconde + flag d'affichage des secondes (1 == secondes necessaires)  */
  char *useragent; /* pointe dans la zone mémoire allouée pour board_msg_info -> ne pas faire de free(useragent) !!! */
  char *msg; /* pointe dans la zone mémoire allouée pour board_msg_info -> ne pas faire de free(msg) !!! */
  char *login; /* non mallocé, comme useragent, msg etc..*/
  struct _board_msg_info *next;

  /* on a aussi une structure d'arbre qui se colle là dessus
     (la structure de liste triée reste pour des raisons historiques ..) */
  struct _board_msg_info *left;
  struct _board_msg_info *right;

  /*
    describes how the message will appear in the trolloscope 
     also contains a hopefully useful short name describing the useragent
  */
  MiniUA miniua;

  /* le niveau de trollitude du post (cf troll_detector.c) */
  int troll_score BITFIELD(13); 
  int is_my_message BITFIELD(1);
  int is_answer_to_me BITFIELD(1);

  /* utilisé par board_key_list_test_thread et board_key_list_get_hungry_bak_list pour éviter de récurser comme un ouf */
  int bidouille_qui_pue BITFIELD(1); 
  int in_boitakon BITFIELD(1); /* le niveau ultime de la plopification */
  int contagious_boitakon BITFIELD(1); /* repondre à la boitakon rend kon */
  short nb_refs BITFIELD(14);
  board_msg_ref *refs; /* pointeur mallocé, indique la liste des messages pointés par celui ci */

  /* pointeurs inter-sites: le point de depart est dans la structure boards,
     et ils permettent de parcourir les messages de tous les sites, dans l'ordre */
  struct _board_msg_info *g_next, *g_prev;
};

typedef struct md5_and_time_ {
  char tstamp[15];
  md5_byte_t md5[16];
  int viewed;
  struct md5_and_time_ *next;
} md5_and_time;

struct Board_ {
  unsigned char last_post_time[5];

  int last_post_timestamp; /* en secondes */
  int64_t last_post_id;
  int64_t last_post_id_prev;

  /* log des 'last_post_id' au cours des 'nb_trollo_log' derniers check de la board,
     utilise pour calculer le nb moyen de messages postes / seconde */
  board_msg_info *msg;

  /* valeur du wmcc_tic_cnt lors du dernier check positif */
  int wmcc_tic_cnt_last_check;

  /* nombre de secondes ecoulees depuis que le dernier message a ete recu */
  int nbsec_since_last_msg;

  int nb_msg_at_last_check; /* ne compte pas les posts boitakonnés */

  /* le dernier id qui a ete affiche sur la tribune, 
     le nb de messages (non vus, donc) qui ont ete recus depuis */
  int64_t last_viewed_id;
  int nb_msg_since_last_viewed;

  /* date a laquelle le dernier check a ete fait
     (c'est pas redondant, je part du principe que l'horloge locale
     et celle de linuxfr ne sont pas synchronisees */
  time_t local_time_last_check, local_time_last_check_old, local_time_last_check_end;

  /* fourchette d'écart de temps */
  time_t time_shift_min, time_shift_max;

  time_t time_shift; /* time@localhost - time@remotehost */

  /* regles de reconnaissance des useragent (par regex)
     ce qui permet de leur assigner des couleurs/formes differentes */
  //  board_load_rule *rules;

  int just_posted_anonymous; /* positionné si on vient juste d'envoyer un message en anonyme
				(pour aider la reconnaissance de nos messages) */
  
  board_msg_info *mi_tree_root; /* rooh un arbre binaire ..
				     c'est utilisé par board_find_id */
  char *last_modified; /* pour les requetes http */
  struct Site_ *site;
  Boards *boards; /* pointeur vers la structure multi-site 
		     il est juste là par commodité
		   */
  int flag_answer_to_me;

  /* stocke le useragent (potentiellement modifié) */
  unsigned char coin_coin_useragent[USERAGENT_MAXMAX_LEN+1];

  /* compteurs mis à jour dans Net_loop (25 fois/sec) */
  int board_refresh_decnt, board_refresh_delay;

  volatile int auto_refresh; /* refreshs auto activé desactivé par la ptite croix en bas à droite du tab */

  md5_and_time *oldmd5; /* utilise de maniere transitoire par les feeds rss */

  char *rss_title; /* nul sur les boards non rss */

  char *encoding; /* encoding utilisé pour les POST. Déduit de l'encoding du backend */

  int64_t last_posted_id; /* id du dernier post qu'on a posté, récupéré du header X-Post-Id */
};

typedef struct SiteNameHash_ {
  int hash;
  int sid;
} SiteNameHash;

struct Boards_ {
  board_msg_info *first;
  board_msg_info *last;
  Board *btab[MAX_SITES];
  int nb_rss_e, max_rss_e;
  RSSBonusInfo **rss_e;
  int nb_aliases;
  SiteNameHash *aliases;
};


struct Site_ {
  Board *board;
  SitePrefs *prefs;
  struct Site_ *next;

  int site_id; /*
		 un numéro unique au site,
		  susceptible de changer a chaque rechargement des prefs
		  (ajout ou suppression de sites)
		  (ce numero est compris entre 0 et MAX_SITES-1, et represente l'indice
		  du pointeur 'prefs' dans la structure globale Prefs.site[] )
	       */
  int http_error_cnt;    /* increment quand il y a eu une erreur http sur le site */
  int http_success_cnt;  /* et inversement */
  int http_recent_error_cnt; /* incrementé à chaque erreur, et remis à zero à chaque
				succes */

  /* bien sûr c'est pas des vrai pings, c'est le temps pour faire un connect
     envoyer une requete http et recevoir l'entente de la reponse */
#define NB_HTTP_PING_STAT (42/2)
  /* c'est un buffer tournant je sais pas comment on dit */
  float http_ping_stat_buf[NB_HTTP_PING_STAT];
  int http_ping_stat_i;
  float http_ping_stat; /* moyenne judicieusement pondérée */
  char *relative_urls_base;
};

struct SiteList_ {
  Site *list; /* liste de sites, reordonnees lors de la relecture des prefs */
  Boards *boards;
};

/* le docker qui est dans le pinni qui sert à docker le dock, tout simplement 
   inspiré par le concept des autofuck que j'avais vu à la telé dans ma jeunesse
 */
typedef struct SwallowedApp {
  Window win;
  int winx, winy;
  int winw, winh;
  int framex, framey;
  int framew, frameh;
  char *name;
  struct SwallowedApp *next;
} SwallowedApp;

typedef struct SwallowCoincoin {
  Window hostwin;
  int x0, x1, y0, y1; /* position de la zone d'avalage dans hostwin */
  int nb_swallowed;
  int layout_dirty;
  SwallowedApp *self;
  SwallowedApp *apps;
} SwallowCoincoin;

typedef struct TotozBookmarkItem_ {
  char *name;
  int popularity;
  struct TotozBookmarkItem_ *next;
} TotozBookmarkItem;

typedef struct TotozBookmark_ {
  time_t last_modif;
  TotozBookmarkItem *first;
} TotozBookmark;

typedef enum {OFF=0, BLUE=1, GREENLIGHT=2, YELLOW=3, VIOLET=4, CYAN=5, RED=6, GREEN=7,BIGREDLIGHT=7, BIGRED=8} LedColor;

typedef struct Led_ {
  LedColor coul[3];
  int delay[3];
  
  int xpos, ypos;

  int cnt, colcnt;
  
} Led;

typedef struct Leds {
  Led led[5];
  Pixmap pixmap, mask;
} Leds;



#define DOCK_WIN(d) ((Prefs.use_iconwin && !Prefs.auto_swallow ? (d)->iconwin : (d)->win))


typedef struct TL_item_ {
  id_type id;
  unsigned char R,G,B,symb;
} TL_item;

typedef struct _Balloon Balloon;
typedef struct _EditW EditW;
typedef struct _MsgBox MsgBox;
typedef struct _Pinnipede Pinnipede;
typedef struct _Plopup Plopup; 
typedef void(*plopup_callback_t)(int);

#define DOCK_FIXED_FONT "-*-courier-*--10-*"
#define DOCK_FIXED_FONT_W 6


#define MAX_NEWSTITLES_LEN 512 /* taille de dock->newstitles */
#define MAX_MSGINFO_LEN 300 /* taille de dock->msginfo */

/* periode du clignotement (25-> 1 par sec) */
#define FLAMOMETRE_COMMENT_CLIGN_SPEED 32
#define FLAMOMETRE_XP_CLIGN_SPEED      32
#define FLAMOMETRE_TRIB_CLIGN_SPEED    16
/* duree de clignotement (en secondes) */
#define FLAMOMETRE_COMMENT_DUREE 3600000  /* c'est long, très long */
#define FLAMOMETRE_XP_DUREE      900      /* 1/4 d'heure */
#define FLAMOMETRE_TRIB_DUREE    15

typedef struct _Duck Duck;

typedef struct Dock_ {
  Pixmap pix_porte, mask_porte_haut, mask_porte_bas;
  Leds leds;
  /* le pixmap du load de la board (il n'est regenere que
     quand la tribune a ete modifie, c'est quand meme plus cool
     que le faire a chaque refresh_dock */
  Pixmap pix_trolloscope;

  XFontStruct *fixed_font;
  
  SiteList *sites; /* toutes les données !! */

  /*
    le message defilant en haut 
  */
  int nb_newstitles;
  unsigned char *newstitles;
  int newstitles_pos, newstitles_char_dec;

  /* ouverture,fermeture,enfoncage de bouton..*/
  enum {OPENING, CLOSING, OPENED, CLOSED} door_state;
  int door_state_step;

  /* press_flag : 0: pas presse, 
     +1 en cours d'enfoncements, -1 en cours de desenfoncement
     press_state: niveau d'enfoncement (max=5)
  */
  int red_button_press_flag, red_button_press_state;
  int red_button_send_cnt;
  int post_anonyme;

  /* le message a caractere informatif en bas */
  unsigned char *msginfo;

  /* pointe vers la structure tl_item si la souris
     est au dessus d'un message dans la zone de trolloscope
     (NULL sinon)
  */
  TL_item *tl_item_survol;
  int tl_item_clicked;

  /* ça c'est le tableau du trolloscope */
  TL_item **trolloscope;
  /* trolloscope_resolution: 5(faible) ou 2(moyenne) ou 1(hires!)*/
  int trolloscope_resolution; /* de retour, à la demande de monsieur 'The Original Palmipède' */
  
  struct {
    int xp_change_decnt;
    int comment_change_decnt;
    int board_answer_decnt;
  } flamometre;

  /* si non nul, c'est le compteur de defilement*/
  int msginfo_defil;

  int flag_survol_trollo; /* qd la souris est audessus du trollometre on affiche sa valeur */
  int flag_survol_led1; /* qd la souris est audessus de la led1, on affiche la vitesse de defilement du trolloscope */

  /* position du pointeur & comptage du nombre de millisecondes depuis le dernier mvt */
  int mouse_x, mouse_y, mouse_cnt;
  Window mouse_win;

  /* InputMethod -> pour gerer les carac. acceptues sur 
     clavier qwerty, par exemple.. */
  XIM input_method;

  int fuck_utf8; /* non nul si la locale est utf */

  Atom atom_WM_DELETE_WINDOW; /* oh les bon gros atomes  */
  Atom atom_WM_SAVE_YOURSELF;
  Atom atom_WM_TAKE_FOCUS;
  Atom atom_WM_PROTOCOLS;
  /* plein d'autres atome dans coin_xutil.c */


  /* si non nul, on voit l'id du msg designe par tl_item_clicked, 
     --> active par un click sur un symbole dans trolloscope 
    (contient l'id, ce n'est pas un flag 0/1) */
  id_type view_id_in_newstitles;
  int view_id_timer_cnt;

  unsigned char coin_coin_message[MESSAGE_MAXMAX_LEN+1];
  int coin_coin_site_id;
  int coin_coin_sent_decnt;

  Cursor trib_load_cursor;
  int flag_trib_load_cursor;

  GC NormalGC, or_GC, and_GC;
  Display *display;

  Window rootwin;

  Window iconwin,win;

  Window msgwin;

  Window pp_win; /* == pinnipede->win quand le pinnipede est mappe */
  Pixmap pix_msgwin;
  int msgwin_visible;
  GC msgwin_GC;

  /* les trois couleurs de base du dock */
  unsigned long bg_pixel,light_pixel,dark_pixel;
  CCColorId bg_color, light_color, dark_color;
  CCColorId white_color, black_color, green_color, blue_color, red_color, gray_colors[16];
  int screennum;
  Pixmap coinpix, coin_pixmask;
  Pixmap clockpix, clock_pixmask;
  Pixmap led, month, date, weekday;

  RGBAContext *rgba_context;

  Balloon *balloon;

  EditW *editw;

  Pinnipede *pinnipede;

  MsgBox *msgbox;

  Plopup *plopup;

  int trolloscope_speed; /* vitesse de defilement du trolloscope (1,2,4 ou 8), defaut:2 */

  struct {
    int palmipede_used;
    int pinnipede_used;

    int last_sig_is_usr1;
  } discretion_saved_state;

  int horloge_mode;

  float trib_trollo_rate, trib_trollo_score;

  Pixmap wm_icon_pix, wm_icon_mask; /* icone utilisée par le windowmanager (pour le pinnipede) */
  pid_t wmccc_pid;

  /* multi-head support */
  struct {
    int   screen_number;
    short x_org;
    short y_org;
    short width;
    short height;
  } *xiscreen;
  int nb_xiscreen;

  SwallowCoincoin *swallow;

  TotozBookmark *totoz_bm;
  int nb_duck;
  Duck *duck_lst;
} Dock;

/* c'est classé par ordre de priorité décroissante */
typedef enum { Q_PREFS_UPDATE, Q_BOARD_POST, Q_BOARD_UPDATE, 
	       Q_NEWSLST_UPDATE, Q_SMILEY_DL } ccqueue_elt_type;

typedef struct _ccqueue_elt {
  ccqueue_elt_type what;
  int  sid;

  char *ua;
  char *msg;
  int  nid;
  struct _ccqueue_elt *next;
} ccqueue_elt;


/* wmcoincoin.c */
#define URL_YES_I_KNOW_WHAT_I_DO 16384
void open_url(const unsigned char *url, int balloon_x, int balloon_y, int browser_num);
void wmcc_init_http_request(HttpRequest *r, SitePrefs *sp, char *url_path);
void wmcc_init_http_request_with_cookie(HttpRequest *r, SitePrefs *sp, char *url_path);
void wmcc_log_http_request(Site *s, HttpRequest *r);
void block_sigalrm(int bloque);
int launch_wmccc(Dock *dock,...);
void exec_coin_coin(Dock *dock, int sid, const char *ua, const char *msg);
void wmcc_save_or_restore_state(Dock *dock, int do_restore);
Dock *get_dock();

/* picohtml.c */
void picohtml_set_url_path(PicoHtml *ph, const char *s);
void picohtml_unset_url_path(PicoHtml *ph);
void picohtml_parse(PicoHtml *ph, const char *buff, int width);
void picohtml_gettxtextent(PicoHtml *ph, int *width, int *height);
void picohtml_render(PicoHtml *ph, Drawable d, int x, int y);
void picohtml_freetxt(PicoHtml *ph);
int  picohtml_isempty(PicoHtml *ph);
void picohtml_set_parag_indent(PicoHtml *ph, int parag_indent);
void picohtml_set_parag_skip(PicoHtml *ph, float parag_skip);
void picohtml_set_line_skip(PicoHtml *ph, float line_skip);
void picohtml_set_tabul_skip(PicoHtml *ph, int tabul_skip);
CCFontId picohtml_get_fn_base(PicoHtml *ph);
CCFontId picohtml_get_fn_bold(PicoHtml *ph);
PicoHtml *picohtml_create(Dock *dock, char *base_family, int base_size, int white_txt);
void picohtml_destroy(PicoHtml *ph);
void picohtml_set_default_pixel_color(PicoHtml *ph, unsigned long pix);

/* cc_queue.c */
void ccqueue_build();
void ccqueue_push_prefs_update(int whatfile);
void ccqueue_push_board_post(int sid, char *ua, char *msg);
void ccqueue_push_board_update(int sid);
void ccqueue_push_comments_update(int sid);
void ccqueue_push_messages_update(int sid);
void ccqueue_push_newslst_update(int sid);
void ccqueue_push_smiley_dl(char *imgname);
int ccqueue_state();
void ccqueue_print();
const ccqueue_elt *ccqueue_doing_what();
ccqueue_elt* ccqueue_find_next(ccqueue_elt_type what, int sid, ccqueue_elt *q);
ccqueue_elt* ccqueue_find(ccqueue_elt_type what, int sid);
void ccqueue_loop(Dock *dock);

/* palmipede.c  */
void editw_show(Dock *dock, SitePrefs *sp, int useragent_mode);
void editw_hide(Dock *dock, EditW *ew); /* rentrer le palmipede */
void editw_unmap(Dock *dock, EditW *ew); /* cacher immediatement le palmipede */
void editw_reload_colors(Dock *dock, EditW *ew);
void editw_build(Dock *dock);
void editw_rebuild(Dock *dock);
void editw_set_kbfocus(Dock *dock, EditW *ew, int get_it);
int editw_handle_keypress(Dock *dock, EditW *ew, XEvent *event);
void editw_handle_keyrelease(Dock *dock, EditW *ew, XEvent *event);
void editw_dispatch_event(Dock *dock, EditW *ew, XEvent *event);
Window editw_get_win(EditW *ew);
int editw_get_site_id(Dock *dock);
void editw_change_current_site(Dock *dock, int sid);
void editw_action(Dock *dock, EditW *ew);
int editw_ismapped(EditW *ew);
void editw_balloon_test(Dock *dock, EditW *ew, int x, int y);
void editw_refresh(Dock *dock, EditW *ew);
void editw_erase(EditW *ew);
int editw_insert_string(EditW *ew, const unsigned char *s); /* utilise par le pinnipede */
void editw_move_end_of_line(EditW *ew, int shift_move); /* utilise par le pinnipede */
int editw_check_corse(Dock *dock, unsigned keycode); /* utilise par le pinnipede */
void editw_check_bloub(Dock *dock);

/* deux fonction d'utilite generale, en depis de leur nom */
void editw_cb_copy(Dock *dock, Window win, const char *text, int len);
void editw_cb_handle_selectionrequest(Dock *dock, XSelectionRequestEvent *rq);



/* balloon.c */
void balloon_build(Dock *dock);
void balloon_destroy(Dock *dock);
void balloon_hide(Dock *dock);
void balloon_show(Dock *dock, int x, int y, int h, int w, const char *text, int bwidth);
void balloon_show_with_image(Dock *dock, int x, int y, int h, int w, const char *text, int bwidth, Pixmap image, int img_w, int img_h);
int balloon_ismapped(Dock *dock);
void balloon_check_event(Dock *dock, XEvent *event);
void balloon_disable_key(Dock *dock, unsigned keycode);
int balloon_test_nomsg(Dock *dock, int x, int y, int bcnt, int bx, int by, int bw, int bh);
int balloon_test(Dock *dock, int x, int y, int win_xpos, int win_ypos, int bcnt, int bx, int by, int bw, int bh, const char *btxt);
int balloon_test_with_image(Dock *dock, int x, int y, int winx, int winy, int bcnt, int bx, int by, int bw, int bh, const char *btxt, Pixmap image, int img_w, int img_h);

/* msgbox.c */
void msgbox_dispatch_event(Dock *dock, XEvent *event);
Window msgbox_get_win(Dock *dock);
int msgbox_ismapped(Dock *dock);
void msgbox_hide(Dock *dock);
void msgbox_show(Dock *dock, char *text);
void msgbox_show_modal(Dock *dock, char *text);
void msgbox_build(Dock *dock);

/* plopup.c */
void plopup_build(Dock *dock);
void plopup_set_description(Dock *dock, char *txt);
void plopup_pushentry(Dock *dock, char *txt, int id);
void plopup_pushsepar(Dock *dock);
void plopup_show(Dock *dock, int x, int y, plopup_callback_t cback);
int  plopup_show_modal(Dock *dock, int x, int y);
int  plopup_ismapped(Dock *dock);
void plopup_unmap(Dock *dock);
void plopup_dispatch_event(Dock *dock, XEvent *event);

/* pinnipede.c */
void pp_build(Dock *dock);
void pp_rebuild(Dock *dock, int destroy_tabs);
void pp_destroy(Dock *dock);
void pp_show(Dock *dock);
void pp_unmap(Dock *dock);
int pp_ismapped(Dock *dock);
int pp_handle_keypress(Dock *dock, XEvent *event);
int pp_handle_keyrelease(Dock *dock, XEvent *event);
int pp_dispatch_event(Dock *dock, XEvent *event);
void pp_minib_dispatch_event(Dock *dock, Board *trib, XEvent *event);
Window pp_get_win(Dock *dock);
void pp_check_board_updated(Dock *dock);
void pp_animate(Dock *dock);
void pp_set_board_updated(Dock *dock);
void pp_check_balloons(Dock *dock, int x, int y);
void pp_change_filter_mode(Dock *dock, int zero_or_one);
int pp_get_filter_mode();
void pp_set_ua_filter(Dock *dock, char *ua);
void pp_set_word_filter(Dock *dock, char *word);
void pp_show_message_from_id(Dock *dock, id_type id);
void pp_save_state(Dock *dock, FILE *f);
void pp_restore_state(Dock *dock, FILE *f);
Site *pp_tabs_get_main_site(Dock *dock);
int pp_tabs_is_site_visible(Dock *dock, Site *s);
void pp_tabs_set_flag_answer_to_me(Dock *dock, Site *s);
void pp_set_download_info(char *site, char *what);
void pp_unset_kbnav(Dock *dock);
void pp_totoz_check_updates(Dock *dock);
char *pp_totoz_realfname(unsigned char *name, int with_path);
void pp_totoz_get_image(Dock *dock, unsigned char *imgname);
/* prefs_gestion.c */
char *check_install_data_file(char *data_file_name, char *dot_wmcc_file_name);
void wmcc_prefs_initialize(int argc, char **argv, GeneralPrefs *p);
void wmcc_prefs_relecture(Dock *dock, int whichfile);
char *get_wmcc_options_filename();
char *get_wmcc_tmp_options_filename();

/* troll_detector.c */
void troll_detector(board_msg_info *mi);


/* board.c */
int logged_urls_find_url(unsigned char *url, id_type *pid);
Board *board_create(Site *site, Boards *boards);
void board_tatouage(Board *trib, board_msg_info *it);
/* renvoie l'age du message, en secondes */
time_t board_get_msg_age(const Board *trib, const board_msg_info *it);
void board_frequentation(const Board *trib, int nb_minutes, int *ua_cnt, int *msg_cnt, int *my_msg_cnt);
void board_get_trollo_rate(const Board *trib, float *trate, float *tscore);
void boards_update_boitakon(Boards *boards);
void board_update(Board* board);
void board_destroy(Board *board);
void board_save_state(FILE *f, Board *board);
void board_restore_state(FILE *f, Board *board);
void board_set_viewed(Board *board, int64_t id);
int board_is_rss_feed(Board *b);
int board_is_regular_board(Board *b);
int board_can_post_messages(Board *b);
RSSBonusInfo *rss_find_from_link(Boards *boards, char *link);
RSSBonusInfo *rss_find_from_id(Boards *boards, id_type id); /* slow */

/* coincoinswallow.c */
void swallower_init(Dock *dock);
int swallower_is_autoswallowed(Dock *dock);
void sw_layout_dockapps(Dock *dock, int x0, int y0, int x1, int y1);
void sw_redraw(Dock *dock);
void swallower_autoswallow(Dock *dock);
/* totoz_bookmark.c */
const char *totoz_bookmark_filename();
const char *totoz_bookmark_url_html();
TotozBookmarkItem* totoz_bookmark_search(Dock *dock, const char *name);
TotozBookmarkItem* totoz_bookmark_insert(Dock *dock, const char *name);
void totoz_bookmark_load(Dock *dock);
void totoz_bookmark_save(Dock *dock, int merge_first);
void totoz_bookmark_save_html(Dock *dock);
#endif
