/*
    wmCoinCoin, the stupidest WindowMaker dock applet
    Copyright (C) 2001  Julien Pommier

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA


 */

/*
  rcsid=$Id: board.c,v 1.32 2005/09/25 12:08:55 pouaite Exp $
  ChangeLog:
  $Log: board.c,v $
  Revision 1.32  2005/09/25 12:08:55  pouaite
  ca marche encore ca ?

  Revision 1.31  2005/06/11 22:47:41  pouaite
  prout?

  Revision 1.30  2005/02/22 18:45:26  pouaite
  *** empty log message ***

  Revision 1.29  2004/05/16 12:54:29  pouaite
  250c

  Revision 1.28  2004/04/26 20:32:31  pouaite
  roger demande le commit

  Revision 1.27  2004/04/18 15:37:28  pouaite
  un deux un deux

  Revision 1.26  2004/03/07 13:51:12  pouaite
  commit du dimanche

  Revision 1.25  2004/03/03 23:00:39  pouaite
  commit du soir

  Revision 1.24  2004/02/29 19:01:26  pouaite
  et hop

  Revision 1.23  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.22  2003/08/26 21:50:48  pouaite
  2.6.4b au mastic

  Revision 1.21  2003/07/20 22:22:28  pouaite
  ce commit est dedie a Pierre Tramo

  Revision 1.20  2003/06/29 23:58:35  pouaite
  suppression de l'overrideredirect du palmi et ajout de pinnipede_totoz.c et wmcoincoin-totoz-get etc

  Revision 1.19  2003/06/21 14:48:45  pouaite
  g cho

  Revision 1.18  2003/03/09 13:02:47  pouaite
  ouéééééé

  Revision 1.17  2003/01/11 17:44:10  pouaite
  ajout de stats/coinping sur les sites

  Revision 1.16  2002/12/20 18:11:46  pouaite
  bon...

  Revision 1.15  2002/12/20 15:49:51  pouaite
  prout 2.4.2b ?

  Revision 1.14  2002/11/30 00:10:39  pouaite
  2.4.2a

  Revision 1.13  2002/10/13 23:30:49  pouaite
  plop

  Revision 1.12  2002/09/25 22:02:15  pouaite
  hungry boitakon

  Revision 1.11  2002/09/08 14:28:45  pouaite
  bugfixes salutaires

  Revision 1.10  2002/09/07 16:21:15  pouaite
  ça va releaser en douce

  Revision 1.9  2002/09/05 23:11:57  pouaite
  <blog>ce soir g mangé une omelette</blog>

  Revision 1.8  2002/09/02 23:24:41  pouaite
  bugfixes de la soiree

  Revision 1.7  2002/08/29 00:15:53  pouaite
  cosmétique et capillotraction

  Revision 1.6  2002/08/26 00:52:22  pouaite
  coin coin coin

  Revision 1.5  2002/08/21 20:22:16  pouaite
  fix compil

  Revision 1.4  2002/08/21 01:11:49  pouaite
  commit du soir, espoir

  Revision 1.3  2002/08/18 19:00:28  pouaite
  plop

  Revision 1.2  2002/08/18 00:29:30  pouaite
  en travaux .. prière de porter le casque

  Revision 1.1  2002/08/17 18:33:38  pouaite
  grosse commition


  ---------------- renommage en board.c --------------------

  Revision 1.39  2002/06/26 22:19:49  pouaite
  ptit fix pour la tribune de f-cpu + patch de lordoric

  Revision 1.38  2002/06/23 22:26:01  pouaite
  bugfixes+support à deux francs des visuals pseudocolor

  Revision 1.37  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.35  2002/06/01 17:54:04  pouaite
  nettoyage

  Revision 1.34  2002/05/28 20:11:55  pouaite
  modif pr un pinnipede + fluide qd il y a bcp de messages stockés + tribune sur plusieurs jours

  Revision 1.33  2002/05/12 22:06:27  pouaite
  grosses modifs dans http.c

  Revision 1.32  2002/04/13 11:55:19  pouaite
  fix kde3 + deux trois conneries

  Revision 1.31  2002/04/11 23:16:54  pouaite
  boitakon mega combo

  Revision 1.30  2002/04/09 23:38:29  pouaite
  boitakon et son cortège de bugfixes

  Revision 1.29  2002/04/09 00:28:19  pouaite
  quelques modifs faites dans un état d'hébétude avancé /!\ travaux en cours /!\

  Revision 1.28  2002/04/01 22:56:03  pouaite
  la pseudo-transparence du pinni, bugfixes divers, option tribune.backend_type

  Revision 1.27  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

  Revision 1.26  2002/03/27 23:27:10  pouaite
  tjs des bugfixes (pour gerer des posts qui peuvent atteindre 10ko !), en parallele de la v2.3.6-5

  Revision 1.25  2002/03/27 19:02:04  pouaite
  bugfix pour le nouveau format du backend

  Revision 1.24  2002/03/18 22:46:49  pouaite
  1 ou 2 bugfix mineurs, et surtout suppression de la dependance avec la libXpm .. un premier pas vers wmc² en 8bits

  Revision 1.23  2002/03/03 10:10:04  pouaite
  bugfixes divers et variés

  Revision 1.22  2002/03/01 00:27:40  pouaite
  trois fois rien

  Revision 1.21  2002/02/27 00:32:19  pouaite
  modifs velues

  Revision 1.20  2002/02/24 22:13:56  pouaite
  modifs pour la v2.3.5 (selection, scrollcoin, plopification, bugfixes)

  Revision 1.19  2002/02/03 23:07:32  pouaite
  *** empty log message ***

  Revision 1.18  2002/02/02 23:49:17  pouaite
  plop

  Revision 1.17  2002/01/20 20:53:22  pouaite
  bugfix configure.in && http_win.c pour cygwin + 2-3 petis trucs

  Revision 1.16  2002/01/20 02:17:13  pouaite
  modifs d'ordre esthetique (!) sans grand interet

  Revision 1.15  2002/01/18 00:28:42  pouaite
  le ménage continue + grosses modifs (experimentales pour l'instant)

  Revision 1.14  2002/01/16 00:35:26  pouaite
  debut de detection des reponse à nos message avec des couleurs hideuses et certainement plein de bugs moisis

  Revision 1.13  2002/01/15 15:30:17  pouaite
  rien d'interessant

  Revision 1.12  2002/01/14 23:54:06  pouaite
  reconnaissance des posts effectué par l'utilisateur du canard (à suivre...)

  Revision 1.11  2002/01/13 20:06:14  pouaite
  decidement je fais rien que des conneries

  Revision 1.10  2002/01/13 19:44:48  pouaite
  minifix pour éviter que la commande externe soit lancée sur tous les messages à l'initialisation du coincoin

  Revision 1.9  2002/01/13 15:19:00  pouaite
  double patch: shift -> tribune.post_cmd et lordOric -> tribune.archive

  Revision 1.8  2002/01/12 17:29:08  pouaite
  support de l'iso8859-15 (euro..)

  Revision 1.7  2002/01/10 09:03:06  pouaite
  integration du patch de glandium (requetes http/1.1 avec header 'If-Modified-Since' --> coincoin plus gentil avec dacode)

  Revision 1.6  2002/01/06 16:52:37  pouaite
  preparation pour la prochaine v. de la tribune avec sa gestion integree du wiki et des logins, tout ça ..

  Revision 1.5  2001/12/16 21:51:22  pouaite
  *** empty log message ***

  Revision 1.4  2001/12/16 20:28:45  pouaite
  bugfixes divers

  Revision 1.3  2001/12/16 01:43:33  pouaite
  filtrage des posts, meilleure gestion des posts multiples

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

*/
#include "board_priv.h"
#include "balltrap.h"



static md5_and_time *
load_md5_array(FILE *f) {
  md5_and_time *m = NULL;
  if (f) {
    char tstamp[15];
    unsigned char md5txt[33];
    int viewed;
    while (fscanf(f, " MD5:%s %s %d", md5txt, tstamp, &viewed) == 3) {
      if (strlen(md5txt) == 32 && strlen(tstamp)==14) {
        BLAHBLAH(2, myprintf("loaded md5 %<grn %s> tstamp %<cya %s> viewed=%d\n", md5txt, tstamp,viewed));
        int i;
        md5_and_time *n; ALLOC_OBJ(n,md5_and_time);
        n->next = m; m = n;
        strcpy(m->tstamp,tstamp);
        for (i=0; i < 16; ++i) {
          int j;
          sscanf(md5txt+2*i,"%02x",&j); m->md5[i] = j;
        }
        m->viewed = viewed;
      }
    }
  }
  return m;
}

void release_md5_array(Board *b) {
  if (b) {
    md5_and_time *m = b->oldmd5;
    while (m) {
      md5_and_time *n = m->next; free(m); m = n;
    }
    b->oldmd5 = NULL;
  }
}

md5_and_time *
find_md5_in_md5_array(md5_byte_t md5[16], md5_and_time *m) {
  while (m) {
    if (memcmp(md5, m->md5,16) == 0)
      return m;
    m = m->next;
  }
  return NULL;
}


static void save_md5_and_time(FILE *f, md5_byte_t md5[16], char tstamp[15], int viewed) {
  char md5txt[33];
  int i;
  for (i = 0; i < 16; ++i) {
    md5txt[2*i  ] = "0123456789ABCDEF"[md5[i]/16];
    md5txt[2*i+1] = "0123456789ABCDEF"[md5[i]%16];
  }
  md5txt[32] = 0;
  fprintf(f, "MD5:%s %s %d\n", md5txt, tstamp, viewed);
}

static void save_md5_array_recurs(FILE *f, Board *b, board_msg_info *mi) {
  char tstamp[15];
  if (!mi) return;
  time_t_to_tstamp(mi->timestamp, tstamp);

  if (mi->ri) save_md5_and_time(f,mi->ri->md5,tstamp, id_type_lid(mi->id) <= b->last_viewed_id);

  save_md5_array_recurs(f, b, mi->left);
  save_md5_array_recurs(f, b, mi->right);
}

int board_is_rss_feed(Board *b) {
  if (!b) return 0;
  return b->site->prefs->backend_type == BACKEND_TYPE_RSS;
}

