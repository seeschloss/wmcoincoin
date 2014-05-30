/*
  rcsid=$Id: troll_detector.c,v 1.19 2004/02/29 15:01:19 pouaite Exp $
  ChangeLog:
  $Log: troll_detector.c,v $
  Revision 1.19  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.18  2003/06/29 23:58:39  pouaite
  suppression de l'overrideredirect du palmi et ajout de pinnipede_totoz.c et wmcoincoin-totoz-get etc

  Revision 1.17  2003/05/07 18:44:57  pouaite
  bugfix divers+solaris7

  Revision 1.16  2003/02/28 19:08:43  pouaite
  trucs divers

  Revision 1.15  2002/08/21 20:22:16  pouaite
  fix compil

  Revision 1.14  2002/08/17 18:33:40  pouaite
  grosse commition

  Revision 1.13  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.12  2002/04/13 11:55:19  pouaite
  fix kde3 + deux trois conneries

  Revision 1.11  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

  Revision 1.10  2002/03/18 22:46:49  pouaite
  1 ou 2 bugfix mineurs, et surtout suppression de la dependance avec la libXpm .. un premier pas vers wmc² en 8bits

  Revision 1.9  2002/03/05 21:04:28  pouaite
  bugfixes suite à l'upgrade de dlfp [et retour au comportement à l'ancienne du clic sur les horloges pour les moules ronchonnes]

  Revision 1.8  2002/03/03 10:10:04  pouaite
  bugfixes divers et variés

  Revision 1.7  2002/02/27 00:32:19  pouaite
  modifs velues

  Revision 1.6  2002/02/24 22:13:57  pouaite
  modifs pour la v2.3.5 (selection, scrollcoin, plopification, bugfixes)

  Revision 1.5  2002/02/02 23:49:17  pouaite
  plop

  Revision 1.4  2002/01/30 21:03:51  pouaite
  correction du bug du au signe des char, et d'un petit bug dans les reference d'horloges

  Revision 1.3  2002/01/14 23:54:06  pouaite
  reconnaissance des posts effectué par l'utilisateur du canard (à suivre...)

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

*/

#include <libintl.h>
#define _(String) gettext (String)

#include "config.h"
#include "coincoin.h"
#include "coin_util.h"
#include "myprintf.h"

/*

                                              "Abandon All Hope, Ye Who Enter Here"

*/


#define MI_MAX_LEN 1024 /* lg max du message prise en compte */
#define MI_MAX_WORD 200 /* nb max de mots pris en compte */
#define MAX_WLEN 32     /* lg max d'un mot trollesque */
#define MAX_TD_MATCH 20 /* nb max d'occurences d'un meme mot dans troll_data */
struct troll_data {
  unsigned int hash;
  unsigned char debut_troll; /* = 1 si le mot peut être à la source d'un troll */
  unsigned char need_lnk1; /* = 1 si le mot n'est valable que si on satisfait la categorie lnkcat1 */
  unsigned char categ; /* > à 0 */
  unsigned char ltrunc;
  unsigned char bonus;
  unsigned char lnkcat1; /* > à 0 si catagorie liée, 0 sinon */
  unsigned char bonuscat1;
  unsigned char lnkcat2; /* > à 0 si catagorie liée, 0 sinon */
  unsigned char bonuscat2;
};

#include "troll_data.h"

typedef struct _Word {
  unsigned char *w; /* pointeur non mallocé */
  int len;
  
  int nb_td_idx; /* 0 = mot non trollesque */
  int *td_idx;   /* liste de mots trollesques correspondant (a priori un même mot 
		    trollesque peut apparaitre plusieurs fois dans troll_data, mais avec 
		    des contextes differents (cad des categ, lnkcat12 et bonuscat12 differents)

		    exemple: gpl + xemacs = troll
		             gpl + windows = troll
			     mais windows + xemacs = pas forcement troll

			     bon cet exeple est pas genial mais l'idee est la
		 */

  int in_tag; /* dans un tag ? */
  int num;
  struct _Word *prev, *next;
} Word;

int cnt_anti_blocage; /* anti recursions qui se comportent en O(n!) (genre avec 30 coins..)*/
#define MAX_CNT_ANTI_BLOCAGE 300000


static Word*
wordlist_merge(Word *w1, Word *w2)
{
  Word *w;

  if (w2 == NULL) return w1;
  if (w1 == NULL) return w2;
  
  w = w1;
  while (w->next) { w = w->next; }
  w->next = w2;
  w2->prev = w;
  
  return w1;
}

