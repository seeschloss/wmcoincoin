
/*
  rcsid=$Id: global.h,v 1.31 2004/02/29 15:01:19 pouaite Exp $
  ChangeLog:
  $Log: global.h,v $
  Revision 1.31  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.30  2003/08/26 21:50:48  pouaite
  2.6.4b au mastic

  Revision 1.29  2002/12/20 17:40:55  pouaite
  ornythorinque en gelée

  Revision 1.28  2002/11/11 15:26:39  pouaite
  fix soulignement et strike avec les span

  Revision 1.27  2002/10/16 20:41:45  pouaite
  killall toto

  Revision 1.26  2002/10/15 23:17:28  pouaite
  rustinage à la truelle

  Revision 1.25  2002/09/05 23:11:57  pouaite
  <blog>ce soir g mangé une omelette</blog>

  Revision 1.24  2002/08/28 00:42:32  pouaite
  wmccc aware

  Revision 1.23  2002/08/21 01:11:49  pouaite
  commit du soir, espoir

  Revision 1.22  2002/08/17 18:33:39  pouaite
  grosse commition

  Revision 1.21  2002/06/02 13:31:37  pouaite
  bon, _maintenant_ c'est parti pour la 2.3.8b

  Revision 1.20  2002/04/09 00:28:19  pouaite
  quelques modifs faites dans un état d'hébétude avancé /!\ travaux en cours /!\

  Revision 1.19  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

  Revision 1.18  2002/03/18 22:46:49  pouaite
  1 ou 2 bugfix mineurs, et surtout suppression de la dependance avec la libXpm .. un premier pas vers wmc² en 8bits

  Revision 1.17  2002/03/07 18:54:34  pouaite
  raaa .. fix login_color (jjb) patch plop_words (estian) et bidouille pour le chunk encoding (a tester)

  Revision 1.16  2002/03/03 10:10:04  pouaite
  bugfixes divers et variés

  Revision 1.15  2002/02/26 22:02:07  pouaite
  bugfix gruikissime pour les pbs de lag sous cygwin

  Revision 1.14  2002/02/26 09:18:23  pouaite
  bugfixes divers

  Revision 1.13  2002/02/24 22:13:57  pouaite
  modifs pour la v2.3.5 (selection, scrollcoin, plopification, bugfixes)

  Revision 1.12  2002/02/02 23:49:17  pouaite
  plop

  Revision 1.11  2002/01/20 02:17:13  pouaite
  modifs d'ordre esthetique (!) sans grand interet

  Revision 1.10  2002/01/19 19:56:09  pouaite
  petits crochets pour la mise en valeur de certains messages (cf changelog)

  Revision 1.9  2002/01/16 00:35:26  pouaite
  debut de detection des reponse à nos message avec des couleurs hideuses et certainement plein de bugs moisis

  Revision 1.8  2002/01/14 23:54:06  pouaite
  reconnaissance des posts effectué par l'utilisateur du canard (à suivre...)

  Revision 1.7  2002/01/13 15:19:00  pouaite
  double patch: shift -> tribune.post_cmd et lordOric -> tribune.archive

  Revision 1.6  2002/01/12 17:29:08  pouaite
  support de l'iso8859-15 (euro..)

  Revision 1.5  2002/01/10 09:18:23  pouaite
  patch de jjb (ralentissement progressif des updates de la tribune en cas d'inactivité du coincoin)

  Revision 1.4  2001/12/18 12:43:37  pouaite
  ajout de l'option de la fonte des ballons d'aide (pour mr. imr !) + bugfix d'une connerie assez naze dans la gestion du nom du fichier d'options (merci glandium de me l'avoir signalé)

  Revision 1.3  2001/12/16 01:43:33  pouaite
  filtrage des posts, meilleure gestion des posts multiples

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

*/

#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED /* pour getdate */
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#include "config.h"
#include "prefs.h"
#ifndef HAVE_BACKTRACE
# include <assert.h>
#else 
void assertion_failed();
# undef assert
# define assert(x) if (!(x)) assertion_failed(__PRETTY_FUNCTION__, #x);
#endif
#define USERNAME_MAX_LEN 60 // lg max du username affiché dans la tribune, pour les personnes loggées

#ifndef __APPLE_CC__
# ifdef __GNUC__
#  define UNUSED __attribute((unused))
# else
#  define UNUSED
# endif
#else
# define UNUSED
#endif

#ifdef GLOBALS_HERE
# define DECL_GLOB(x) x;
# define DECL_GLOB_INIT(x,y) x = y
#else
# define DECL_GLOB(x) extern x
# define DECL_GLOB_INIT(x,y) extern x
#endif

#ifndef NO_BLAHBLAH
# define BLAHBLAH(n,x) if (Prefs.verbosity >= n) { x; fflush(stdout); }
#else
# define BLAHBLAH(n,x)
#endif

#ifndef NO_BITFIELDS
#  define BITFIELD(n) :n
#else
#  define BITFIELD(n) 
#endif 

#define WMCC_TIMER_DELAY_MS 40 /* un bip toutes les 40 millisecondes */

#ifndef IN_WMCCC
/* variables communes ici: */

#ifdef USE_VALGRIND
# define NOSIGNALS
# include <sys/times.h>
  DECL_GLOB_INIT(volatile clock_t last_call_X_loop, 0);
# define VALGRINDCHK { struct tms buf; clock_t t = times(&buf); if (t-last_call_X_loop > 3) { last_call_X_loop = t; X_loop_request++; wmcc_tic_cnt++; }}
#else
# define VALGRINDCHK
#endif

