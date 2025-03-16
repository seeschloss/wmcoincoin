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
  fonctions diverses sur la tribune

  rcsid=$Id: board_util.c,v 1.19 2004/04/18 15:37:28 pouaite Exp $
  ChangeLog:
  $Log: board_util.c,v $
  Revision 1.19  2004/04/18 15:37:28  pouaite
  un deux un deux

  Revision 1.18  2004/03/03 23:00:39  pouaite
  commit du soir

  Revision 1.17  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.16  2003/08/26 21:50:48  pouaite
  2.6.4b au mastic

  Revision 1.15  2003/06/29 23:58:37  pouaite
  suppression de l'overrideredirect du palmi et ajout de pinnipede_totoz.c et wmcoincoin-totoz-get etc

  Revision 1.14  2003/06/09 16:42:29  pouaite
  pan pan

  Revision 1.13  2003/03/02 14:41:22  pouaite
  ce commit est d�di� � la m�moire de jacques martin

  Revision 1.12  2003/02/17 19:16:58  pouaite
  fix urls relatives pour la tribune

  Revision 1.11  2002/11/11 15:26:38  pouaite
  fix soulignement et strike avec les span

  Revision 1.10  2002/10/05 18:08:14  pouaite
  ajout menu contextuel + fix de la coloration des boutons du wmccc

  Revision 1.9  2002/09/25 22:02:15  pouaite
  hungry boitakon

  Revision 1.8  2002/09/07 16:21:15  pouaite
  �a va releaser en douce

  Revision 1.7  2002/09/01 23:54:56  pouaite
  completurage du wmc3 et compatibilitation avec new.linuxfr

  Revision 1.6  2002/08/31 21:26:46  pouaite
  ajout du wmccc

  Revision 1.5  2002/08/23 00:25:21  pouaite
  ou�

  Revision 1.4  2002/08/21 20:22:16  pouaite
  fix compil

  Revision 1.3  2002/08/18 19:00:28  pouaite
  plop

  Revision 1.2  2002/08/18 00:29:30  pouaite
  en travaux .. pri�re de porter le casque

  Revision 1.1  2002/08/17 18:33:38  pouaite
  grosse commition

  Revision 1.26  2002/06/23 14:01:36  pouaite
  ouups, j'avais flingu� les modifs depuis la v2.3.8b

  Revision 1.25  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.23  2002/05/28 23:22:58  pouaite
  ptit fix

  Revision 1.22  2002/05/28 20:11:55  pouaite
  modif pr un pinnipede + fluide qd il y a bcp de messages stock�s + tribune sur plusieurs jours

  Revision 1.21  2002/04/26 04:45:51  pouaite
  reconnaissance des horloges suivies de 3 pts de suspension

  Revision 1.20  2002/04/24 19:44:00  pouaite
  option pinnipede.use_AM_PM pour les horloges � l'anglaise comme sur http://woof.lu

  Revision 1.19  2002/04/13 11:55:19  pouaite
  fix kde3 + deux trois conneries

  Revision 1.18  2002/04/11 23:16:54  pouaite
  boitakon mega combo

  Revision 1.17  2002/04/10 22:53:44  pouaite
  un commit et au lit

  Revision 1.16  2002/04/09 23:38:29  pouaite
  boitakon et son cort�ge de bugfixes

  Revision 1.15  2002/04/09 00:28:19  pouaite
  quelques modifs faites dans un �tat d'h�b�tude avanc� /!\ travaux en cours /!\

  Revision 1.14  2002/04/03 20:15:11  pouaite
  plop

  Revision 1.13  2002/04/01 22:56:03  pouaite
  la pseudo-transparence du pinni, bugfixes divers, option tribune.backend_type

  Revision 1.12  2002/03/27 20:45:06  pouaite
  deuxi�me vague de bugfix

  Revision 1.11  2002/03/09 00:25:12  pouaite
  coin coin

  Revision 1.10  2002/03/08 23:53:40  pouaite
  derniers bugfixes pour la v2.3.6

  Revision 1.9  2002/03/03 11:58:55  pouaite
  bugfix du crash avec les posts autoreferants (gniiii)

  Revision 1.8  2002/02/27 00:32:19  pouaite
  modifs velues

  Revision 1.7  2002/02/25 01:36:58  pouaite
  bugfixes pour la compilation

  Revision 1.6  2002/02/24 22:13:57  pouaite
  modifs pour la v2.3.5 (selection, scrollcoin, plopification, bugfixes)

  Revision 1.5  2002/01/31 23:45:00  pouaite
  plop

  Revision 1.4  2002/01/20 20:53:22  pouaite
  bugfix configure.in && http_win.c pour cygwin + 2-3 petis trucs

  Revision 1.3  2002/01/20 02:17:13  pouaite
  modifs d'ordre esthetique (!) sans grand interet

  Revision 1.2  2002/01/19 19:56:09  pouaite
  petits crochets pour la mise en valeur de certains messages (cf changelog)

  Revision 1.1  2002/01/18 19:45:58  pouaite
  petit oubli d'un fichier..

*/

#include "board_util.h"

#include <libintl.h>
#define _(String) gettext (String)