int board_is_regular_board(Board *b) {
  if (!b) return 0;
  return b->site->prefs->backend_type == BACKEND_TYPE_BOARD;
}

int board_is_pop3(Board *b) {
  if (!b) return 0;
  return b->site->prefs->backend_type == BACKEND_TYPE_POP;
}

int board_can_post_messages(Board *b) {
  if (!b) return 0;
  if (b->site->prefs->post_url && strlen(b->site->prefs->post_url) &&
      b->site->prefs->post_template && strlen(b->site->prefs->post_template)) {
    if (str_startswith(b->site->prefs->post_url, "fucking_broken")) {
      myprintf("\n\n%<RED BROKEN POST_URL DETECTED>: %s FIX THAT (update your options file and use the new '.post.url:' and '.post.template:' ones\n\n", b->site->prefs->post_url);
      return 0;
    }
    return 1;
  }
  else return 0;
}

void board_save_state(FILE *f, Board *board) {
  long tmin=LONG_MIN,tmax=LONG_MAX,t=0;
  tmin = board->time_shift_min;
  tmax = board->time_shift_max;
  t = board->time_shift;
  BLAHBLAH(1,myprintf("%<yel site_save_state> %10s : tmin -> %<cya %3ld> tmax -> %<cya %3ld> t -> %<CYA %3ld>\n",
                      board->site->prefs->site_name, tmin, tmax, t));
  fprintf(f, "time_shift_min=%ld time_shift_max=%ld time_shift=%ld\n", (long)tmin, (long)tmax, (long)t);
  fprintf(f, "last_viewed_id=%d\n", board->last_viewed_id);
  if (board_is_rss_feed(board) || board_is_pop3(board)) {
    if (board->mi_tree_root) {
      save_md5_array_recurs(f, board, board->mi_tree_root);
    } else { /* on n'a pas fait un seul refresh, ça serait bete de tout ecraser
                et comme le fichier a dejà ete ouvert en ecriture .. */
      md5_and_time *m = board->oldmd5;
      while (m) {
        save_md5_and_time(f, m->md5, m->tstamp, m->viewed); m = m->next;
      }
    }
  }
}

void board_restore_state(FILE *f, Board *board) {
  long tmin,tmax,t;
  if (fscanf(f, "time_shift_min=%ld time_shift_max=%ld time_shift=%ld\n",&tmin, &tmax, &t) != 3) return;
  if (tmin <= tmax && tmin <= t && t <= tmax && board) {
    BLAHBLAH(1,myprintf("%<yel site_restore_state> %10s : tmin <- %<cya %3ld> tmax <- %<cya %3ld> t <- %<CYA %3ld>\n",
			board->site->prefs->site_name, tmin, tmax, t));
    board->time_shift_min = tmin;
    board->time_shift_max = tmax;
    board->time_shift = t;
  }
  if (fscanf(f, "last_viewed_id=%d", &board->last_viewed_id) == EOF)
    myfprintf(stderr, "fscanf() failed\n");
  if (board_is_rss_feed(board) || board_is_pop3(board)) {
    board->last_viewed_id = -1; /* on dispose d'une liste de md5 */
    release_md5_array(board);
    board->oldmd5 = load_md5_array(f);
  } else board->oldmd5 = NULL;
}


void
board_set_viewed(Board *board, int id) {
  board->last_viewed_id = MAX(id,board->last_viewed_id);
  board->nb_msg_since_last_viewed = MAX(board->last_post_id - board->last_viewed_id,0);
}


Board *
board_create(Site *site, Boards *boards)
{
  SitePrefs *sp = site->prefs;
  Board *board;

  assert(boards);
  ALLOC_OBJ(board, Board);
  strncpy(board->last_post_time, "xx:xx", 5);
  board->msg = NULL;
  board->last_post_id = -1;
  board->last_post_id_prev = -1;
  board->wmcc_tic_cnt_last_check = 0;
  board->last_post_timestamp = 0;
  board->nbsec_since_last_msg = 0;
  board->last_viewed_id = -1; board->nb_msg_since_last_viewed = 0;
  board->nb_msg_at_last_check = 0;
  board->local_time_last_check = time(NULL);
  //board->rules = NULL;
  board->last_modified = NULL;
  board->flag_answer_to_me = 0;
  board->board_refresh_delay = sp->board_check_delay*(1000/WMCC_TIMER_DELAY_MS);
  /* juste pour que le premier check se fasse avant celui des news */
  board->board_refresh_decnt = 10; 
  strncpy(board->coin_coin_useragent, sp->user_agent, USERAGENT_MAXMAX_LEN);
  board->coin_coin_useragent[USERAGENT_MAXMAX_LEN] = 0;

  board->site = site;
  board->boards = boards;

  board->time_shift_min = LONG_MIN;
  board->time_shift_max = LONG_MAX;
  board->time_shift = 0;
  board->auto_refresh = 1; //sp->board_auto_refresh;
  board->oldmd5 = NULL;
  board->rss_title = NULL;
  board->encoding = NULL;
  return board;
}

/* delie un message de la liste globale (avec sa suppression effective) */
static void
board_global_unlink_msg(Boards *boards, board_msg_info *mi)
{
  assert(mi);

  if (mi->g_prev) {
    mi->g_prev->g_next = mi->g_next;
  } else {
    assert(boards->first == mi);
    boards->first = mi->g_next;
    if (boards->first) boards->first->g_prev = NULL;
  }
  if (mi->g_next) {
    mi->g_next->g_prev = mi->g_prev;
  } else {
    assert(boards->last == mi);
    boards->last = mi->g_prev;
    if (boards->last) boards->last->g_next = NULL;
  }
  if (boards->first == NULL || boards->last == NULL) {
    assert(boards->last == NULL);
    assert(boards->first == NULL);
  }
}


void
board_destroy(Board *board)
{
  board_msg_info *mi, *nmi;
  Boards *boards = board->boards;

  boards->btab[board->site->site_id] = NULL;
  /* 
     effacage de toutes les refs à des messages de cette board 
     c'est pas joli joli, mais faut bien faire le sale boulot
   */
  mi = board->boards->first;
  while (mi) {
    int i;
    if (mi->nb_refs) {
      for (i = 0; i < mi->nb_refs; i++) {
	if (mi->refs[i].mi) {
	  Board *ref_board = NULL;
	  ref_board = boards->btab[id_type_sid(mi->refs[i].mi->id)];
	  if (ref_board == board) mi->refs[i].mi = NULL;
	}
      }
    }
    mi = mi->g_next;
  }

  /*
    j'vais lui montrer qui c'est Raoul. Au 4 coins d'Paris qu'on va l'retrouver 
    éparpillé par petits bouts façon puzzle... Moi quand on m'en fait trop 
    j'correctionne plus, j'dynamite... j'disperse... et j'ventile...
  */
  mi = board->msg;
  while (mi) {
    board_global_unlink_msg(board->boards, mi);
    nmi = mi->next;
    if (mi->refs) free(mi->refs); mi->refs = NULL;
    mi->next = NULL;
    free(mi);
    mi = nmi;
  }

  board->msg = NULL;
  if (board->last_modified) free(board->last_modified);
  if (board->rss_title) free(board->rss_title);
  if (board->encoding) free(board->encoding);
  release_md5_array(board);
  free(board);
}


/*
  statistique à la noix sur le nombre de personnes sur la tribune
*/
#define TF_HASH_SZ 2048
void
board_frequentation(const Board *board, int nb_minutes, int *ua_cnt, int *msg_cnt, int *my_msg_cnt) {
  unsigned short hash_cnt[TF_HASH_SZ];
  unsigned hash_val;
  const unsigned c2 = 31117, c1 = 11279; 

  time_t t_last;

  board_msg_info *it;
  int i;
  

  

  *ua_cnt = 0;
  *msg_cnt = 0;
  *my_msg_cnt = 0;

  it = board_find_id(board, board->last_post_id);
  if (it == NULL) return;
  t_last = it->timestamp;
  t_last += board->nbsec_since_last_msg;

  memset(hash_cnt, 0, sizeof(hash_cnt));
  it = board->msg; 
  while (it) {
    if (difftime(t_last, it->timestamp) < nb_minutes*60) {
      const char *s;
      if (it->login[0] == 0) {
	s = it->useragent;
      } else {
	s = it->login;
      }
      /* au pifometre... faudra ptet verifier qu'on hache correctement */
      hash_val = 1;
      for (i=0; s[i]; i++) {
	hash_val = ((hash_val*((unsigned char)s[i] + c1)) + 1) % c2;
      }
      hash_val = hash_val % TF_HASH_SZ;
      hash_cnt[hash_val]++;
      (*msg_cnt)++;
      if (it->is_my_message) (*my_msg_cnt)++;
    }

    it = it->next;
  }
  for (i=0; i < TF_HASH_SZ; i++) {
    if (hash_cnt[i]) (*ua_cnt)++;
  }
}