/* enleve les mots dont le nb_td_idx == 0 */
static Word *
wordlist_cleanup(Word *wlst)
{
  Word *w, *next=NULL, *prev=NULL, *first=NULL;
  
  w = wlst;
  if (w) { prev = w->prev; }
  while (w) {
    next = w->next;
    if (w->nb_td_idx == 0) {
      if (w->td_idx) free(w->td_idx);
      free(w);
    } else {
      if (prev) prev->next = w;
      w->prev = prev;
      w->next = NULL;
      if (first == NULL) first = w;
      prev = w;
    }
    w = next;
  }
  return first;
}

static Word *
decoupe_bloc(unsigned char *start, unsigned char *end, int in_tag)
{
  int n;
  unsigned char *s, *tagstart, *tagend;
  Word *wl;
  
  assert(start); assert(end);

  n = end - start;
  if (start == end) return NULL;
  wl = NULL;

  tagstart = NULL; tagend = NULL;
  
  /* recherche un tag (ouverture + fermeture) complet dans le bloc
     si on n'en trouve qu'un partie, on considère que c'est un mot de na><0r */
  for (s = start; s < end; s++) {
    if (*s == '\t' && s < end-1) {
      if (s[1] == '<' && tagstart == NULL) { tagstart = s; }
      if (s[1] == '>' && tagstart != NULL) { tagend = s; break; }
    }
  }
  
  if (tagstart && tagend) {
    wl = decoupe_bloc(start, tagstart, 0);
    wl = wordlist_merge(wl, decoupe_bloc(tagstart+2, tagend, 1));
    wl = wordlist_merge(wl, decoupe_bloc(tagend+2, end, 0));
  } else {
    /* si le mot se termine par des chiffres, on sépare la partie chiffre de la partie mot */
    s = end-1;
    while (s > start) {
      if (*s >= '0' && *s <= '9') {
	s--; assert(*s);
      } else { s++; break;}
    }
    if (s != end && s != start) {
      /* on vient de couper en deux ? */
      wl = decoupe_bloc(start, s, in_tag);
      wl = wordlist_merge(wl,decoupe_bloc(s, end, in_tag));
    } else {
      ALLOC_OBJ(wl, Word);
      wl->next = NULL;
      wl->prev = NULL;
      wl->w = start;
      wl->len = end-start;
      wl->in_tag = in_tag;
    }
  }
  return wl;
}

static int
tdata_lookup(unsigned hash_codes[MAX_WLEN], int preflen, int **match)
{
  int i;
  int idx[MAX_TD_MATCH];
  int nb_match;

  assert(match);
  assert(*match == NULL);

  nb_match = 0;

  /* recherche tous les mots qui collent et les stocke dans la table 'idx' */
  for (i = 0; i < NB_TROLL_WORDS; i++) {
    if (troll_data[i].hash == hash_codes[preflen]) {
      idx[nb_match++] = i;
      if (nb_match == MAX_TD_MATCH) break;
    } else if (troll_data[i].ltrunc > 0 && troll_data[i].ltrunc <= preflen) {
      assert(troll_data[i].ltrunc < MAX_WLEN && troll_data[i].ltrunc >= 2);
      if (troll_data[i].hash == hash_codes[troll_data[i].ltrunc]) {
	idx[nb_match++] = i;
	if (nb_match == MAX_TD_MATCH) break;
      }
    }
  }
  
  /* alloue la liste de retour */
  if (nb_match) {
    ALLOC_VEC((*match), nb_match, int);
    for (i = 0; i < nb_match; i++) {
      (*match)[i] = idx[i];
    }
  } else {
    *match = NULL;
  }

  return nb_match;
}


/* 
   on se donne une liste de mots trollesques, et parmis ceux-ci, on choisi la
   combinaison donnant le meilleur score

   c'est recursif, pas optimal, donc ça peut bouffer beaucoup de temps, d'où le
   cnt_anti_blocage et la limitation à 5 niveaux de récursion

   nb_mots = nombre de mots disponibles (taille de sel_init, best_sel, sel)

   filter_categ = 0 si on commence un nouveau troll (level == 1)
                  numéro de la catgogie sondée si un troll est en cours (level > 1)

   level = niveau de récursion (le plus bas : level = 1)

   best_sel = en entrée: liste des mots déjà selectionnés 
              en sortie: même chose + les nouveaux mots qui complètent le troll

   trouve -> en sortie , indique si on a pu satisfaire les conditions du troll	      

 */