/* search an id in the tree : fast */
board_msg_info *
board_find_id(const Board *board, int64_t id)
{
  board_msg_info *it;

  it = board->mi_tree_root;

  while (it && it->id.lid != id) {
    if (id < it->id.lid)  it = it->left;
    else it = it->right;
  }
  return it;
}

/* search an md5 in the tree : exhaustive.. */
static board_msg_info *
board_find_md5_rec(board_msg_info *it, md5_byte_t md5[16]) {
  board_msg_info *it2;
  if (it->ri && memcmp(md5, it->ri->md5, sizeof *md5) == 0) return it;
  else if (it->left && (it2=board_find_md5_rec(it->left, md5))) return it2;
  else if (it->right && (it2=board_find_md5_rec(it->right, md5))) return it2;
  return NULL;
}
board_msg_info *
board_find_md5(const Board *board, md5_byte_t md5[16]) {
  if (board->mi_tree_root) 
    return board_find_md5_rec(board->mi_tree_root,md5);
  return NULL;
}

board_msg_info *
boards_find_id(const Boards *boards, id_type id)
{
  if (id_type_is_invalid(id)) {
    return NULL;
  } else {
    Board *b;

    assert((unsigned)id.sid < MAX_SITES);
    b = boards->btab[id.sid];
    assert(b);
    return board_find_id(b, id.lid);
  }
}

id_type
boards_last_id(const Boards *b) {
  if (b->last) return b->last->id;
  return id_type_invalid_id();
}

board_msg_info*
board_find_previous_from_id(const Board *board, int64_t id)
{
  board_msg_info *mi;
  board_msg_info *prev = NULL;

  mi = board->mi_tree_root;

  while (mi) {
    if (mi->id.lid < id) {
      prev = mi;
      mi = mi->right;
    } else {
      mi = mi->left;
    }
  }
  return prev;
}


board_msg_info *
board_find_previous(const Board *board, board_msg_info *mi)
{
  board_msg_info *it;

  it = board_find_previous_from_id(board, mi->id.lid);
  return it;
    /*
      board->msg; 
      while (it) {
      if (it->next == mi) return it;
      it = it->next;
      }
      return NULL;
    */
}


/*----------------------- fonctions key_list ---------------- */


/* supprime les keylist faisant ref � des messages detruits */
KeyList *
board_key_list_cleanup(Boards *boards, KeyList *first)
{
  KeyList *hk;
  hk = first;
  while (hk) {
    if (hk->type == HK_ID || hk->type == HK_THREAD) {
      id_type id = int_to_id_type(atoi(hk->key));
      if (boards_find_id(boards, id) == NULL) {
	first = key_list_remove(first, hk->key, hk->type);
	hk = first;
      }
    }
    if (hk) hk = hk->next;
  }
  return first;
}


/* fonction interne, appel�e par board_key_list_test_mi_hk */
static int
board_key_list_test_thread(Boards *boards, board_msg_info *mi, id_type thread_id, int *antibug)
{
  int i;
  (*antibug)++;
  if (*antibug > 10000) { printf(_("sniff? sniff ? soit vous vous amusez a faire de threads de folie, soit ca sent le BEUGGUE!\n")); return 0; }

  if (mi == NULL) return 0;

  mi->bidouille_qui_pue = 1;

  /*  printf("test: mi->id=%d, %d\n", mi->id, thread_id); */

  if (id_type_eq(mi->id,thread_id)) return 1;

  for (i = 0; i < mi->nb_refs; i++) {
    int j;
    board_msg_info *tmi;

    tmi = mi->refs[i].mi; j = 0;
    while (j < mi->refs[i].nbmi && tmi) {
      if (tmi->bidouille_qui_pue == 0 && board_key_list_test_thread(boards, tmi, thread_id, antibug)) return 1;
      tmi = tmi->next; j++;
    }
  }
  return 0;
}

static int
board_key_list_test_mi_hk(Boards *boards, board_msg_info *mi, KeyList *hk)
{
  if (hk->type == HK_UA) {
    if (strcmp(hk->key, mi->useragent) == 0) {
      return 1;
    }
  } else if (hk->type == HK_UA_NOLOGIN) {
    if (mi->login[0]==0 && strcmp(hk->key, mi->useragent) == 0) {
      return 1;
    }
  }  else if (hk->type == HK_LOGIN) {
    if (strcmp(hk->key, mi->login) == 0) {
      return 1;
    }
  } else if (hk->type == HK_WORD) {
    if (str_noaccent_casestr(mi->msg, hk->key)) {
      /* printf("mot clef %s trouv� dans le msg id=%d\n", hk->key, mi->id); */
      return 1;
    } 
  } else if (hk->type == HK_ID) {
    char sid[10];
    snprintf(sid,10, "%d", id_type_to_int(mi->id));
    if (strcmp(sid, hk->key) == 0) {
      return 1;
    }
  } else if (hk->type == HK_THREAD) {
    board_msg_info *tmi;
    id_type id;
    int antibug = 0;
 
    
   
    tmi = boards->first; while (tmi) { tmi->bidouille_qui_pue = 0; tmi = tmi->g_next; }	
    id = int_to_id_type(atoi(hk->key));
    
    if (board_key_list_test_thread(boards, mi, id, &antibug)) { return 1; }
  }
  return 0;
}