/* je l'aime bien celle la */
static void
miniua_eval_from_ua(MiniUARules *rules, board_msg_info *mi)
{
#define MUA_MAX_MATCH 9

  MiniUARule *r;
  MiniUA *mua;
  int sid, matched;
  int color_done = 0, ua_done = 0, symb_done = 0;
  static unsigned default_colors[] = { 
    0x82e734, /*joli vert*/
    0xd2262f, /*rouge qui claque*/
    0xedd92e, /*jaune*/
    0xed992c, /*orange*/
    0xed610d, /*tres orange*/
    0x526bed, /*bleu electrique*/
    0x9c9c9c, /*gris*/
    0xed6b52, /*rose*/
    0x23b6ff, /*joli bleu*/
    0xd264ed, /*violet*/
    0x9523ff, /*plus violet*/
    0xd5008b, /*encore plus*/
    0x1Feda7, /*bleu-vert*/
    0xd5e734, /*jaune-vert*/
  };

  assert(mi);
  mua = &mi->miniua;
  sid = id_type_sid(mi->id);

  /* valeur par defaut -- on essaye de prendre quelque chose d'un peu chattoyant */
  int coul = default_colors[((unsigned)str_hache(Prefs.site[id_type_sid(mi->id)]->site_name,-1)>>4) 
                            % (sizeof default_colors / sizeof default_colors[0])];
  //printf("coul = %06x mx=%d\n", coul, (sizeof default_colors / sizeof default_colors[0]));
  mua->R = coul >> 16;
  mua->G = (coul >> 8) & 0xff;
  mua->B = coul & 0xff;
  if (Prefs.site[id_type_sid(mi->id)]->backend_type == BACKEND_TYPE_RSS) { mua->symb = 1; /* carre */ }
  else if (Prefs.site[id_type_sid(mi->id)]->backend_type == BACKEND_TYPE_POP) { mua->symb = 10; /* cercle*/ }
  else {
    md5_byte_t md5[16];    
    if (mi->login && strlen(mi->login)) { 
      md5_digest(mi->login, md5); mua->symb = (md5[0]%8)+2; 
    } else if (mi->useragent && strlen(mi->useragent)) { 
      md5_digest(mi->useragent, md5); mua->symb = (md5[0]%8)+2;
    } else { mua->symb = 13; }
  }
  make_short_name_from_ua(mi->useragent, mua->name, MIN(MINIUA_SZ,15));

  for (r = rules->first; r; r = r->next) {
    int res;
    
    matched = 0;

    /* ça colle pour le login / nom du site ? */
    if ((r->site_name == NULL || strcasecmp(Prefs.site[sid]->site_name, r->site_name) == 0) &&
	(r->user_login == NULL || strcasecmp(mi->login, r->user_login) == 0)) {
      regmatch_t match[MUA_MAX_MATCH];

      /* y'a-t-il une regexp à matcher ? */
      if (r->rgx) {
	res = regexec(r->rgx, mi->useragent, MUA_MAX_MATCH, match, 0);
	if (res == 0) { /*       \o/      */
	  matched = 1;
	  /* y'a -t-il une regle de remplacement de l'ua ? */
	  if (r->rua && ua_done == 0) {
	    const char *keys[] = {"\\0", "\\1", "\\2", "\\3", "\\4", "\\5", "\\6", "\\7", "\\8", "\\9"};
	    char *repl[MUA_MAX_MATCH];
	    char *s;
	    int i;

	    for (i = 0; i < MUA_MAX_MATCH; i++) {
	      int i0, i1;
	      i0 = match[i].rm_so; i1 = match[i].rm_eo;
	      if (i0 != -1 && i1 != -1) { 
		assert(i0 <= i1);
		repl[i] = malloc(i1-i0 + 1);
		strncpy(repl[i], mi->useragent + i0, i1-i0);
		repl[i][i1-i0] = 0;
	      } else repl[i] = strdup("<!>");
	    }
	    /* sale cast */
	    s = str_multi_substitute(r->rua, keys, (const char **)repl, MUA_MAX_MATCH);
	    
	    /* printf("miniua <--- '%s' 1:'%s' 2:'%s' 3:'%s'\n", s, repl[0], repl[1], repl[2]); */

	    strncpy(mua->name, s, MINIUA_SZ); mua->name[MINIUA_SZ-1] = 0;
	    free(s);
	    for (i = 0; i < MUA_MAX_MATCH; i++) free(repl[i]);

	    ua_done = r->ua_terminal;
	  }
	}
      } else {
	matched = 1;
	if (r->rua) { /* une regle statique */
	  strncpy(mua->name, r->rua, MINIUA_SZ); mua->name[MINIUA_SZ-1] = 0;
	}
      }
    }

    if (matched) {
      if (r->color >= 0 && color_done == 0) {
	mua->R = (r->color & 0xff0000) >> 16;
	mua->G = (r->color & 0x00ff00) >> 8;
	mua->B = (r->color & 0x0000ff);
	color_done = r->color_terminal;
      }
      if (r->symb >= 0 && symb_done == 0) {
	mua->symb = r->symb;
	symb_done = r->symb_terminal;
      }
    }
/*     if (matched) { */
/*       printf("msg %d [%.20s] matched width {%s,%s,%p} %s\n", id_type_lid(mi->id), mi->useragent, r->site_name, r->rua, r->rgx, r->terminal ? "[TERMINAL]" : ""); */
/*     } */
    if (color_done && ua_done && symb_done)
      break;
  }
}

/* 
   renvoie l'age du message, en secondes 
*/
time_t
board_get_msg_age(const Board *board, const board_msg_info *it)
{
  return MAX(time(NULL) - (it->timestamp + board->time_shift),0);
}

/*
  construction d'un arbre binaire pour retrouver + rapidement les
  messages en fonction de l'id
*/

static board_msg_info *
board_build_tree_recurs(board_msg_info **v, int cnt)
{
  int i_root;
  board_msg_info *root;

  assert(cnt>0);
  i_root = cnt/2;
  
  root = v[i_root]; assert(root); v[i_root] = NULL;

  if (i_root > 0) {
    root->left = board_build_tree_recurs(v, i_root);
  } else root->left = NULL;
  if (i_root+1 < cnt) {
    root->right = board_build_tree_recurs(v+i_root+1, cnt - i_root - 1);
  } else root->right = NULL;
  return root;
}

static void
board_build_tree(Board *board)
{
  int count = 0, i;
  board_msg_info *it;
  board_msg_info **vec;

  /* remise à zero de l'arbre */
  board->mi_tree_root = NULL;

  /* on compte le nb de messages */
  it = board->msg; 
  while (it) { count++; it = it->next; }
  
  if (count == 0) return;

  /* on s'offre temporairement un accès séquentiel */
  vec = (board_msg_info**) calloc(count, sizeof(board_msg_info*)); assert(vec);
  it = board->msg; i = 0;
  while (it) { 
    vec[i] = it;  it = it->next; i++;
  }
  
  board->mi_tree_root = board_build_tree_recurs(vec, count);

  for (i=0; i < count; i++) {
    assert(vec[i] == NULL); /* sinon j'ai encore laissé trainer une pouille */
  }

  free(vec);
}

/*
  c'est triste, mais il faut bien que quelqu'un se charge d'éliminer les messages trop vieux
*/
static void
board_remove_old_msg(Board *board)
{
  board_msg_info *it;
  int cnt;
  int removed = 0;

  cnt = 0;
  it = board->msg;
  while (it) {
    cnt++;
    it = it->next;
  }
  while (cnt > board->site->prefs->board_max_msg && board->msg) {
    BLAHBLAH(4, printf(_("[%s] board_remove_old_msg: destroying id=%d (date=%s)\n"), 
		       board->site->prefs->site_name, id_type_lid(board->msg->id), 
		       ctime(&board->msg->timestamp)));

    /* on le supprime d'abord de la liste globable */
    board_global_unlink_msg(board->boards, board->msg);

    /* on sauve le prochain premier message */
    it = board->msg->next;


    /* nettoyage des references à board->msg 
       on fait ça sur boards pour aussi gerer les ref. cross tribune
    */
    {
      board_msg_info *mi;
      int i;
      mi = board->boards->first;
      while (mi) {
	for (i=0; i < mi->nb_refs; i++) {
	  /* si on trouve un ref à ce message ... */
	  if (mi->refs[i].mi == board->msg) {
	    assert(mi->refs[i].nbmi>=1);
	    
	    /* alors on l'efface si la ref pointe uniquement sur lui,
	       et si c'est un ref sur plusieurs messages consécutifs, alors
	       un decremente le compteur et on la fait pointer sur le suivant. 
	       c'est finaud en fait : comme ça il n'y a pas de bug lors de la
	       suppression de 11:11:11² car on a supprimé *auparavant* 11:11:11¹
	    */
	    mi->refs[i].nbmi--;
	    if (mi->refs[i].nbmi == 0) {
	      mi->refs[i].mi = NULL;
	    } else {
	      mi->refs[i].mi = board->msg->next;
	    }
	  }
	}
	mi = mi->g_next;
      }
    }

    free(board->msg);
    cnt--;
    board->msg = it;
    removed++;
  }
  
  if (removed) {
    board_build_tree(board);
  }
}

/*
  prout
*/

/* verifie pour chaque message, si il est necessaire d'afficher les secondes, ou bien
   si le poste peut etre identifie sans ambiguite par hh:ss 

   nouveau (v2.3.2) -> gère aussi les sub_timestamp 
*/
static void 
update_secondes_flag(Board *board)
{
  board_msg_info *it,*pit;

  pit = board->msg;
  if (pit == NULL) return;
  it = pit->next;
  while (it) {
    if (it->next) it->hmsf[3] = 0; 
    if (it->hmsf[0] == pit->hmsf[0] && it->hmsf[1] == pit->hmsf[1]) {
      it->hmsf[3] = 1; pit->hmsf[3] = 1;
    }
    if (it->timestamp == pit->timestamp) {
      if (pit->sub_timestamp == -1) {
	pit->sub_timestamp = 0;
      }
      it->sub_timestamp = MIN(pit->sub_timestamp+1,9); /* MIN(10,.) sinon y'aura des pb dans le pinnipede */
    }
    pit = it;
    it = it->next;
  }
}

/*
  enleve les commentaires xml (qui sont inseres autour des lien du wiki etc)
*/
static char *
nettoie_message_tags(const char *inmsg) 
{
  char *outmsg;
  const char *s; char *w, *p;
  int in_comment;

  outmsg = malloc(strlen(inmsg)+1);
  in_comment = 0;
  p = outmsg;
  for (s = inmsg; *s; s++) {
    if (strncmp(s, "\t<!--",5)==0 && in_comment == 0) {
      w = strstr(s, "--\t>");
      if (w) {
	in_comment = w+4-s;
      }
    }
    if (in_comment == 0) {
      *p = *s; p++;
    } else in_comment--;
  }
  *p = 0;
  return outmsg;
}

/*
  rajoute du pseudo-html à la noix autour du mot 
  et renvoie le resultat
*/
static char *
wiki_url_encode(Board *board,const unsigned char *w)
{
  unsigned char *w2, *ret;
  w2 = str_preencode_for_http(w);
  ret = str_printf("\t<a href=\"%s%s\"\t>[%s]\t</a\t>", board->site->prefs->board_wiki_emulation, w2, w);
  free(w2); 
  return ret;
}