static int
eval_best_troll(Word *wlst, int nb_mots, int filter_categ, int level, int bonus_if_found, char *best_sel, int *trouve)
{
  Word *w;
  int score;
  int best_score = -10000;
  char sel_init[nb_mots];
  int i;

  *trouve = 0;

  assert(wlst);

  if (level > 6) return 0; /* faut pas exagerer */

  /* liste des mots déjà selectionnés à des level inférieurs */
  for (i=0; i < nb_mots; i++) {
    sel_init[i] = best_sel[i];
  }

  /* on parcours tous les mots */
  w = wlst;

  while (w) {
    if (sel_init[w->num] == 0) { 
      int idx;
      /* assez souvent, nb_td_idx = 1.. */
      for (idx=0; idx < w->nb_td_idx; idx++) {
	struct troll_data *td;

	assert(w->td_idx[idx] >= 0);
	td = &troll_data[w->td_idx[idx]];

	/* 
	   le mot est admissible pour un supplement d'enquete si: 
	   - on est au premier niveau, toutes les catégories sont bonnes mais il faut
	   que le mot soit habilité à débuter un troll
	   - on est en cours de troll (level > 1), il faut que le mot soit dans 
	   la categorie recherchee
	*/
	if ((level == 1 && td->debut_troll) ||
	    (level > 1 && td->categ == filter_categ)) {
	  char sel[nb_mots];
	  
	  ALLOW_X_LOOP;
	  
	  cnt_anti_blocage++;
	  if (cnt_anti_blocage > MAX_CNT_ANTI_BLOCAGE) goto stooooop;
	  
	  /* on reprend la liste de mots bloqués initial */
	  for (i=0; i < nb_mots; i++) {
	    sel[i] = sel_init[i];
	  }
	  
	  /* score initial */
	  score = td->bonus + bonus_if_found;
	  BLAHBLAH(2,myprintf("lvl %<mag %d>, test du mot %<mag %.*s>, score=%<mag %d>, bscore=%d\n", level, w->len, w->w, score, best_score));
	  
	  
	  /* on bloque le mot et on essaye d'aller plus loin dans le troll */
	  sel[w->num] = level;
	  if (td->lnkcat1) {
	    int trouve1, trouve2;
	    int subscore;
	    subscore = eval_best_troll(wlst, nb_mots, td->lnkcat1, level+1, td->bonuscat1, sel, &trouve1);
	    BLAHBLAH(2,myprintf("    recherche lnk1 de %<mag %.*s>, (cat=%<mag %d>)-> trouve = %<yel %d>, subscore=%<yel %d>\n", w->len, w->w, td->lnkcat1,trouve1, subscore));
	    /* certains mots ne satisfont leur catégorie que si leur catégorie liée est elle satisfaite
	       on considère celle-ci comme OBLIGATOIRE */
	    if (td->need_lnk1 == 0 || (trouve1)) {
	      *trouve = 1;
	      score += subscore;
	    }
	    if (td->lnkcat2 && trouve1) {
	      score += eval_best_troll(wlst, nb_mots, td->lnkcat2, level+1, td->bonuscat2, sel, &trouve2);
	    }
	  } else {
	    *trouve = 1;
	  }
	  
	    /* yaisssseuuu ! */
	  if (*trouve && (score > best_score || best_score == -10000)) {
	    for (i=0; i < nb_mots; i++) {
	      best_sel[i] = sel[i];
	    }
	    best_score = score;
	  }
	}
      }
    }
    w = w->next;
  }
 stooooop:
  return ((*trouve) ? best_score : 0);
}