KeyList *
board_key_list_test_mi(Boards *boards, board_msg_info *mi, KeyList *klist)
{
  KeyList *hk;
  char sid[10];

  if (mi == NULL) return NULL;
  snprintf(sid,10, "%d", id_type_to_int(mi->id));
  
  hk = klist;
  while (hk) {
    if (board_key_list_test_mi_hk(boards,mi,hk)) return hk;
    hk = hk->next;
  }
  return NULL;
}


KeyList *
board_key_list_test_mi_num(Boards *boards, board_msg_info *mi, KeyList *klist, int num)
{
  KeyList *hk;

  if (mi == NULL) return NULL;
  
  hk = klist;
  while (hk) {
    if (hk->num >= num)
      if (board_key_list_test_mi_hk(boards,mi,hk)) return hk;
    hk = hk->next;
  }
  return NULL;
}


/* construit recursivement et vilainement la liste des mots plops qui ont provoqu�
   une plopification recursive */
static void
board_key_list_get_recurs_plop_list(Boards *boards, board_msg_info *mi, KeyList **pin, KeyList **pout, int lvl)
{
  KeyList *hk;
  int i;
  if (mi->contagious_boitakon == 0 || mi->bidouille_qui_pue) return;
  mi->bidouille_qui_pue = 1;
  if (lvl > 50) return;
  for (hk = *pin; hk; hk=hk->next) {
    if (board_key_list_test_mi_hk(boards, mi, hk)) {
      *pout = key_list_add(*pout, hk->key, hk->type, hk->num, 0);
      *pin = key_list_remove(*pin, hk->key, hk->type);
    }
  }
  if (*pin == NULL) return;
  for (i=0; i < mi->nb_refs; i++) {
    int j;
    board_msg_info *mi2;
    mi2 = mi->refs[i].mi;
    for (j=0; mi2 && j < mi->refs[i].nbmi; j++) {
      if (mi2->contagious_boitakon && mi2->bidouille_qui_pue == 0)
	board_key_list_get_recurs_plop_list(boards, mi2, pin, pout, lvl+1);
    }
  }
}


/*
  renvoie la liste des element de la keylist qui sont 'positifs' pour le 
  message mi
  le truc un peu lourdeau c'est pour la boitakon recursive (m� sa economise de la m�moire)
*/
KeyList *
board_key_list_get_mi_positive_list(Boards *boards, board_msg_info *mi, KeyList *klist, int is_plop_list)
{
  KeyList *pl = NULL;
  KeyList *hk;
  /* liste de plopification normale */
  for (hk = klist; hk; hk = hk->next) {
    if (board_key_list_test_mi_hk(boards,mi,hk)) {
      pl = key_list_add(pl, hk->key, hk->type, hk->num, 0);
    }
  }
  /* l� c'est plus boulet */
  if (is_plop_list && mi->contagious_boitakon) {
    board_msg_info *mi2;
    KeyList *hbak = NULL;

    for (mi2 = boards->first; mi2; mi2 = mi2->g_next) mi2->bidouille_qui_pue = 0;

    for (hk = klist; hk; hk = hk->next) {
      if (hk->num == 3 && key_list_find(pl, hk->key, hk->type)==NULL) { /* hungry bak */
	hbak = key_list_add(hbak, hk->key, hk->type, hk->num, 0);
      }
    }
    board_key_list_get_recurs_plop_list(boards, mi, &hbak, &pl, 0);
  }
  return pl;
}

int is_utf8_superscript(const unsigned char *p, int *superscript) {
  if (p[0] == 0xC2) {
    if (p[1] == 0xB2) { 
      if (superscript) *superscript = 2; 
      return 2;
    }
    if (p[1] == 0xB3) { 
      if (superscript) *superscript = 3; 
      return 2;
    }
    if (p[1] == 0xB9) { 
      if (superscript) *superscript = 1; 
      return 2;
    }
  }
  return 0;
}