/* 
   transforme les occurences de [mots entre crochets] par des <a href=> vers le wiki 
   
   si cette fonction est appelée avec 'dest == NULL' , alors elle se contente de renvoyer 
   la longueur finale obtenue
*/
static int
do_wiki_emulation(Board *board, const char *inmsg, char *dest) 
{
  int j;
  const char *s;

  int in_a_href = 0;
  s = inmsg; j = 0;
  while (*s) {
    if (*s == '\t') {
      /* pas touche à ce qui est déjà dans une <a> </a> */
      if (strncasecmp(s, "\t<a href", 8) == 0 && in_a_href==0) {
	in_a_href = 1;
      } else if (strncasecmp(s, "\t</a", 4) == 0 && in_a_href) {
	in_a_href = 0;
      }
    }
    if (*s == '[') {
      char *pfin;
      
      pfin = strchr(s+1, ']');
      
      if (pfin && in_a_href == 0) {
	char *ptag, *pautre;
	ptag = strchr(s+1, '\t');
	pautre = strchr(s+1, '[');

	/* on a un truc entre crochets sans autre crochet ouvrant à l'intérieur? */
	if ((pautre == NULL || pautre > pfin) &&
	    (ptag == NULL || ptag > pfin)) {
	  char *wiki_word, *wiki_url, *p;

	  /* on copie le mot-wiki */
	  wiki_word = malloc(pfin-s); assert(wiki_word);
	  strncpy(wiki_word, s+1, pfin-s-1); wiki_word[pfin-s-1] = 0;

	  /* on le modifie un peu (' ' --> '+' par ex. ) */
	  wiki_url = wiki_url_encode(board, wiki_word); free(wiki_word); wiki_word = NULL;
	  p = wiki_url;
	  while (*p) { 
	    if (dest) dest[j] = *p; 
	    j++; p++;
	  }
	  s = pfin+1;
	  free(wiki_url); wiki_url = NULL;
	  continue;
	}
      }
    }
    if(dest) dest[j] = *s; 
    j++; s++;
  }
  if (dest) dest[j] = 0;
  j++;
  return j;
}

/* remplace (laborieusement) une [url] par [yahoo] ou [prout] */
void
do_url_replacements(char **pmessage)
{
  unsigned char *src1, *src2, *insert, *src3;
  unsigned char *message;
  int repl = 0;

  message = NULL;
  src1 = *pmessage;
  assert(src1);
  while (src1) {
    src3 = insert = NULL;
    src2 = src1;
    while (*src2 && insert == NULL) {
      insert = NULL;
      
      if (*src2 == '\t' && strncasecmp(src2, "\t<a href=\"", 10) == 0) {
	unsigned char *deb_url, *fin_url, *tag_fermant;
	deb_url = src2+10;
	fin_url = strstr(deb_url, "\t>[url]\t</");
	tag_fermant = strstr(deb_url, "\t</a");
	if (deb_url && fin_url && tag_fermant && (tag_fermant > fin_url)) {
	  
	  URLReplacement *ur = Prefs.url_repl.first;
	  unsigned char *url, *crochet;
	  crochet = fin_url+2;
	  while (fin_url > deb_url+1 && *fin_url != '"') fin_url--;
	  url = str_ndup(deb_url, MIN(fin_url-deb_url,1024));
	  str_tolower(url);
	  while (ur) {
	    if (strstr(url, ur->key)) {
	      break;
	    }
	    ur = ur->next;
	  }
	  free(url);
	  if (ur) {
	    src2 = crochet;
	    src3 = crochet+5;
	    insert = ur->repl;
	    repl++;
	  }
	}
      }
      
      if (insert == NULL)
	src2++;
    }

    assert(src2);
      
    message = str_ncat(message, src1, src2-src1);
    if (insert) {
      message = str_cat(message, insert);
    }
    src1 = src3;
  }
  free(*pmessage);
  *pmessage = message;
  if (repl) {
    BLAHBLAH(4,myprintf("replaced (%d)! '%<yel %s>'\n", repl, message));
  }
}

/* Triton> Remplace (tout aussi laborieusement)
 * <clock time="..."[ site="..."]>nhorloge</clock> par nhorloge
 * Pour fonctionner avec les tribunes zoreliennes
 * (zorel, tifauv, bouchot.org, !llinfo, ...)
 */
void
do_clean_zorel_clocks(char **pmessage)
{
  /* zorel< est bien gentil avec ses tags clock mais ca fout en l'air
   * l'affichage dans wmcoincoin. En plus, wmcc est deja pourvu d'un fantastique
   * detecteur de nhorloges donc on va purement et simplement virer son bordel.
   * message     = contenu du message nettoye
   * not_parsed  = debut du message non parse
   * deb_clock   = debut du tag clock (tabulation initiale incluse)
   * fin_clock   = fin de ce meme tag clock
   * tag_fermant = debut du tag de fermeture de clock (10 car avec les tabulations)
   */
  unsigned char *not_parsed, *deb_clock, *fin_clock, *tag_fermant;
  unsigned char *message = NULL;

	message = malloc(1);
	*message = '\0';

  not_parsed = *pmessage;
  assert(not_parsed);
  /* Tant que le message n'est pas fini de parser et qu'il y a des horloges.
   * On en profite pour noter l'endroit ou commence l'horloge dans deb_clock.
   */
  while (not_parsed && (deb_clock = strstr(not_parsed, "\t<clock time=\""))) {
    fin_clock = strstr(deb_clock, "\t>");
		/* Ca fini _apres_ "\t>" donc +2 mais seulement si non nul */
		if (fin_clock) fin_clock += 2;
    tag_fermant = strstr(deb_clock, "\t</clock\t>");
    /* Si tag_fermant est non nul, fin_clock l'est forcement (= tag_fermant + 8 dans le pire des cas) */
    if (tag_fermant && (tag_fermant > fin_clock)) {
      /* Bon, on a trouve une horloge zorelienne a peu pres correcte.
       * Maintenant, faut nettoyer de deb_clock à fin_clock
       * et de tag_fermant a tag_fermant+10
       */
      message = str_ncat(message, not_parsed, deb_clock - not_parsed); /* Avant le tag : copie */
      message = str_ncat(message, fin_clock, tag_fermant - fin_clock); /* La nhorloge : copiee */
      not_parsed = tag_fermant+10; /* Pas de nhorloge dans l'intervalle, on saute */
    }
    else if (tag_fermant) {
      /* Le tag semble en vrac mais on va quand même regarder la suite si elle existe */
      message = str_ncat(message, not_parsed, (tag_fermant+10) - not_parsed);
      not_parsed = tag_fermant + 10;
    }
    else if (fin_clock) {
      /* Il n'y a pas de tag de fermeture, tant pis, on supprime celui d'ouverture. */
      message = str_ncat(message, not_parsed, deb_clock - not_parsed); /* Avant le tag : copie */
      message = str_cat(message, fin_clock); /* on copie la fin du message */
      not_parsed = NULL;
    }
    else {
      /* C'est pas la merde maintenant, on n'a meme pas de fin de tag. */
      message = str_cat(message, not_parsed); /* On n'a plus qu'à copier la fin du message. */
      not_parsed = NULL;
    }
    /* Si on est la fin (cas nominal ou tag en vrac) */
    if (not_parsed && ((*not_parsed) == '\0')) {
      not_parsed = NULL;
    }
  }
	/* S'il reste une partie non lue, c'est qu'il n'y a plus d'horloge dedans, on la copie */
	if (not_parsed) message = str_cat(message, not_parsed);
  /* On efface le message d'origine et on le remplace par la version epuree */
  free(*pmessage);
  *pmessage = message;
}

/* Triton> Remplace (toujours laborieusement)
 * <a class=" smiley ..." href="...">[:totoz]</a> par [:totoz]
 * Pour fonctionner avec les tribunes zoreliennes
 * (zorel, tifauv, bouchot.org, !llinfo, ...)
 */
void
do_clean_zorel_totozz(char **pmessage)
{
  /* zorel< est bien gentil avec ses tags autour des [:totoz] mais ca fout en
	 * l'air l'affichage dans wmcoincoin. En plus, wmcc est deja pourvu d'un
   * extraordinaire detecteur de [:totoz] donc on va purement et simplement
	 * virer son bordel.
   * message     = contenu du message nettoye
   * not_parsed  = debut du message non parse
   * deb_totoz   = debut du tag <a class="smiley snap_nopreview"
   * fin_totoz   = fin de ce meme tag d'ancre
   * tag_fermant = debut du tag de fermeture de [:totoz]
   */
  unsigned char *not_parsed, *deb_totoz, *fin_totoz, *tag_fermant;
  unsigned char *message = NULL;

	message = malloc(1);
	*message = '\0';

  not_parsed = *pmessage;
  assert(not_parsed);
  /* Tant que le message n'est pas fini de parser et qu'il y a des [:totoz].
   * On en profite pour noter l'endroit ou commence les [:totoz] dans deb_totoz.
   */
  while (not_parsed && (deb_totoz = strstr(not_parsed, "\t<a class=\"smiley "))) {
    fin_totoz = strstr(deb_totoz, "\t>[:");
		/* Ca fini _apres_ "\t>" donc +2 mais seulement si non nul */
		if (fin_totoz) fin_totoz += 2;
    tag_fermant = strstr(deb_totoz, "]\t</a\t>");
		/* Le crochet fermant, c'est juste pour s'assurer du fait que c'est un [:totoz] */
		if (tag_fermant) ++tag_fermant;
		/* Si tag_fermant > fin_totoz et fin_totoz > 0 alors tag_fermant > 0, c'est pourtant evident */
    if (fin_totoz && (tag_fermant > fin_totoz)) {
      /* Bon, on a trouve un tag zorelien a peu pres correct.
       * Maintenant, faut nettoyer de deb_totoz à fin_totoz
       * et de tag_fermant a tag_fermant+6
       */
      message = str_ncat(message, not_parsed, deb_totoz - not_parsed); /* Avant le tag : copie */
      message = str_ncat(message, fin_totoz, tag_fermant - fin_totoz); /* Le [:totoz]  : copie */
      not_parsed = tag_fermant+6; /* Pas de [:totoz] dans l'intervalle, on saute */
    }
    else if (tag_fermant && fin_totoz) {
      /* Le tag semble en vrac mais on va quand même regarder la suite si elle existe */
      message = str_ncat(message, not_parsed, (tag_fermant+6) - not_parsed);
      not_parsed = tag_fermant + 6;
    }
    else if (fin_totoz && (NULL == tag_fermant)) {
      /* Il n'y a pas de tag de fermeture, tant pis, on supprime celui d'ouverture. */
      message = str_ncat(message, not_parsed, deb_totoz - not_parsed); /* Avant le tag : copie */
      message = str_cat(message, fin_totoz); /* on copie la fin du message */
      not_parsed = NULL;
    }
    else {
      /* C'est pas la merde maintenant, on n'a meme pas de fin de tag. */
      message = str_cat(message, not_parsed); /* On n'a plus qu'à copier la fin du message. */
      not_parsed = NULL;
    }
    /* Si on est la fin (cas nominal ou tag en vrac) */
    if (not_parsed && ((*not_parsed) == '\0')) {
      not_parsed = NULL;
    }
  }
	/* S'il reste une partie non lue, c'est qu'il n'y a plus d'horloge dedans, on la copie */
	if (not_parsed) message = str_cat(message, not_parsed);
  /* On efface le message d'origine et on le remplace par la version epuree */
  free(*pmessage);
  *pmessage = message;
}