/*
  il faut des protections pour les appels systèmes non reentrant :-( (malloc...)
  (la cause des bugs bizarres du "mur vert" ?)
*/
void X_loop();
void ispell_run_background(const char* spellCmd, const char* spellDict);
#define ALLOW_ISPELL if (Prefs.ew_do_spell) {ispell_run_background(Prefs.ew_spell_cmd, Prefs.ew_spell_dict);}
#define ALLOW_X_LOOP VALGRINDCHK; if (X_loop_request) { if (X_loop_request > 1 && Prefs.verbosity) { printf("%s, ligne %d : X_loop_request=%d!\n", __FILE__, __LINE__, X_loop_request); }X_loop(); }
#define ALLOW_X_LOOP_MSG(m) VALGRINDCHK; if (X_loop_request) { if (X_loop_request > 1 && Prefs.verbosity) { printf(m " : X_loop_request=%d!\n", X_loop_request); }  X_loop(); }

/* très très laid, voir wmcoincoin.c/Timer_Thread */
#ifdef __CYGWIN__
# define CYGWIN_ENABLE_THREAD_X_LOOP flag_cygwin_x_loop_in_thread = 1;
# define CYGWIN_DISABLE_THREAD_X_LOOP flag_cygwin_x_loop_in_thread = 0;
DECL_GLOB_INIT(volatile int flag_cygwin_x_loop_in_thread,0);
#else
# define CYGWIN_ENABLE_THREAD_X_LOOP 
# define CYGWIN_DISABLE_THREAD_X_LOOP 
#endif


DECL_GLOB(GeneralPrefs Prefs);
DECL_GLOB_INIT(volatile int X_loop_request, 0);

/* +1 = demande au coincoin de fermer ses fenetres,
   -1 = demande au coincoin de se rouvrir
   0  = ça roule
*/
DECL_GLOB_INIT(volatile int flag_discretion_request, 0);
/*
  non nul quand on met a jours les donnees:
   -> updates des news, update tribune
*/
DECL_GLOB_INIT(volatile int flag_news_updated, 0);
DECL_GLOB_INIT(volatile int flag_updating_board, 0);
DECL_GLOB_INIT(volatile int flag_troll_braining,0); /* pour indiquer sur la led si on est dans 'troll_detector' */
DECL_GLOB_INIT(volatile int flag_gethostbyname,0); /* pour savoir si le coincoin est en plein gethostbyname */
DECL_GLOB_INIT(volatile int flag_board_updated, 0);
DECL_GLOB_INIT(volatile int flag_http_transfert, 0);
DECL_GLOB_INIT(volatile int flag_http_error, 0);
DECL_GLOB_INIT(volatile int flag_spell_request, 0); /* pour le palmipede (ça commence à puer le vilain hack... mais bon, je veux pas de threads alors j'assume) */
DECL_GLOB_INIT(volatile int flag_spell_finished, 0);
DECL_GLOB_INIT(int temps_depuis_dernier_event, 0); /* incrémenté 25 fois / sec */

DECL_GLOB_INIT(volatile int flag_cancel_task, 0); /* positionné qd l'utilisateur demande l'annulation du d/l en cours */

/*
  1 == relire le fichier d'options normal
  2 == relire le fichier d'options temporaire écrit par wmccc
  ne pas utiliser directement, passer par ccqueue_find
*/
DECL_GLOB_INIT(volatile int flag_update_prefs_request, 0);

DECL_GLOB_INIT(volatile int wmcc_tic_cnt,0);

DECL_GLOB_INIT(unsigned global_http_upload_cnt,0);
DECL_GLOB_INIT(unsigned global_http_download_cnt,0);
DECL_GLOB_INIT(char *options_file_name, NULL); /* le nom du fichier d'options (par defaut: 'options') */

DECL_GLOB_INIT(char *app_useragent, NULL);


typedef struct id_type {
  int lid BITFIELD(25); /* attention c pas portable, on ne peut pas présumer que le bitfield sera
			   effectivement signé :-/ (ex. compiler avec gcc -ftraditionnal) */
  int sid  BITFIELD(7);
} id_type;

inline static int
id_type_is_invalid(id_type id) {
  if (id.lid == -1) {
    assert(id.sid==-1); return 1;
  } else {
    assert(id.lid>=0);
    assert(id.sid>=0); return 0;
  }
}

inline static id_type
id_type_invalid_id() {
  id_type id;
  id.lid = -1; id.sid = -1;
  return id;
}

inline static int
id_type_to_int(id_type id) {
  int i;
  if (id_type_is_invalid(id)) return -1;
  i = id.sid + id.lid * MAX_SITES;
  return i;
}

inline static id_type
int_to_id_type(int i) {
  id_type id;
  if (i == -1) return id_type_invalid_id();
  id.sid = i % MAX_SITES;
  id.lid = i / MAX_SITES;
  return id;
}

inline static int
id_type_eq(id_type a, id_type b) {
  return (a.lid == b.lid && a.sid == b.sid);
}

inline static int 
id_type_sid(id_type a) {
  return a.sid;
}

inline static int 
id_type_lid(id_type a) {
  return a.lid;
}

inline static void id_type_set_lid(id_type *id, int lid) {
  id->lid = lid;
}

inline static void id_type_set_sid(id_type *id, int sid) {
  id->sid = sid;
}


#endif /* IN_WMCCC */
#endif