/* si 'ww' contient une reference (du type '1630', '125421', '12:45:30') vers un message existant, on renvoie 
   son msg_info, et on rempli 'commentaire' 


   avertissement au lecteur t�m�raire:
   cette fonction est un sac de noeuds et �a ne va pas en s'arrangeant
   
   (en gros c'est toute une serie de tests pour filtrer les refs invalides tout en
   etant le plus g�n�ral possible ..)

   site_name_hash contient 0 quand il n'est pas fait allusion � un site particulier (du genre 11:11:11@linuxfr)
*/
static int
check_for_horloge_ref_basic_helper(const unsigned char *ww, const char **site_name, int *ref_h, int *ref_m, int *ref_s, int *ref_num, int *ref_year, int *ref_month, int *ref_day)
{
  int l, h, m, s, num;  /* num est utilise pour les posts multiples (qui on un m�me timestamp) */
  int year = -1, month = -1, day = -1;
  unsigned char w[11];

  *ref_h = -1; *ref_m = -1; *ref_s = -1; *ref_num = -1;
  num = -1; h = -1; m = -1; s = -1;
  l = strlen(ww);

  *ref_year = -1; *ref_month = -1; *ref_day = -1;

  if (l < 4) return 0; /* de qui se moque-t-on ? */

  /* on tente d'abord d'intercepter le nom du site */
  *site_name = NULL;
  if (l > 6) {
    int i;    
    for (i = 5; i < 10 && ww[i]; i++) {
      if (ww[i] == '@') {
	l = i; /* ben oui faut pas affoler tout le monde */

	if (isalnum(ww[i+1]) || ww[i+1] == '_')
	  *site_name = ww+i+1;
	
/* 	printf("--> site_name = '%s'\n",*site_name);  */
	break;
      }
    }
  }

  if (l > 4 + 11 && l <= 10 + 11) {
      // certainement une horloge avec une date

      if (ww[4] != '-' || ww[7] != '-') {
          // en fait ce n'est pas une date
          return 0;
      }

      if (ww[10] == 'T') {
          // date � la con de devnewton<
          year = atoi(ww);
          month = atoi(ww + 5);
          day = atoi(ww + 8);

          l -= 11;
          strncpy(w, ww + 11, l);
          w[l] = 0;
      } else if (ww[10] == ' ') {
          year = atoi(ww);
          month = atoi(ww + 5);
          day = atoi(ww + 8);

          l -= 11;
          strncpy(w, ww + 11, l);
          w[l] = 0;
      } else {
          // �a devait etre autre chose, finalement
          return 0;
      }
  } else if (l < 5 || l > 10) {
      return 0; /* on elimine les cas les plus explicites */
  } else {
      strncpy(w, ww, l);
      w[l] = 0;
  }

  if (w[2] == ':') {
    h = atoi(w);
    m = atoi(w + 3);

    if (l > 6) {
        s = atoi(w + 6);
    } else {
        s = -1;
    }

    if (l > 8) {
        if (w[8] == ':') {
            num = atoi(w + 9) - 1;
        } else if (is_utf8_superscript(w + 8, &num)) {
            num -= 1;
        }
    }

  } else return 0;

  if (h > 23 || m > 59 || s > 59) return 0;

  *ref_h = h; *ref_m = m; *ref_s = s; *ref_num = num;
  *ref_year = year; *ref_month = month; *ref_day = day;

  return 1;
}

/* 
   comme au-dessus, mais fait l'effort de recherche le numero du site 
   attention aux valeurs de retour de site_id 
*/
static int
check_for_horloge_ref_basic(Boards *boards, const unsigned char *ww, int *site_id, int *ref_h, int *ref_m, int *ref_s, int *ref_num, int *ref_year, int *ref_month, int *ref_day)
{
  const char *site_name;
  int ret;
  
  *site_id = -1;
  ret = check_for_horloge_ref_basic_helper(ww, &site_name, ref_h, ref_m, ref_s, ref_num, ref_year, ref_month, ref_day);
  if (ret && site_name) { /* bon .. �a m�rite qu'on cherche le site */
    int i, hash;

    for (i=0; isalpha(site_name[i]); i++) ;

    hash = str_hache_nocase(site_name, i);
    for (i = 0; i < boards->nb_aliases; i++) {
      if (boards->aliases[i].hash == hash) {
	*site_id = boards->aliases[i].sid;
	break;
      }
    }
    if (*site_id == -1) *site_id = -2; /* pour savoir qu'un @site a �t� utilis�, mais qu'il est mauvais */
  }
  return ret;
}

/* ceci est un commentaire � la con pour forcer le commit (oui je suis un tocard mais g la flemme de chercher � comprendre */