typedef struct URLhash {
  unsigned char md5digest[16];
  struct URLhash *next;
  id_type id;
  unsigned cnt;
} URLhash;

#define URL_HASH_SZ 233
URLhash *urlh[URL_HASH_SZ];

static URLhash *
logged_urls_find_url_(unsigned char *url_, unsigned char digest[16], unsigned *pnn) {
  /* on canonise l'url , c'est un hommage a jean-paul II */
  unsigned char url[2048]; strncpy(url,url_,2047); url[2047] = 0;
  url_au_coiffeur(url,0);
  str_tolower(url);
  /* remplace les %xx par le car correspondant */
  unsigned i=0,j=0;
  while (url[i]) {
    if (url[i] != '%' || url[i+1] == '%') url[j++] = url[i++];
    else {
      unsigned v; sscanf(url+i+1, "%02x", &v); if (v < 32) v = ' '; url[j] = v;
      i+=3; j++;
    }
  }

  /* insert your comment here */
  md5_digest(url, digest);
  unsigned nn = (digest[0] + digest[1]*256 + digest[2] * 65536)%URL_HASH_SZ;
  URLhash *h = urlh[nn];
  while (h) {
    if (memcmp(digest, h->md5digest, 16) == 0) break;
    h = h->next;
  }
  if (pnn) *pnn = nn;
  return h;
}

static int
logged_urls_add_url(id_type id, unsigned char *url) {
  unsigned char digest[16];
  unsigned nn;
  URLhash *h = logged_urls_find_url_(url, digest, &nn);
  if (h) { h->cnt++; return h->cnt-1; }
  ALLOC_OBJ(h, URLhash);
  h->next = urlh[nn]; urlh[nn] = h;
  h->id = id;
  h->cnt = 1;
  memcpy(h->md5digest, digest, 16);
  return 0;
}

/* utilise pour l'antibloub */
int 
logged_urls_find_url(unsigned char *url, id_type *pid) {
  unsigned char digest[16];
  URLhash *h = logged_urls_find_url_(url, digest, NULL);
  if (h) {
    if (pid) *pid = h->id;
    return 1;
  } else return 0;
}

/* stocke les md5 des urls recentes */
static void
log_urls(char *msg, id_type id)
{
  unsigned char url[2048];
  for ( ; *msg; ++msg) {
    if (*msg == '\t' && strncasecmp(msg, "\t<a href=\"", 10) == 0) {
      unsigned char *deb_url, *fin_url, *tag_fermant;
      deb_url = msg+10;
      fin_url = strstr(deb_url, "\t>");
      tag_fermant = strstr(deb_url, "\t</a");
      if (deb_url && fin_url && tag_fermant && (tag_fermant > fin_url)) {
        while (fin_url > deb_url+1 && *fin_url != '"') fin_url--;
        unsigned n = MIN(fin_url-deb_url,2047);
        strncpy(url, deb_url, n); url[n] = 0;
        unsigned cnt = logged_urls_add_url(id, url);
        BLAHBLAH(2,myprintf("nouvelle URL: '%<grn %s>' : %d\n", url, cnt));
      }
    }
  }
}


inline static void
mi_check_boitakon(Boards *boards, board_msg_info *mi)
{
  int i, one_ref_in_bak, all_refs_in_bak;
  KeyList *hk;

  hk = board_key_list_test_mi_num(boards, mi, Prefs.plopify_key_list, 2);
  mi->in_boitakon = 0;
  mi->contagious_boitakon = 0;
  if (hk) { /* bienvenu dans la boitakon */
    mi->in_boitakon = 1;
    if (hk->num == 3) mi->contagious_boitakon = 1;
  }

  /* maintenant on verifie si il repond à une boitakon contagieuse */
  one_ref_in_bak = 0;
  all_refs_in_bak = 1;
  for (i=0; i < mi->nb_refs; i++) {
    int j;
    board_msg_info *mi2;
    mi2 = mi->refs[i].mi;
    for (j=0; mi2 && j < mi->refs[i].nbmi; j++) {
      if (mi2->contagious_boitakon) one_ref_in_bak = 1;
      else all_refs_in_bak = 0;
    }
  }
  if (one_ref_in_bak) {
    mi->in_boitakon = Prefs.hungry_boitakon ? 1 : (all_refs_in_bak || mi->in_boitakon);
    mi->contagious_boitakon = mi->in_boitakon;
  }
}

/*
  comme son nom l'indique ..
*/
void
boards_update_boitakon(Boards *boards)
{
  board_msg_info *mi = boards->first;
  while (mi) {
    mi_check_boitakon(boards, mi);
    mi = mi->g_next;
  }
  flag_board_updated = 1;
}

/* la chasse au bug chelou est ouverte */
void
assert_boards_ok(Boards *boards) {
  int pid = -1,i, ptab[MAX_SITES],sid;
  board_msg_info *mi;

  if (boards->last) {
    assert(boards->last->next == NULL);
    assert(boards->last->g_next == NULL);
  }
  if (boards->first) {
    assert(boards->first->g_prev == NULL);
  }
  for (i = 0; i < MAX_SITES; ++i) {
    Board *board = boards->btab[i];
    if (board) {
      pid = -1;
      for (mi = board->msg; mi; mi = mi->next) {
        assert(id_type_lid(mi->id)>pid);
        pid = id_type_lid(mi->id);
      }
    }
  }
  for (i = 0; i < MAX_SITES; ++i) ptab[i] = -1;
  for (mi = boards->first; mi; mi = mi->g_next) {
    sid = id_type_sid(mi->id);
    assert(ptab[sid] < id_type_lid(mi->id));
    ptab[sid] = id_type_lid(mi->id);
    if (mi->g_next) assert(mi->g_next->g_prev == mi);
  }
  for (i = 0; i < MAX_SITES; ++i) ptab[i] = 2000000000;
  for (mi = boards->last; mi; mi = mi->g_prev) {
    sid = id_type_sid(mi->id);
    assert(ptab[sid] > id_type_lid(mi->id));
    ptab[sid] = id_type_lid(mi->id);
    if (mi->g_prev) assert(mi->g_prev->g_next == mi);
  }
}