void
troll_detector(board_msg_info *mi) {
  int score;

  unsigned char *trans_simple  = "éèëêÊËÉÈàâáäÀÂÁÄûüùÙçÇîïíìÏÎÍÌôóòõÔÓÒÕñ";
  unsigned char *trans_simple2 = "eeeeeeeeaaaaaaaauuuucciiiiiiiioooooooon";

  unsigned char txt_simple[MI_MAX_LEN];

  unsigned char *s;
  int majuscule_cnt;
  int exclamation_cnt;
  int bizarre_cnt;
  int smiley_flag;
  int tag_cnt;
  int boldwords_cnt;
  int in_url;
  int i;

  Word *wlst;

  mi->troll_score = 0;
  if (Prefs.enable_troll_detector == 0) return;

  flag_troll_braining = 1;
  score = 0;
  majuscule_cnt = 0;
  exclamation_cnt = 0;
  bizarre_cnt = 0;
  smiley_flag = 0;
  tag_cnt = 0;
  boldwords_cnt = 0;
  i = 0; s = mi->msg; assert(mi->msg);

  /* passe zero: recherche de la présence de smileys -> ça diminue le coefficient trollesque */
  if (strstr(s, ";-)") || strstr(s, ":-)") || strstr(s, ":o)") || strstr(s, "(-:") || 
      strstr(s, ":P") || strstr(s, ":)") || strstr(s, ";)") || strstr(s, ":°)") || strstr(s, ";o)")) {
    smiley_flag = 1;
  }

  /* premiere passe: filtrage de certains caractères, mise en minuscule et suppression des accents
     comptage des majuscules, exclamations et caractère graphiques */

  in_url = 0;
  
  while (*s) {
    unsigned char *p;

    /* on ignore les tags des [urls] */
    if (*s == '\t') {
      if (s[1] == '<') {
	if (strncasecmp(s, "\t<a href=\"",10) == 0) {
	  s += 10;
	  in_url++;
	  if (i < MI_MAX_LEN-5) {
	    txt_simple[i++] = '\t';
	    txt_simple[i++] = '<';
	    txt_simple[i++] = 'a';
	    txt_simple[i++] = '\t';
	    txt_simple[i++] = '>';
	  }
	  continue;
	} else if (strncasecmp(s, "\t</a\t>", 6) == 0) {
	  s += 6;
	  in_url--;
	  if (i < MI_MAX_LEN-6) {
	    txt_simple[i++] = '\t';
	    txt_simple[i++] = '<';
	    txt_simple[i++] = '/';
	    txt_simple[i++] = 'a';
	    txt_simple[i++] = '\t';
	    txt_simple[i++] = '>';
	  }
	  continue;
	}	
	if (in_url) {
	  do s++; while (*s != '\t' && *s);
	  if (*s) s++;
	}
      }
      
      
    }

    if (i == MI_MAX_LEN-1) break;

    if ((p=strchr(trans_simple, *s))) {
      txt_simple[i] = trans_simple2[(p-trans_simple)]; i++; 
      s++;
    } else if (*s < ' ' && *s != '\t') {
      txt_simple[i] = ' '; i++;
      s++;
    } else if (*s >= 'A' && *s <= 'Z') {
      majuscule_cnt++;
      txt_simple[i] = 'a' + (*s - 'A'); i++; 
      s++;
    } else if (*s == '!' || *s == '?') {
      exclamation_cnt++; 
      s++;
    } else if (strchr(".;,:@*()[]{}#\"'«»", *s)) {
      txt_simple[i] = ' '; i++; 
      s++;
    } else {
      /* on vire les '/' sauf si il ferment une balise */
      if (*s == '/' && s > (unsigned char*)mi->msg && *(s-1) != '<') {
	txt_simple[i] = ' ';
      } else {
	txt_simple[i] = *s;
      }
      i++;
      if (*s > 127) { bizarre_cnt++; } 
      s++;
    }
    if (i == MI_MAX_LEN-1) break;
  }
  txt_simple[i] = 0;
  BLAHBLAH(2,myprintf(_("troll_detector, initial message : %<YEL %s>\n"), mi->msg));
  /*  myprintf("message filtré : %<GRN %s>\n", txt_simple); */
  /*
    passe deux: construction de la liste de mots avec identification des tags, 
     séparation des mots se terminant par des chiffres, du genre 'emacs21' en 'emacs' '21' 
     tout cela sans modifier les mots du genre 'na><0r'
  */
  s = txt_simple; wlst = NULL;
  i = 0;
  while (*s) {
    unsigned char *p;
    Word *sublst;

    while (*s && *s <= ' ' && *s != '\t') s++; 
    if (*s == 0) break;

    p = s;
    while (*p && (*p > ' ' || *p == '\t')) p++;

    sublst = decoupe_bloc(s, p, 0);
    wlst = wordlist_merge(wlst, sublst);
    s = p;
  }
  ALLOW_X_LOOP;

  if (wlst == NULL) goto cassos;

  /* comptage du nombre de tags,
     ainsi que du nombre de mots mis en gras */
  {
    Word *w = wlst;
    int in_bold=0;
    in_url=0;

    while (w) {
      if (w->in_tag && w->len == 2 && w->w[0] == '/' && w->w[1] == 'b') {
	in_bold = 0;
      }
      if (in_bold && !in_url) {
	boldwords_cnt++;
      }

      /* on ne compte pas les tags fermant */
      if (w->in_tag && w->w[0] != '/') {
	if (!(w->len == 1 && (w->w[0] == 'i' || w->w[0] == 'a'))) {
	  tag_cnt ++; /* on ne compte pas les <i>, c'est des tags gentils, mais on compte tout le reste */
	}
	if (w->len == 1 && w->w[0] == 'b') {
	  in_bold = 1;
	}
	if (w->len == 1 && w->w[0] == 'a') {
	  in_url = 1;
	}
      }
      if (in_url && w->len == 2 && w->w[0] == '/' && w->w[1] == 'a') {
	in_url = 0;
      }
      w = w->next; 
    }
  }


  if (Prefs.verbosity > 1) {
    Word *w;
    w = wlst;
    myprintf("liste de mots: (smiley_flag = %<bld %d>, bizarre_cnt = %<bld %d>, exclam_cnt=%<bld %d>, majuscule_cnt=%<bld %d>, tag_cnt=%<bld %d>\n",
	     smiley_flag, bizarre_cnt, exclamation_cnt, majuscule_cnt, tag_cnt); 
    while (w) { 
      assert(w->len); 
      if (w->in_tag == 0) {
	myprintf("'%<GRN %.*s>' ", w->len, w->w); 
      } else {
	myprintf("'%<MAG %.*s>' ", w->len, w->w); 
      }
      w = w->next;
    }
    printf("\n\n");
  }

  /* recherche des mots trollesques */

  {
    Word *w;
    w = wlst;
    while (w) {
      unsigned hash_codes[MAX_WLEN];

      int i,j;
      char s[MAX_WLEN+1];
      
      strncpy(s, w->w, MAX_WLEN); s[MIN(MAX_WLEN, w->len)] = 0; 


      /* calcul du hash code pour differentes longueurs */
      hash_codes[0] = hash_codes[1] = 0;
      for (i = 2; i < MAX_WLEN; i++) {
	hash_codes[i] = str_hache(s, MIN(i, w->len));
      }
      
      /* trouve-t-on un mot troll correspondant ? */
      
      
      w->td_idx = NULL;
      w->nb_td_idx = tdata_lookup(hash_codes, MIN(w->len, MAX_WLEN-1), &w->td_idx);

      if (w->nb_td_idx == 0) {
	/* si pas trouvé, deuxieme chance: on */
	/* enleeeeeeeeve leeeees letttttres en douuublons */
	i=0; j=0;
	while (s[i]) {
	  s[j] = s[i];
	  while (s[j] == s[i]) i++;
	  j++;
	}
	s[j] = 0;
	for (i = 2; i < MAX_WLEN; i++) {
	  hash_codes[i] = str_hache(s, MIN(i, (int)strlen(s)));
	}
	w->nb_td_idx = tdata_lookup(hash_codes, MIN(strlen(s), MAX_WLEN-1), &w->td_idx);
      }

      if (w->nb_td_idx > 0) {
	BLAHBLAH(2,myprintf("mot troll trouvé: '%<MAG %.*s>' (idx = ", w->len,w->w);
		 for (i=0; i < w->nb_td_idx; i++) printf("%d ", w->td_idx[i]); printf(")\n"));
      }
      w = w->next;
    }
  }

  /* 
     suppression des doublons (genre 'coin coin coin coin coin coin coin ... ') -> 
     on ne conserve qu'un seul exemplaire (et qui ne sera utilise que dans un troll)

     exception: si le mot_troll est present en plusieurs exemplaires dans troll_data,
     alors on le laisse en autant d'exemplaires (bonne idee? mauvaise idee ?)
  */

  {
    Word *w;
    int wcnt[NB_TROLL_WORDS];
    int i;
    
    for (i=0; i < NB_TROLL_WORDS; i++) wcnt[i] = 0;
    w = wlst;
    while (w) {
      int keep_word;
      
      keep_word = 0;
      for (i=0; i < w->nb_td_idx; i++) {
	if (wcnt[w->td_idx[i]] == 0) {
	  wcnt[w->td_idx[i]]++;
	  keep_word = 1;
	}
      }
      if (keep_word == 0) {
	w->nb_td_idx = 0; /* marqué pour effecament */
      }
      w = w->next;
    }
  }


  do {
    int nb_mots;
    Word *w;

    /* on vire les mots dont le nb_td_idx == 0 */

    wlst = wordlist_cleanup(wlst);

    if (wlst == NULL) break; /* tous les mots ont été évalués */

    /* comptage et numérotation des mots restants */
    nb_mots = 0;
    w = wlst;
    while (w) {
      w->num = nb_mots++; 
      assert(nb_mots < 1000);
      w = w->next;
    }

    assert(nb_mots > 0);

    {
      char *selection;
      int sub_score;
      int i, trouve;

      /* tableau stockant la liste des mots selectionnes pour le troll */
      selection = malloc(nb_mots);
      for (i=0; i < nb_mots; i++) selection[i] = 0;

      /* anti-je-tourne-en-rond */
      cnt_anti_blocage = 0;

      /* recherche du plus gros troll dans la liste de mots */
      sub_score = eval_best_troll(wlst, nb_mots, 0, 1, 0, selection, &trouve);
      if (cnt_anti_blocage > MAX_CNT_ANTI_BLOCAGE) {
	BLAHBLAH(2,myprintf(_("%<RED this one was too hard> ! (nb_words = %d)\n txt='%s'\n"), nb_mots, mi->msg));
      }

      if (trouve == 0) {
	BLAHBLAH(2,printf(_(" -> unable to use the words left in a troll, this is the end\n")));
	free(selection);
	break;
      }
      
      score += sub_score;

      BLAHBLAH(2,myprintf(_("score = %<YEL %d> (sub_score=%<YEL %d>), the following words were used: "), score, sub_score));
      /* on marque les mots selectionnés pour la suppression à la prochaine étape */
      w = wlst;
      while (w) {
	if (selection[w->num]) { 
	  w->nb_td_idx = 0; 
	  BLAHBLAH(2,myprintf("'%<CYA %.*s>' (lvl=%d) ", w->len,w->w,  selection[w->num]));
	}
	w = w->next;
      }
      BLAHBLAH(2,printf("\n"));

      free(selection);
    }
  } while (1);

  while (wlst) {
    Word *w;
    w = wlst;
    if (w->td_idx) free(w->td_idx); w->td_idx = NULL;
    wlst = w->next;
    free(w);
  }

  /* ajout de divers bonus :) */
  {
    int msglen;
    int bonus;

    msglen = strlen(txt_simple);
    if (majuscule_cnt > msglen/3 && msglen > 5) {
      bonus = MIN(majuscule_cnt / 6, 3);
      if (majuscule_cnt > msglen*4/5 && msglen > 10) {
	bonus += 1;
      }
      BLAHBLAH(2,myprintf(_("bonus for using capitals: %<YEL %d>\n"), bonus));
      score += bonus;
    }
    
    if (exclamation_cnt > 4 && msglen > 6) {
      bonus = 1;
      if (exclamation_cnt > 8) bonus += 3;
      BLAHBLAH(2,myprintf(_("bonus for exclamation marks: %<YEL %d>\n"), bonus));
      score += bonus;
    }
    if (bizarre_cnt > 2 && msglen > 2) {
      bonus = 1;
      if (bizarre_cnt > msglen/4) {
	bonus += 3;
      }
      BLAHBLAH(2,myprintf(_("bonus for abusive use of weird characters: %<YEL %d>\n"), bonus));
      score += bonus;
    }
    if (tag_cnt>1) {
      bonus = MIN(tag_cnt/2, 6);
      BLAHBLAH(2,myprintf(_("bonus for abusive use of html tags: %<YEL %d>\n"), bonus));
      score += bonus;
    }
    if (boldwords_cnt > 1) {
      bonus = MIN(boldwords_cnt/2, 6);
      BLAHBLAH(2,myprintf(_("bonus for abusive usage of BOLD: %<YEL %d>\n"), bonus));
      score += bonus;
    }
    if (smiley_flag) { /* attenuateur de troll */
      score /= 2;
      BLAHBLAH(2,myprintf(_("%<bld attenuation for using a smiley>\n")));
    }
    
    BLAHBLAH(2,myprintf(_("%<WHT final score: >%<YEL %d>\n\n"), score));
  }


  mi->troll_score = score;

  


 cassos:
    flag_troll_braining = 0;
}