/* dans la famille des fonction pourries, je demande ... */
char *
board_get_tok(const unsigned char **p, const unsigned char **np, 
	unsigned char *tok, int max_toklen, int *has_initial_space)
     /* ... ou��� celle-l� est vraiment bien */
{
  const unsigned char *start, *end;

  assert(p); 
  assert(*p); 
  assert(tok);

  /* has_initial_space doit etre initialise dans la procedure appelante (sinon y'a des pbs avec les tag html)
   *has_initial_space = 0; */ /* indique si le token commence par un (ou plusieurs) espace */

  start = *p; *np = NULL;
  /* saute les espaces */
  while (*start <= ' ' && *start != '\t' && *start) { start++; *has_initial_space = 1; }
  end = start;

  /* if (*start == '\t') printf("allez ! '%.20s'\n", start); */

  /* les bon vieux tags html (update les '<' et '>' des tags sont prefix�s par une tabulation) */
  if (*start == '\t' && *(start+1) == '<') {
    static const char *balise = "abusiABUSI";
    int i;
    /* c'est un peu chiant, on risque de mal reconnaitre les balise ou
       d'en reconnaitre qui ont ete ajoutees a la main 
       on teste d'abord les balises courantes
    */
    for (i=0; i < 10; i++) { 
      if (start[2] == balise[i] && start[3] == '\t' && start[4] == '>') {
	end = start+5;
	break;
      } else if (start[2] == '/' && start[3] == balise[i] && start[4] == '\t' && start[5] == '>') {
	end = start+6;
	break;
      }
    }
    if (end == start) {
      int is_href;
      const unsigned char *s0 = "\t<a href=\"";
      const unsigned char *s1 = "\t<a href=\"../";
      const unsigned char *s2 = "\t<a href=\"./";
      /* puis les <a href> (c'est un peu particulier */

      /* c'est un peu facho, d'autant que c'est reverifi� au niveau de open_url, mais
	 bon, apres la douche froide... */
      is_href = (strncasecmp(start, s0, strlen(s0)) == 0 && is_url(start+10) != -1) ||
        (strncasecmp(start, s1, strlen(s1)) == 0) ||
        (strncasecmp(start, s2, strlen(s2)) == 0);
      if (is_href) {
	/* printf("get_tok: '"); */
	end = start+1;
	while (*end && *end != '\t') end++; /* { printf("%c", *end); end++; } */
	if (*end == '\t' && *(end+1)=='>') end+=2;
	/* printf("\n"); */
/* 	if (*end) end++; */
      } else if (strncasecmp(start+2, "tt\t>", 4) == 0) {
	end = start+6;
      } else if (strncasecmp(start+2, "/tt\t>", 5) == 0) {
	end = start+7;
      } else if (strncasecmp(start+2, "span style=\"text-decoration: underline\"\t>", 41)==0) {
	end = start+43;
      } else if (strncasecmp(start+2, "span style=\"text-decoration: line-through\"\t>", 44)==0) {
	end = start+46;
      } else if (strncasecmp(start+2, "/span\t>", 7) == 0) {
	end = start+9;
      } else {
	const char *p;

	/* sinon on ignore */
	start++;
	end=start+1;
	p = start;
        if (Prefs.verbosity >= Prefs.verbosity_underpants) {
          printf(_("get_tok not recognized: (len=%d)'"), (int)strlen(p));
          while (*p) { printf("%c", *p); p++;}
          printf("\n");
        }
      }
    }
    /*
    {
      char c;
      c = *end; *end = 0;
      myprintf("tok='%<YEL %s>'\n", start);
      *end = c;
    }
    */
  } else if (*start == '\t') { /* �a pue .. le backend ou le coincoin est sans slip */
    /* arf �a c'est de la traduction ! vive les slips et jjb */
    if (Prefs.verbosity >= Prefs.verbosity_underpants) {
      printf(_("Hmmm, looks like there's an underpants problem here: %s\n"), start);
    }
    start++;
    if (*start) end = start+1;
  } else if (*start == '#') { /* une r�f�rence � un post id */
      do {
          end++;
      } while (*end && isdigit(*end) && end-start <= 16);

      if (end-start == 3 && *(end) == ':') {
          // en fait, c'est probablement juste une vieille horlage avec date de wmcc/olcc
          end = start;

          // euh...
          goto timestamp;
      }

      if (*end == '@') {
          // alors l� y a un @tribune indiqu�
          do {
              end++;
          } while (isalnum(*end) || *end == '_');
      }
  } else {
timestamp:
    /* pour aider la reconnaissance des timestamp */
    if (*end >= '0' && *end <= '9') {
      unsigned char last = *end;
      int is_multi = 0;
      while (*end && (
                (*end >= '0' && *end <= '9')
             || (end-start == 10 && (*end == ' ' || *end == 'T'))
             || strchr(":-", *end)
             || is_utf8_superscript(end, NULL)
           )) {
        int len = is_utf8_superscript(end,NULL); if (!len) len = 1;
	end+=len;
	if ((last < '0' || last > '9') && (*end < '0' || *end > '9'))
	  break; /* deux caract�res non num�riques cons�cutifs, c'est la fin de l'horloge.. */
	last = *end;
      }
      if (*end == '@') {
	is_multi = 1;
	do { 
	  end++;
	} while (isalnum(*end) || *end == '_');
/* 		{ char c = *end; *end = 0; printf("TOK : %s\n", start); *end = c; } */
      }
      /* un petit coup de marche arriere si on n'a pas termine sur un chiffre */
      if (end-start > 4 && is_multi == 0 && *(end-1) == ':') end--;
    } else {
      int ok_totoz = 0;
      if (end[0] == '[' && end[1] == ':') { /* totoz ? */
        if (Prefs.board_enable_hfr_pictures) {
          const unsigned char *s = end+2;
          int i;
          for (i=0; i < 100 && s[i]; ++i) {
            if (s[i] == ']' && i >= 2) { end = s+i+1; ok_totoz = 1; break; }
            if (!isalnum(s[i]) && s[i] != '_' && s[i] != ' ' && s[i] != '-' && s[i] != ':') break;
          }
        }
        if (!ok_totoz) end++; /* pour relancer la machine */
      }
      if (!ok_totoz) {
        /* un mot normal */
        while (*end && *end != '\t' && *end > ' ' && (*end < '0' || *end > '9') && (!(end[0] == '[' && end[1] == ':'))) end++;
      }
      if (ok_totoz) {
        char ss[512]; strncpy(ss, start, MIN(end-start, max_toklen-1));ss[MIN(end-start, max_toklen-1)] = 0;
      }
    }
  }
  if (end == start) return NULL;
  else strncpy(tok, start, MIN(end-start, max_toklen-1));
  tok[MIN(end-start, max_toklen-1)] = 0;
  *p = start;
  *np = end;
  return tok;
}