/*
  enregistre un nouveau message
*/
board_msg_info *
board_log_msg(Board *board, char *ua, char *login, char *stimestamp, char *_message, int id, 
              const unsigned char *my_useragent)
{
  board_msg_info *nit, *pit, *it;
  board_msg_info *g_it, *pg_it;
  char *message = NULL;
  Boards *boards = board->boards;

  message = nettoie_message_tags(_message);

  do_url_replacements(&message);
  /* Triton> Le slip de zorel< est plein de morceaux */
  do_clean_zorel_clocks(&message);
  do_clean_zorel_totozz(&message);

  char *saved_message = strdup(message);

  /* emulation du wiki (en insérant les bons tags dans le message) */
  if (board->site->prefs->board_wiki_emulation) {
    char *tmp = message;
    int sz;
    sz = do_wiki_emulation(board, tmp, NULL); 
    message = malloc(sz); 
    do_wiki_emulation(board, tmp, message);
    free(tmp);
  }
#ifdef WMCC_EXTRA_CHECKS
  assert_boards_ok(boards);
#endif
  /*if (Prefs.verbosity >= 0) {
    int i; 
    printf("log new message MD5="); 
    for (i=0; i < 16; ++i) myprintf("%<CYA %02X>", md5[i]);
    printf(") tstamp = %s login=%s", stimestamp, login);
    if (Prefs.verbosity > 3) {
      printf(_(" content: '%s'\n"), message);
    } else printf("\n");
  }
  */
  nit = board->msg;
  pit = NULL;
  while (nit) {
    if (nit->id.lid > id) {
      break;
    }
    pit = nit;
    nit = nit->next;
  }

  it = (board_msg_info*) malloc(sizeof(board_msg_info)+strlen(ua)+1+strlen(message)+1+strlen(login)+1);
  assert(str_to_time_t(stimestamp, &it->timestamp));
  it->sub_timestamp = -1;
  it->useragent = ((char*)it) + sizeof(board_msg_info);
  it->msg = ((char*)it) + sizeof(board_msg_info) + strlen(ua) + 1;
  it->login = it->msg + strlen(message) + 1;
  it->ri = NULL;
  it->in_boitakon = 0; /* voir plus bas */
  it->left = NULL; it->right = NULL;

  /* insere le message dans la liste de la tribune locale */
  it->next = nit;
  if (pit) {
    pit->next = it;
  } else {
    board->msg = it;
  }
  id_type_set_lid(&it->id, id);
  id_type_set_sid(&it->id, board->site->site_id);
  
  /* insertion dans la grande chaine de messages globale (inter-sites)*/
  g_it = pit ? pit : boards->first; /* on demarre sur le dernier message de la 
                                       même tribune d'id inférieur
                                       histoire de respecter inconditionnellement 
                                       l'ordre par tribune
                                       (sinon il y a des problèmes quand 
                                       un backend laggue et qu'il y
                                       a des sauts de 'time_shift' */
  pg_it = g_it ? g_it->g_prev : NULL;
  while (g_it &&
         (it->timestamp +  board->boards->btab[it->id.sid]->time_shift >= 
          g_it->timestamp +board->boards->btab[g_it->id.sid]->time_shift)) {
    if (it->id.sid == g_it->id.sid && it->id.lid < g_it->id.lid) 
      break; /* respect de l'ordre sur le site */
    pg_it = g_it;
    g_it = g_it->g_next;
  }

  if (pg_it) {
    pg_it->g_next = it;
  } else {
    boards->first = it;
  }
  it->g_prev = pg_it;
  if (g_it) {
    g_it->g_prev = it;
  } else {
    boards->last = it;
  }
  it->g_next = g_it;
#ifdef WMCC_EXTRA_CHECK
  assert_boards_ok(boards);
#endif

  if (board->last_post_id < it->id.lid) {
    board->last_post_timestamp = it->timestamp;
    board->last_post_id = it->id.lid;
    board->last_post_time[0] = stimestamp[8];
    board->last_post_time[1] = stimestamp[9];
    board->last_post_time[2] = ':';
    board->last_post_time[3] = stimestamp[10];
    board->last_post_time[4] = stimestamp[11];
  }

  it->hmsf[0] = (stimestamp[8]-'0')*10 + (stimestamp[9]-'0');
  it->hmsf[1] = (stimestamp[10]-'0')*10 + (stimestamp[11]-'0');
  it->hmsf[2] = (stimestamp[12]-'0')*10 + (stimestamp[13]-'0');
  it->hmsf[3] = 1;

  strcpy(it->useragent, ua);
  strcpy(it->msg, message);
  strcpy(it->login, login);

  it->nb_refs = 0;
  it->refs = NULL; /* ça sera traité un peu plus tard */

  BLAHBLAH(3, myprintf("log msg id=%d, login=%s timestamp=%u msg='%<YEL %s>'\n", id, it->login, (unsigned)it->timestamp, it->msg));

  /* et on n'oublie pas..*/
  board->nbsec_since_last_msg = 0;

  /* remise a jour du flag d'affichage des secondes */
  update_secondes_flag(board);

  /* essaye de detecter si vous êtes l'auteur du message */
  if (id == board->last_posted_id) {
    it->is_my_message = 1;
  } else if (board->site->prefs->user_login && board->site->prefs->user_login[0] && board->just_posted_anonymous == 0) {
    it->is_my_message = !strcasecmp(board->site->prefs->user_login, it->login);
  } else {
    /* special pour les sites qui rajoutent des trucs à la fin,
       on limite la longueur de la comparaison */
    it->is_my_message = my_useragent && !strncmp(my_useragent, it->useragent, board->site->prefs->palmi_ua_max_len-1);
/*    if (it->is_my_message) {
      myprintf("my_message: '%<yel %s>' == '%<grn %s>'\n", it->useragent, my_useragent);
    }*/
  }
  board->just_posted_anonymous = 0;


  it->is_answer_to_me = 0;

  /* essaye d'identifier le user agent */
  miniua_eval_from_ua(&Prefs.miniuarules, it);

  /* evalue le potentiel trollesque */
  if (id_type_lid(it->id) == 9348) {
    printf("attention chérie ça va couper\n");
  }

  mi_check_boitakon(board->boards, it);
  if (it->in_boitakon) { /* bienvenu dans la boitakon */
    BLAHBLAH(2, myprintf(_("Welcome to the message from '%.20s' in the boitakon\n"), it->login ? it->login : it->useragent));
  }

  /* oui faire ça ici c'est pas efficace, surtout quand le coincoin démarre
     et qu'il ajoute 100 messages d'un coup -> on va refaire 100 fois l'arbre

     mais pour l'instant, ça reste comme ça (c'est quand même très peu couteux finalement)

     et comme le pinnipede peut etre mis a jour en plein milieu du d/l du backend,
     ça évite toute inconsistence entre la structure de liste des messages, et
     celle d'arbre */
     
  board_build_tree(board);

  /* attention le troll detector prend du temps .. du coup on vire le flag de toute
     façon il n'a aucun impact ici */
  flag_updating_board--;
  troll_detector(it);
  flag_updating_board++;

  if (board_is_regular_board(board))
    log_urls(saved_message, it->id);

  free(saved_message);
  free(message);
  return it;
}

/* 
   renvoie le nombre moyen de messages postes sur la tribune libre au
   cours des derniere 'trollo_log_extent' secondes
*/
void
board_get_trollo_rate(const Board *board, float *trollo_rate, float *trollo_score)
{
  board_msg_info *it;
  int cnt;
  
  it = board->msg;

  *trollo_score = 0.;
  cnt = 0;
  while (it) {
    int age;
    float coef;
    
    age = board_get_msg_age(board, it);
    //    printf("id=%d, %d , age=%d\n", it->id.lid, cnt, board_get_msg_age(board, it));
    if (board_get_msg_age(board, it) <= trollo_log_extent*60) {
      cnt++;
      coef = 0.1 * (trollo_log_extent*60 - age)/((float)(trollo_log_extent*60));
      *trollo_score += (it->troll_score)*coef;
    }
    it = it->next;
  }
  /* renvoie le nb de messages par minute */
  *trollo_rate = (float)cnt/(float)trollo_log_extent;

}

static void
board_do_balltrap(Board *board, int last_id) {
 board_msg_info *it = board->msg;
 BLAHBLAH(1, myprintf("board_call_external, id=%d - %d\n", last_id, board->last_post_id));
 if (last_id != -1) { /* si ce n'est pas le premier appel.. */
   it = board_find_id(board, last_id);
   if (it) it = it->next;
 } else {
   return; /* à l'initialisation, on de lacher un milliard de canards */
  }
  while (it) { 
    balltrap_check_message(it->id, it->msg);
    it = it->next;
  }
  balltrap_launch();
}
/*
  merci shift pour ce patch !

  appelle le programme externe (dans l'ordre des id) pour chaque nouveau message reçu
*/
static void
board_call_external_(Board *board, int last_id, char *cmd) {
  board_msg_info *it;
  BLAHBLAH(1, myprintf("board_call_external, id=%d - %d\n", last_id, board->last_post_id));
  if (last_id != -1) { /* si ce n'est pas le premier appel.. */
    it = board_find_id(board, last_id);
    if (it) it = it->next;
  } else {
    return; /* à l'initialisation, on évite de passer tous les messages dans le coincoin */
  }
  while (it) {
    char *qlogin;
    char *qmessage;
    char *qua;
    char *qhost;
    char sid[20], stimestamp[20], strollscore[20], *stypemessage,  stypemessage2[4];
    int typemessage;
    char *shift_cmd;

    const char *keys[] = {"$l", "$m", "$u", "$i", "$t", "$s", "$r", "$R", "$v","$h"};
    const char *subs[] = {  "",   "",   "",   "",   "",   "",   "", "", VERSION, ""};

    //----< Code pour passer les infos d'un post à une commande extérieure >

    qlogin = shell_quote(it->login);
    qmessage = shell_quote(it->msg);
    qua = shell_quote(it->useragent);
    qhost = shell_quote(board->site->prefs->site_name);
    snprintf(sid, 20, "%d", it->id.lid);
    snprintf(stimestamp, 20, "%lu", (unsigned long)it->timestamp);
    snprintf(strollscore, 20, "%d", it->troll_score);
    
    /* je garde ça pendant qqes version pour pas casser les bigornos, 
       mais l'option à utiliser dorénavant, c'est plutot $R */
    stypemessage = "0";
    if (it->is_my_message) stypemessage = "1";
    else if (it->is_answer_to_me) stypemessage = "2";
    else if (board_key_list_test_mi(board->boards, it, Prefs.hilight_key_list)) stypemessage = "3";
    else if (board_key_list_test_mi(board->boards, it, Prefs.plopify_key_list)) stypemessage = "4";

    /* pour $R */
    typemessage = 0;
    if (it->is_my_message) typemessage |= 1;
    if (it->is_answer_to_me) typemessage |= 2;
    if (board_key_list_test_mi(board->boards, it, Prefs.hilight_key_list)) typemessage |= 4;
    if (board_key_list_test_mi(board->boards, it, Prefs.plopify_key_list)) typemessage |= 8;
    snprintf(stypemessage2, 4, "%d", typemessage);

    subs[0] = qlogin;
    subs[1] = qmessage;
    subs[2] = qua;
    subs[3] = sid;
    subs[4] = stimestamp;
    subs[5] = strollscore;
    subs[6] = stypemessage;
    subs[7] = stypemessage2;
    subs[9] = qhost;
    shift_cmd = str_multi_substitute(cmd, keys, subs, 10);
    BLAHBLAH(2, myprintf("post_cmd: /bin/sh -c %<YEL %s>\n", shift_cmd));
    if (system(shift_cmd) == -1)
      myfprintf(stderr, "%s failed\n", shift_cmd);

    free(shift_cmd);
    free(qlogin);
    free(qmessage);
    free(qua);
    free(qhost);
    //----</ Code >
    it = it->next;

    ALLOW_X_LOOP;
  }
}

static void
board_call_external(Board *board, int last_id)
{
  int i;
  for (i=0; i < NB_BIGORNO; ++i) {
    if (Prefs.post_cmd[i] && Prefs.post_cmd_enabled[i]) {
      board_call_external_(board,last_id,Prefs.post_cmd[i]);
    }
  }
}




/*
  detecte les refs par petites horloges
  (mais ne tente rien pour ipot)
*/
void
board_check_my_messages(Board *board, int old_last_post_id) { 
  if (board->last_post_id != old_last_post_id) { /* si de nouveaux messages ont été reçus */
    board_msg_info *it;
    /* truc batard ci-dessous... il faudrait faire mieux */
    char *callme = NULL;
    if (!str_is_empty(board->site->prefs->user_login)) callme = str_printf("%s<", board->site->prefs->user_login);

    /* essaye de detecter si il s'agit d'une réponse à un de vos messages 
     */
    if (old_last_post_id != -1) { /* si ce n'est pas le premier appel.. */
      it = board_find_id(board, old_last_post_id);
      if (it) it = it->next;
    } else {
      it = board->msg;
    }
    while (it) {
      flag_updating_board++;
      board_msg_find_refs(board, it); // rhoo il etait bien caché cet appel sournois
      mi_check_boitakon(board->boards, it); /* ce deuxième appel est un peu redondant
					       mais il permet de s'assurer de la contagion des
					       boitakon */
      flag_updating_board--;

      if (!it->in_boitakon) {
        if (board_msg_is_ref_to_me(board->boards, it) ||
            (callme && str_case_str(it->msg, callme))) {
          flag_updating_board++;
          it->is_answer_to_me = 1;
          flag_updating_board--;
          if (old_last_post_id != -1 && !it->is_my_message) board->flag_answer_to_me = 1;
        }
      }
      it = it->next;
    }
    FREE_STRING(callme);
  }
}


void
cctime(const time_t* t, char *s)
{
  sprintf(s, "%02ld:%02ld:%02ld", (*t/3600)%24, (*t/60)%60, *t%60);
}
/*
  determination approximative du décalage horaire 
*/
static void
board_update_time_shift(Board *board, int old_last_post_id) { 
  if (!board_is_regular_board(board)) {
    board->time_shift_min = board->time_shift_max = 0; // fait chier sinon
  } else if (board->last_post_id != old_last_post_id) { /* si de nouveaux messages ont été reçus */
    char s1[15],s2[15];
    board_msg_info *it;
    time_t t_min = 0, t_max = 0;
    int nbmsg = 1;
    /* essaye de detecter si il s'agit d'une réponse à un de vos messages 
     */
    if (old_last_post_id == -1) { /* si ce n'est pas le premier appel.. */
      return;
    }
    it = board_find_id(board, old_last_post_id);
    if (it && it->next) {
      it = it->next;      
      t_min = it->timestamp;
      t_max = it->timestamp;
      it = it->next;
    } else 
      return;

    while (it) {
      t_min = MIN(t_min, it->timestamp);
      t_max = MAX(t_max, it->timestamp);
      nbmsg ++;
      it = it->next;
    }


    if (t_max - t_min > board->local_time_last_check_end-board->local_time_last_check_old) {
      BLAHBLAH(1,myprintf("%<YEL ------------------------------\nle backend LAGGUE !!\n----------------->\n"));
    }
    board->time_shift_min = MAX(board->time_shift_min, 
				board->local_time_last_check_old - t_min);
    board->time_shift_max = MIN(board->time_shift_max, 
				board->local_time_last_check_end - t_max);

    if (board->time_shift_min == board->time_shift_max) {
      board->time_shift_min--;
      board->time_shift_max++;
    }

    if (board->time_shift_min >  board->time_shift_max) {
      int marge;
      time_t tmp;
      if (board->time_shift_min - board->time_shift_max > 1) {
	BLAHBLAH(1,myprintf("%<YEL ------------------------------\nRAAAAAAAAAAAAH SWAP!!\n----------------->\n"));
	marge = 10;
      } else {
	BLAHBLAH(1,myprintf("%<YEL petit ajustement>\n"));
	marge = 1;
      }
      tmp = board->time_shift_min;
      board->time_shift_min = board->time_shift_max-marge;
      board->time_shift_max = tmp+marge;
    }


    board->time_shift = (board->time_shift_min+board->time_shift_max)/2;
    
    if (Prefs.verbosity >= 1) {
      myprintf("%<YEL %s>\n", board->site->prefs->site_name);
      cctime(&t_min,s1); cctime(&t_max,s2);
      printf("t_min : %s, t_max : %s, d=%ld nbmsg=%d\n", s1, s2, 
	     t_max-t_min, nbmsg);
    
      cctime(&board->local_time_last_check_old,s1); cctime(&board->local_time_last_check_end,s2); 
      printf("loct1 : %s, loct2 : %s, d=%ld\n", s1, s2,
	     board->local_time_last_check_end-board->local_time_last_check_old);
      printf("time_shift_min : %ld, time_shift_max : %ld, ts=%ld\n", 
	     board->time_shift_min, board->time_shift_max,board->time_shift);
    }
  }
}

/* decodage du message, quel que soit l'état du backend .. */
void
board_decode_message(Board *board, char *dest, const char *src) {
  strncpy(dest, src, BOARD_MSG_MAX_LEN); dest[BOARD_MSG_MAX_LEN-1] = 0;
  if (board->site->prefs->backend_flavour == BACKEND_FLAVOUR_UNENCODED) {
    mark_html_tags(dest, BOARD_MSG_MAX_LEN);
  }
  convert_to_ascii(dest, dest, BOARD_MSG_MAX_LEN);
  if (board->site->prefs->backend_flavour == BACKEND_FLAVOUR_ENCODED) {
    mark_html_tags(dest, BOARD_MSG_MAX_LEN);    
    convert_to_ascii(dest, dest, BOARD_MSG_MAX_LEN);
  }

  /* cette partie est destinée a etre modifiée à chaque fois que le backend 
     sera dans un état pas presentable à un validator, 
     par ex. un backend avec des poils qui depassent du string */
  if (board->site->prefs->backend_flavour == BACKEND_FLAVOUR_NO_PANTS) {
    char *s, *s2;
    
    s = strdup(dest); assert(s);
    s2 = str_substitute(s, "</bgt;", "\t</b\t>"); free(s); s = s2; /* special super bug.. */
    s2 = str_substitute(s, "<b>", "\t<b\t>"); free(s); s = s2;
    s2 = str_substitute(s, "</b>", "\t</b\t>"); free(s); s = s2;
    s2 = str_substitute(s, "<i>", "\t<i\t>"); free(s); s = s2;
    s2 = str_substitute(s, "</i>", "\t</i\t>"); free(s); s = s2;
    s2 = str_substitute(s, "<u>", "\t<u\t>"); free(s); s = s2;
    s2 = str_substitute(s, "</u>", "\t</u\t>"); free(s); s = s2;
    s2 = str_substitute(s, "<s>", "\t<s\t>"); free(s); s = s2;
    s2 = str_substitute(s, "</s>", "\t</s\t>"); free(s); s = s2;
    s2 = str_substitute(s, "<a ", "\t<a "); free(s); s = s2;
    s2 = str_substitute(s, "\">", "\"\t>"); free(s); s = s2;
    s2 = str_substitute(s, "</a>", "\t</a\t>"); free(s); s = s2;
    s2 = str_substitute(s, "<clock time", "\t<clock time"); free(s); s = s2;
    s2 = str_substitute(s, "</clock>", "\t</clock\t>"); free(s); s = s2;
    s2 = str_substitute(s, "<!--", "\t<!--"); free(s); s = s2;
    s2 = str_substitute(s, "-->", "--\t>"); free(s); s = s2;
    strncpy(dest, s, BOARD_MSG_MAX_LEN); dest[BOARD_MSG_MAX_LEN-1] = 0; free(s);
    convert_to_ascii(dest, dest, BOARD_MSG_MAX_LEN); /* deuxième passe, à tout hasard */
  }
  BLAHBLAH(4,myprintf(_("Original message: '%<CYA %s>'\n"), src));
  BLAHBLAH(4,myprintf(_("Decoded message: '%<MAG %s>'\n"), dest));
}

/* bou comme c'est laid */
int 
http_get_line_and_convert(HttpRequest *r, char *s, size_t sz, const char *encoding) {
  int cnt = http_get_line_trim(r, s, sz);
  if (cnt) {
    char *w = strdup(s); assert(w);
    convert_to_utf8(encoding, &w);
    strncpy(s,w,sz); s[sz-1] = 0;
    free(w);
  }
  return cnt;
}