/* oh le joli nom en anglais 

  cette fonction n'est pas utilis�e ici mais dans board.c

  c'est le bordel , �a �voluera surement

  TODO: pb � l'initialisation, il faut l'appeler dans l'ordre des ID, sinon y'a pb

  TODO: CETTE FONCTION EST NAZE MAIS JE SUIS TROP CREVE JE FAIS RIEN QUE DES CONNERIES CE SOIR

  update: tiens, c'est quoi ce todo ? bon, je viens de refaire completement cette fonction
  en utilisant les refs stock�es dans les messages, c nettement moins tordu.

  re-update: apr�s un coup d'oeil rapide, je crois que le todo a d�j� �t� r�solu
*/
int
board_msg_is_ref_to_me(Boards *boards, const board_msg_info *ref_mi) {
  board_msg_info *mi;

  mi = boards->first;

  //  printf("board_msg_is_ref_to_me: test de %02d:%02d:%2d(%d)..[nb_refs=%d]\n", ref_mi->hmsf[0], ref_mi->hmsf[1], ref_mi->hmsf[2], ref_mi->hmsf[3], ref_mi->nb_refs);
  while (mi && mi != ref_mi) {
    //    const unsigned char *p, *np;

    if (mi->is_my_message) {
      int i;
      for (i = 0; i < ref_mi->nb_refs; i++) {
	int j;
	board_msg_info *mi2;
	mi2 = ref_mi->refs[i].mi;
	for (j=0; mi2 && j < ref_mi->refs[i].nbmi; j++) {
	  if (mi2 == mi) return 1;
	  mi2 = mi2->next; /* et ___PAS___ g_next puisque les multi-refs ne peuvent etre cons�cutives
			      que sur une m�me tribune, �a peut pas �tre cross-tribune
			      (fo pas pousser grand mere) */
	}
      }
    }
    mi = mi->g_next;
  }
  return 0;
}


/*
  recherche un message par hh:mm:ss^num 
  /!\ la recherche est local � la board indiqu�e en parametre
*/
static board_msg_info *
board_find_horloge_ref(Board *board, id_type caller_id, 
		       int h, int m, int s, int num, int year, int month, int day, unsigned char *commentaire, int comment_sz)
{
  board_msg_info *mi, *best_mi;
  int best_mi_num;
  int previous_mi_was_a_match = 0, match;

  mi = board->msg;
    
  best_mi = NULL;
  best_mi_num = 0;


  while (mi) {
    match = 0;
    if (mi->id.lid > id_type_lid(caller_id) && best_mi ) {
        break; /* on ne tente ipot que dans les cas desesperes ! */
    }
    if (s == -1) {
      if (mi->hmsf[0] == h && mi->hmsf[1] == m) {
	match = 1;

        if (year >= 0 && month >= 0 && day >= 0) {
            struct tm *t = localtime(&mi->timestamp);

            if (t->tm_year == year-1900 && t->tm_mon == month-1 && t->tm_mday == day) {
                match = 1;
            } else {
                match = 0;
            }
        }
      }
    } else {
      if (mi->hmsf[0] == h && mi->hmsf[1] == m && mi->hmsf[2] == s) {
	if (num == -1 || num == best_mi_num) {
	  match = 1; //break;
	}
        if (year >= 0 && month >= 0 && day >= 0) {
            struct tm *t = localtime(&mi->timestamp);

            if (t->tm_year == year-1900 && t->tm_mon == month-1 && t->tm_mday == day) {
                match = 1;
		best_mi_num++;
            } else {
                match = 0;
            }
        } else {
	    best_mi_num++;
	}
      } else {
	best_mi_num = 0; /* la raison est tordue: si on conserve des messages
			    sur plusieurs jours, le comportement naturel est de
			    renvoyer la ref la + r�cente. C'est pour �a que le
			    break juste au-dessus est maintenant
			    comment�. Comme �a la boucle est �xecut�e jusqu'�
			    ce qu'on atteigne caller_id, m�me quand on a trouv�
			    un message qui matche parfaitement, juste pour �tre
			    s�r qu'il n'y en a pas un plus r�cent qui fasse
			    l'affaire. Mais il faut aussi penser � r�initialiser
			    best_mi_num, cqfd ;-) */			    
      }
    }
    
    if (match && !previous_mi_was_a_match) best_mi = mi;
    previous_mi_was_a_match = match;
    mi = mi->next;
  }
  
  if (commentaire) {
    char s_ts[12];
    board_msg_info *caller_mi;

    commentaire[0] = 0;
    caller_mi = boards_find_id(board->boards, caller_id);
    if (caller_mi) {
      commentaire[0] = 0;
      if (s == -1) {    
	snprintf(s_ts, 10, "%02d:%02d", h,m);
      } else {
	char snum[3];
	snum[0] = snum[1] = snum[2] = 0;
	switch (num) {
	case -1: break;
	case 0: snum[0] = '�'; break;
	case 1: snum[0] = '�'; break;
	case 2: snum[0] = '�'; break;
	default: snum[0] = ':'; snum[1] = '1' + num;
	}
	snprintf(s_ts, 12, "%02d:%02d:%02d%s", h,m,s,snum);
      }
      if (best_mi == NULL) {
	if (caller_mi->hmsf[0]*60+caller_mi->hmsf[1] < h*60+m) {
	  snprintf(commentaire, comment_sz, _("IPOT(tm) detected"));
	} else {
	  snprintf(commentaire, comment_sz, _("but where is '%s' ?"), s_ts);
	}
      } else if (best_mi->id.lid > caller_mi->id.lid && 
		 best_mi->id.sid == caller_mi->id.sid) {
	snprintf(commentaire, comment_sz, _("[IPOT(tm)]"));
      } else if (id_type_eq(best_mi->id,caller_mi->id)) {
	snprintf(commentaire, comment_sz, _("Awww, we turn around and around and around and around..."));
      } else if (best_mi->in_boitakon) {
	KeyList *hk;
	char *nom;

	nom = (best_mi->login && best_mi->login[0]) ? best_mi->login : best_mi->useragent;
	hk = board_key_list_test_mi(board->boards, best_mi, Prefs.plopify_key_list);
	if (hk) {
	  snprintf(commentaire, comment_sz, _("Hello from %.30s in the boitakon ! "
		   "(because %s=%.20s)"), nom,
		   key_list_type_name(hk->type), hk->key);
	} else if (best_mi->contagious_boitakon) {
	  snprintf(commentaire, comment_sz, _("Hello from %.30s who was eaten by the boitakon"), nom);
	} else {
	  snprintf(commentaire, comment_sz, _("Hello from %.30s in the boitakon, "
		   "BUT YOU HAVE JUST FOUND A BUG IN THE BOITAKON :-("), nom);
	}
      }
    }
  }
  return best_mi;
}



/* look for a reference on every board */
board_msg_info *
check_for_horloge_ref(Boards *boards, id_type caller_id, 
		      const unsigned char *ww, unsigned char *commentaire, 
		      int comment_sz, int *is_a_ref, int *ref_num)
{
  Board *board;
  int site_id, h, m, s, num; /* num est utilise pour les posts multiples (qui on un m�me timestamp) */
  int year, month, day;
  
  assert(!id_type_is_invalid(caller_id));
  *is_a_ref = 0;
  if (check_for_horloge_ref_basic(boards, ww, &site_id, &h, &m, &s, &num, &year, &month, &day) == 0) return NULL;
  *is_a_ref = 1;

  if (ref_num) *ref_num = num;

  if (site_id >= 0) {
    board = boards->btab[site_id];
  } else if (site_id == -1) { /* on prend le site par d�faut */
    board = boards->btab[caller_id.sid];
  } else {
    snprintf(commentaire, comment_sz, _("I don't fucking know %s"), ww);
    return NULL;
  }
  if (board) {
    return board_find_horloge_ref(board, caller_id, 
				  h, m, s, num, year, month, day, commentaire, comment_sz);
  } else return NULL;
}

/*
  recherche un message par id
  /!\ la recherche est locale � la board indiqu�e en parametre
*/
static board_msg_info *
board_find_horloge_id(Board *board, id_type caller_id,
		       int64_t post_id, unsigned char *commentaire, int comment_sz)
{
  board_msg_info *mi;
  mi = board->msg;

  board_msg_info *found_mi = NULL;

  while (mi) {
    if (mi->id.lid == post_id) {
        found_mi = mi;
        break;
    }

    mi = mi->next;
  }

  if (commentaire) {
      if (!found_mi) {
          if (post_id > caller_id.lid) {
              snprintf(commentaire, comment_sz, _("IPOT(tm) detected"));
          } else {
              snprintf(commentaire, comment_sz, _("but where is '#%" PRId64 "' ?"), post_id);
          }
      }
  }

  return found_mi;
}

/* si 'ww' contient une reference (du type '#1630') vers un message existant, on renvoie 
   son msg_info, et on rempli 'commentaire' 

   site_name_hash contient 0 quand il n'est pas fait allusion � un site particulier (du genre #1630@linuxfr)
*/
static int
check_for_id_ref_basic_helper(const unsigned char *ww, const char **site_name, int64_t *ref_id)
{
  int l;
  int64_t id;

  l = strlen(ww);

  if (l < 2) return 0; /* de qui se moque-t-on ? */

  if (ww[0] != '#') {
      return 0;
  }

  /* on tente d'abord d'intercepter le nom du site */
  *site_name = strchr(ww, '@');
  if (*site_name) {
    *site_name += 1;
  }

  id = strtoull(ww + 1, NULL, 10);

  if (!id) {
      return 0;
  }

  *ref_id = id;

  return 1;
}

static int
check_for_id_ref_basic(Boards *boards, const unsigned char *ww, int *site_id, int64_t *ref_id)
{
  const char *site_name;
  int ret;

  *site_id = -1;
  ret = check_for_id_ref_basic_helper(ww, &site_name, ref_id);
  if (ret && site_name) { /* bon .. �a m�rite qu'on cherche le site */
    int i, hash;

    for (i=0; isalpha(site_name[i]); i++) ;

    hash = str_hache_nocase(site_name, i);
    for (i = 0; i < boards->nb_aliases; i++) {
      if (boards->aliases[i].hash == hash) {
	*site_id = boards->aliases[i].sid;
	break;
      }
    }
    if (*site_id == -1) *site_id = -2; /* pour savoir qu'un @site a �t� utilis�, mais qu'il est mauvais */
  }
  return ret;
}