int
regular_board_update_xml(Board *board, HttpRequest *r) {
  int http_err_flag = 0;
  char *errmsg = NULL;
  char s[16384]; /* must be large enough to handle very long lines
		    (especially with broken backends, yes it happens sometimes) */
  const char *my_useragent = board->coin_coin_useragent;

  /* 
     première ligne : on essaye de chopper l'encoding -- du coup, ça devrait assurer une
     relative compatibilité avec les tribunes en UTF-8 ou autre.
  */
  if (http_get_line_trim(r, s, 16384)) {
    XMLBlock xmlb;
    int pos;
    clear_XMLBlock(&xmlb);
    if ((pos = get_XMLBlock(s, strlen(s), "?xml", &xmlb))>=0) {
      XMLAttr *a;
      int found = 0;
      if (board->encoding) free(board->encoding);
      for (a = xmlb.attr; a; a = a->next) {
        if (str_case_startswith(a->name, "encoding")) {
          board->encoding = str_ndup(a->value,a->value_len);
          BLAHBLAH(1,printf("%s: found encoding: value = '%s'\n", board->site->prefs->site_name, board->encoding));
          found = 1;
          break;
        }
      }
      if (!found) board->encoding = strdup("UTF-8"); /* defaut si pas d'encoding specifie */
    }
    destroy_XMLBlock(&xmlb);
  }

  strbuf sb; strbuf_init(&sb, "");
  while (http_is_ok(r) && !errmsg) {
    XMLBlock post; clear_XMLBlock(&post);
    int ok = 0;

    sb.len = 0; /* petit coup de flemme : ça va chier si on 
                   enchaine les posts sur une même ligne */
    while (http_get_line_and_convert(r, s, sizeof s,board->encoding) > 0 && http_is_ok(r) && sb.len < 500000) {
      strbuf_cat(&sb, s);
      if (get_XMLBlock(sb.str, sb.len, "post", &post)>=0) {
        ok = 1;
        break;
      }
    }
    if (!ok) break;

    char stimestamp[15];
    char ua[BOARD_UA_MAX_LEN];
    char msg[BOARD_MSG_MAX_LEN];
    char login[BOARD_LOGIN_MAX_LEN];
    int id;
    stimestamp[0] = ua[0] = msg[0] = login[0] = 0; id = -1;
    
    BLAHBLAH(3, myprintf("got new post: %s\n", post.content));

    XMLAttr *a = post.attr;
    while (a) {
      unsigned l = MIN(a->name_len, (int)((sizeof s) - 1));
      strncpy(s, a->name, l); s[l] = 0;
      if (strcasecmp(s, "time")==0) {
        unsigned l = MIN((sizeof stimestamp)-1,(unsigned)a->value_len);
        strncpy(stimestamp, a->value, l); stimestamp[l] = 0;
      } else if (strcasecmp(s, "id")==0) {
        if (a->value_len == 0 || !isdigit(a->value[0]))
          id = -1000;
        else id = atoi(a->value);
      }
      a = a->next;
    }
      
    XMLBlock xmlb; clear_XMLBlock(&xmlb);
    if (get_XMLBlock(post.content, post.content_len, "login",&xmlb)) {
      unsigned l = MIN(BOARD_LOGIN_MAX_LEN-1, xmlb.content_len);
      strncpy(login, xmlb.content, l); login[l]=0;
      if (strcasecmp(login, "Anonyme") != 0) {
        convert_to_ascii(login, login, BOARD_LOGIN_MAX_LEN);
      } else login[0] = 0;
    }
    if (get_XMLBlock(post.content, post.content_len, "info",&xmlb)) {
      unsigned l = MIN(BOARD_UA_MAX_LEN-1, xmlb.content_len);
      strncpy(ua, xmlb.content, l); ua[l]=0;
      convert_to_ascii(ua, ua, BOARD_UA_MAX_LEN);
    }
    if (get_XMLBlock(post.content, post.content_len, "message",&xmlb)) {
      char *p = str_ndup(xmlb.content, xmlb.content_len); assert(p);
      int i;
      /* nettoyage des codes < 32 dans le message */
      for (i=0; i < xmlb.content_len; ++i) {
        if ((unsigned char)p[i] < ' ') 
          p[i] = ' ';
      }
      board_decode_message(board, msg, p);
      free(p);
    } else errmsg = "no <message> tag!";
    destroy_XMLBlock(&xmlb);
    
    if (!errmsg && strlen(stimestamp) < 14) {
      fprintf(stderr,"timestamp POURRI: '%s'\n",stimestamp); 
      errmsg = "slip woof?"; 
    }
    if (!errmsg && id < 0) { 
      errmsg="id sgn"; 
    }
    destroy_XMLBlock(&post);
    if (board_find_id(board,id)) break;
    if (!errmsg) { /* encore une victoire de xmlcoincoin */
      flag_updating_board++;
      if (!board_log_msg(board, ua, login, stimestamp, 
                         msg, id, my_useragent)->in_boitakon) {
        board->nb_msg_at_last_check++;
        if (id > board->last_viewed_id) {
          board->nb_msg_since_last_viewed++;
        }
      }
      flag_updating_board--;
      BLAHBLAH(1, myprintf("[%<YEL %s>] board_update: "
                           "last_post_time=%5s - last_post_id=%d\n",
                           board->site->prefs->site_name, 
                           board->last_post_time, id));
    } else if (errmsg) {
      myfprintf(stderr, _("[%<YEL %s>] There is a problem in '%s', "
                          "I can't parse it... error:%<YEL %s>\n"),
                board->site->prefs->site_name,
                board->site->prefs->backend_url, errmsg);
    }
  }
  strbuf_free(&sb);

  if (!http_is_ok(r)) { 
    http_err_flag = 1;
    myfprintf(stderr, _("[%<YEL %s>] Error while downloading "
                        "'%<YEL %s>' : %<RED %s>\n"), 
	      board->site->prefs->site_name, 
              board->site->prefs->backend_url, http_error());
  }
  http_request_close(r);
  return http_err_flag;
}

int
regular_board_update_tsv(Board *board, HttpRequest *r) {
  int http_err_flag = 0;
  char *errmsg = NULL;
  char s[16384]; /* must be large enough to handle very long lines */
  const char *my_useragent = board->coin_coin_useragent;

  while (http_get_line(r, s, 16384) > 0 && http_is_ok(r)) {
    int id;
    char stimestamp[15];
    char ua[BOARD_UA_MAX_LEN];
    char msg[BOARD_MSG_MAX_LEN];
    char login[BOARD_LOGIN_MAX_LEN];
    stimestamp[0] = ua[0] = msg[0] = login[0] = 0; id = -1;

    int offset;
    unsigned int length;
    offset = 0;

    length = strcspn(s, "\t");
    if (length) {
      char field[15];
      strncpy(field, s, length);
      id = atoi(field);
      offset += length + 1;
    } else {
      errmsg = "no id!";
      offset++;
    }

    length = strcspn(s + offset, "\t");
    if (length) {
      unsigned l = MIN((sizeof stimestamp)-1, length);
      strncpy(stimestamp, s + offset, l); stimestamp[l] = 0;
      offset += length + 1;
    } else {
      errmsg = "no timestamp!";
      offset++;
    }

    length = strcspn(s + offset, "\t");
    if (length) {
      unsigned l = MIN((sizeof ua)-1, length);
      strncpy(ua, s + offset, l); ua[l] = 0;
      offset += length + 1;
    } else {
      offset++;
    }

    length = strcspn(s + offset, "\t");
    if (length) {
      unsigned l = MIN((sizeof login)-1, length);
      strncpy(login, s + offset, l); login[l] = 0;
      offset += length + 1;
    } else {
      offset++;
    }

    length = strcspn(s + offset, "\t");
    if (length) {
      unsigned l = MIN((sizeof msg)-1, length);
      strncpy(msg, s + offset, l); msg[l] = 0;
      offset += length + 1;
    }

    BLAHBLAH(3, myprintf("got new post: %s\n", msg));

    if (!errmsg && strlen(stimestamp) < 14) {
      fprintf(stderr, "timestamp POURRI: '%s'\n", stimestamp);
    }

    if (board_find_id(board, id)) break;

    if (!errmsg) { /* encore une victoire de xmlcoincoin */
      flag_updating_board++;
      if (!board_log_msg(board, ua, login, stimestamp,
                         msg, id, my_useragent)->in_boitakon) {
        board->nb_msg_at_last_check++;
        if (id > board->last_viewed_id) {
          board->nb_msg_since_last_viewed++;
        }
      }
      flag_updating_board--;
      BLAHBLAH(1, myprintf("[%<YEL %s>] board_update: "
                           "last_post_time=%5s - last_post_id=%d\n",
                           board->site->prefs->site_name, 
                           board->last_post_time, id));
    } else {
      myfprintf(stderr, _("[%<YEL %s>] There is a problem in '%s', "
                          "I can't parse it... error:%<YEL %s>\n"),
                board->site->prefs->site_name,
                board->site->prefs->backend_url, errmsg);
    }
  }

  if (!http_is_ok(r)) { 
    http_err_flag = 1;
    myfprintf(stderr, _("[%<YEL %s>] Error while downloading "
                        "'%<YEL %s>' : %<RED %s>\n"), 
	      board->site->prefs->site_name, 
              board->site->prefs->backend_url, http_error());
  }
  http_request_close(r);
  return http_err_flag;
}

int 
regular_board_update(Board *board, char *path) {
  HttpRequest r;
  wmcc_init_http_request_with_cookie(&r, board->site->prefs, path);
  /* Triton> Pour les commentaires, il faut voir au dessus */
  r.accept = strdup("text/tab-separated-values, text/xml");
  if (board->site->prefs->use_if_modified_since) { r.p_last_modified = &board->last_modified; }
  http_request_send(&r);
  wmcc_log_http_request(board->site, &r);

  if (r.content_type && strncmp(r.content_type, "text/tab-separated-values", 25) == 0) {
    fprintf(stderr, "parsing backend %s\n", r.content_type);
	  return regular_board_update_tsv(board, &r);
  } else {
	  return regular_board_update_xml(board, &r);
  }
}


/*
  lecture des nouveaux messages reçus

  my_useragent: useragent dernièrement utilisé, sert à reconnaitre si on est 
  l'auteur d'un post, ou non
*/
void
board_update(Board *board)
{
  char path[2048];

  int old_last_post_id;

  int http_err_flag = 0;

  /* maj du nombre de secondes ecoulees depuis le dernier message recu
     (pour pouvoir calculer l'age des message -> on part du principe
     que l'horloge locale et l'horloge de linuxfr ne sont pas synchrones)
  */

  old_last_post_id = board->last_post_id;

  board->local_time_last_check_old = board->local_time_last_check;
  board->local_time_last_check = time(NULL);

  /* ça c'est pour le pinni */
  board->wmcc_tic_cnt_last_check = wmcc_tic_cnt;
  board->last_post_id_prev = old_last_post_id;

  board->nbsec_since_last_msg += difftime(board->local_time_last_check, board->local_time_last_check_old);
  /* des fois qu'une des 2 horloges soit modifie a l'arrache */
  board->nbsec_since_last_msg = MAX(board->nbsec_since_last_msg,0);
  board->nb_msg_at_last_check = 0;

  if ((Prefs.debug & 2) == 0) {
    snprintf(path, 2048, "%s", board->site->prefs->backend_url);
  } else {
    snprintf(path, 2048, "%s/wmcoincoin/test/%s/remote.xml", getenv("HOME"), board->site->prefs->site_name);
    myprintf(_("DEBUG: opening '%<RED %s>'\n"), path);
  }

  if (board_is_regular_board(board)) {
    pp_set_download_info(board->site->prefs->site_name, "updating board");
    http_err_flag = regular_board_update(board, path);
  } else if (board_is_rss_feed(board)) {
    pp_set_download_info(board->site->prefs->site_name, "updating RSS");
    http_err_flag = rss_board_update(board,path);
  } else if (board_is_pop3(board)) {
    pp_set_download_info(board->site->prefs->site_name, "updating POP3");
    http_err_flag = pop3_board_update(board,path);
  }
  if (http_err_flag) {
    board->site->http_error_cnt++;
    board->site->http_recent_error_cnt++;
  } else {
    board->site->http_success_cnt++;
    board->site->http_recent_error_cnt = 0;
  }

  board->local_time_last_check_end = time(NULL);

  /* cleanup .. */
  flag_updating_board++;
  board_remove_old_msg(board);
  flag_updating_board--;


  board_check_my_messages(board, old_last_post_id);
  board_update_time_shift(board, old_last_post_id);

  pp_set_download_info(NULL, NULL);

  flag_board_updated = (board->last_post_id != old_last_post_id) ? 1 : 0;  

  if (board->last_post_id != old_last_post_id) { /* si de nouveaux messages ont été reçus */
    board_call_external(board, old_last_post_id);    
    if (board_is_regular_board(board)) board_do_balltrap(board, old_last_post_id);
  }

}