/* look for a reference on every board */
board_msg_info *
check_for_id_ref(Boards *boards, id_type caller_id, 
		      const unsigned char *ww, unsigned char *commentaire, 
		      int comment_sz, int *is_a_ref)
{
  Board *board;
  int site_id;
  int64_t post_id;
  
  assert(!id_type_is_invalid(caller_id));
  *is_a_ref = 0;
  if (check_for_id_ref_basic(boards, ww, &site_id, &post_id) == 0) return NULL;
  *is_a_ref = 1;

  if (site_id >= 0) {
    board = boards->btab[site_id];
  } else if (site_id == -1) { /* on prend le site par d�faut */
    board = boards->btab[caller_id.sid];
  } else {
    snprintf(commentaire, comment_sz, _("I don't fucking know %s"), ww);
    return NULL;
  }
  if (board) {
    return board_find_horloge_id(board, caller_id, post_id, commentaire, comment_sz);
  } else return NULL;
}


/* appel� discretement par board_check_my_messages dans board.c
   (oui c pas logique) 

*/
void
board_msg_find_refs(Board *board, board_msg_info *mi)
{
  //  Boards *boards = board->boards;
  const unsigned char *p, *np;
  int max_nb_refs;

  p = mi->msg;
  max_nb_refs = mi->nb_refs = 0;
  mi->refs = NULL;

  while (p) {
     int has_initial_space = 0; /* inutilis� */
     unsigned char tok[512];
     
     if (board_get_tok(&p,&np,tok,512, &has_initial_space) == NULL) { break; }
     if (tok[0] >= '0' && tok[0] <= '9') {
       int sid, h,m,s,num;
       int year, month, day;
       if (check_for_horloge_ref_basic(board->boards, tok, &sid, &h, &m, &s, &num, &year, &month, &day)) {
	 board_msg_info *ref_mi;

	 Board *ref_board = board;
	 if (sid >= 0) ref_board = board->boards->btab[sid];
	 if (ref_board) { /* c'est peut etre une tribune desactiv� */
	   
	   if (mi->nb_refs+1 > max_nb_refs) {
	     max_nb_refs += 10;
	     mi->refs = realloc(mi->refs, max_nb_refs*sizeof(board_msg_ref));
	   }
	   
	   mi->refs[mi->nb_refs].h = h;
	   mi->refs[mi->nb_refs].m = m;
	   mi->refs[mi->nb_refs].s = s;
	   mi->refs[mi->nb_refs].num = num;
	   mi->refs[mi->nb_refs].nbmi = 0;
	   mi->refs[mi->nb_refs].mi = NULL;
	   
	   ref_mi = board_find_horloge_ref(ref_board, mi->id, 
					   h, m, s, num, year, month, day, NULL, 0);
	   
	   if (ref_mi && ((ref_mi->id.lid <= mi->id.lid) || ref_board != board)) {
	     mi->refs[mi->nb_refs].mi = ref_mi;
	     mi->refs[mi->nb_refs].nbmi=1;
	     if (num == -1) {
	       /* gestion des post multiples */
	       board_msg_info *ref_mi2;
	       ref_mi2 = ref_mi->next;
	       while (ref_mi2 && ref_mi2->timestamp == ref_mi->timestamp) {
		 mi->refs[mi->nb_refs].nbmi++;
		 ref_mi2 = ref_mi2->next;
	       }
	     }
	   }
	   mi->nb_refs++;
	 }
       }
     } else if (tok[0] == '#') {
       int sid;
	   int64_t post_id;
       if (check_for_id_ref_basic(board->boards, tok, &sid, &post_id)) {
	 board_msg_info *ref_mi;

	 Board *ref_board = board;
	 if (sid >= 0) ref_board = board->boards->btab[sid];
	 if (ref_board) { /* c'est peut etre une tribune desactiv� */
	   
	   ref_mi = board_find_horloge_id(ref_board, mi->id, post_id, NULL, 0);
	   
	   if (ref_mi && ((ref_mi->id.lid <= mi->id.lid) || ref_board != board)) {
	   
	     if (mi->nb_refs+1 > max_nb_refs) {
	       max_nb_refs += 10;
	       mi->refs = realloc(mi->refs, max_nb_refs*sizeof(board_msg_ref));
	     }
	     
	     mi->refs[mi->nb_refs].h = 0;
	     mi->refs[mi->nb_refs].m = 0;
	     mi->refs[mi->nb_refs].s = 0;
	     mi->refs[mi->nb_refs].num = 0;
	     mi->refs[mi->nb_refs].mi = ref_mi;
	     mi->refs[mi->nb_refs].nbmi=1;
	     mi->nb_refs++;
	   }
	 }
       }
     }
     p=np;
  }
  if (mi->refs) mi->refs = realloc(mi->refs, mi->nb_refs*sizeof(board_msg_ref));

  if (Prefs.verbosity > 3) { 
    int i;
    myprintf("msg[%<YEL %04d>]: REFS=", mi->id);
    for (i=0; i < mi->nb_refs; i++) {
      myprintf("{%<CYA %02d>:%<CYA %02d>:%<CYA %02d>(%<cya %d>), %<YEL %04d>/%<GRN %d>} ", 
	mi->refs[i].h, mi->refs[i].m, mi->refs[i].s, mi->refs[i].num, mi->refs[i].nbmi ? mi->refs[i].mi->id.lid : -1, mi->refs[i].nbmi);
    }
    myprintf("\n");
  }
}



