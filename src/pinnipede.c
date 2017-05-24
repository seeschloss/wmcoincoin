/*
  rcsid=$Id: pinnipede.c,v 1.108 2005/09/26 21:40:24 pouaite Exp $
  ChangeLog:
    Revision 1.78  2002/09/21 11:41:25  pouaite 
    suppression du changelog
*/
#include <X11/keysym.h>
#include "pinnipede.h"
#include "kbcoincoin.h"
#include "xpms/miniduck.xpm"

inline static int
pp_thread_filter_find_id(const struct _PinnipedeFilter *f, id_type id) {
  int i;
  for (i=0; i < f->nid; i++) {
    if (id_type_eq(f->id[i],id)) return 1;
  }
  return 0;
}

inline int
filter_msg_info(const board_msg_info *mi, const struct _PinnipedeFilter *filter)
{
  /* cas particulier: la boitakon */
  if (mi->in_boitakon && filter->filter_boitakon) return 0;

  if (filter->visible_sites[id_type_sid(mi->id)] == 0) return 0;
  if (filter->filter_mode == 0) return 1;
  if (filter->anything) {
    if (filter->anything[0] == 0) return 1;
    else
      return ((str_noaccent_casestr(mi->msg, filter->anything) != NULL) ||
              (str_noaccent_casestr(mi->useragent, filter->anything) != NULL) ||
              (str_noaccent_casestr(mi->login, filter->anything) != NULL));
  } else if (filter->ua) {
    return (strcmp(filter->ua, mi->useragent) == 0);
  } else if (filter->login) {
    return (strcmp(filter->login, mi->login) == 0);
  } else if (filter->word && strlen(filter->word)) {
    return (str_noaccent_casestr(mi->msg, filter->word) != NULL);
  } else if (filter->id != NULL) {
    return pp_thread_filter_find_id(filter, mi->id);
  } else {
    return 1;
  }
}



/* les deux fonctions suivantes permettent de se balader dans la liste des posts 
 (de maniere bourrine... c pas pour 250000 messages )
*/
id_type
get_next_id_filtered(Boards *boards, id_type id, board_msg_info **nmi, struct _PinnipedeFilter *filter) 
{
  board_msg_info *mi;
  id_type nid;

  nid = id_type_invalid_id();
  mi = boards_find_id(boards, id);
  if (mi) {
    mi = mi->g_next;
    while (mi) {
      if (filter == NULL || filter_msg_info(mi,filter)) {
	nid = mi->id;
	break;
      }
      mi = mi->g_next;
    }
  }
  if (nmi) *nmi = mi;
  return nid;
}

static id_type
get_prev_id_filtered(Boards *boards, id_type id, board_msg_info **prev, const struct _PinnipedeFilter *filter) 
{
  board_msg_info *mi;

  mi = boards_find_id(boards, id);
  if (mi) {
    mi = mi->g_prev;
    while (mi) {
      if (filter == NULL || filter_msg_info(mi,filter)) {
	break;
      }
      mi = mi->g_prev;
    }
  }

  if (prev) *prev = mi;
  if (mi) {
    return mi->id;
  }
  else return id_type_invalid_id();
}

id_type
get_last_id_filtered(Boards *boards, struct _PinnipedeFilter *filter) {
  board_msg_info *mi;

  mi = boards->last;
  while (mi) {
    if (filter == NULL || filter_msg_info(mi,filter)) {
      return mi->id;
    }
    mi = mi->g_prev;
  }
  return id_type_invalid_id();
}

id_type
get_first_id_filtered(Boards *boards, struct _PinnipedeFilter *filter) {
  board_msg_info *mi;

  mi = boards->first;
  while (mi) {
    if (filter == NULL || filter_msg_info(mi,filter)) {
      return mi->id;
    }
    mi = mi->g_next;
  }
  return id_type_invalid_id();
}

int
count_all_id_filtered(Boards *boards, struct _PinnipedeFilter *filter) {
  board_msg_info *mi;
  int nid;

  mi = boards->first;
  nid = 0;
  while (mi) {
    if (filter == NULL || filter_msg_info(mi,filter)) {
      nid++;
    }
    mi = mi->g_next;
  }
  return nid;
}

id_type
get_nth_id_filtered(Boards *boards, struct _PinnipedeFilter *filter, int n) {
  board_msg_info *mi, *last_mi;

  mi = boards->first; last_mi = NULL;
  while (mi && n > 0) {
    if (filter == NULL || filter_msg_info(mi,filter)) {
      n--;
      last_mi = mi;
    }
    mi = mi->g_next;
  }
  return (last_mi ? last_mi->id : id_type_invalid_id());
}

static int
get_id_count_filtered(Boards *boards, struct _PinnipedeFilter *filter, id_type id) {
  board_msg_info *mi;
  int cnt = 1;

  mi = boards->first;
  while (mi) {
    if (filter == NULL || filter_msg_info(mi,filter)) {
      if (id_type_eq(mi->id, id)) {
	return cnt;
      }
      cnt++;
    }
    mi = mi->g_next;
  }
  return -1;
}

static void
pp_unset_filter(struct _PinnipedeFilter *f)
{
  f->filter_mode = 0;
  if (f->filter_name) { free(f->filter_name); f->filter_name = NULL;}
  if (f->anything) { free(f->anything); f->anything = NULL; }
  if (f->ua) { free(f->ua); f->ua = NULL;}
  if (f->login) { free(f->login); f->login = NULL; }
  if (f->word) { free(f->word); f->word = NULL; }
  if (f->id) { free(f->id); f->id = NULL; f->nid = 0; }
}

void
pp_visited_links_add(Pinnipede *pp, const char *s)
{
  int id = str_hache(s, strlen(s));
  if (pp->nb_visited_links == MAX_VISITED_LINKS) {
    memmove(pp->visited_links, pp->visited_links+1, (MAX_VISITED_LINKS-1)*sizeof(int));
  } else pp->nb_visited_links++;
  pp->visited_links[pp->nb_visited_links-1] = id;
}

int
pp_visited_links_find(Pinnipede *pp, const char *s)
{
  int id = str_hache(s, strlen(s));
  int i;
  assert(pp);
  for (i=pp->nb_visited_links-1; i >= 0; --i) 
    if (pp->visited_links[i] == id) return 1;
  return 0;
}

static void
pv_destroy(PostVisual *pv)
{
  PostWord *pw,*pw2;

  //  printf("pv_destroy(id=%d)\n", pv->id);
  pw = pv->first;
  while (pw) {
    pw2 = pw->next;
    free(pw);
    pw = pw2;
  }
  free(pv);
}

static PostWord*
pw_create(const unsigned char *w, const unsigned char *raw, unsigned attr, const unsigned char *attr_s, PostVisual *parent)
{
  int alen; 
  int wlen;
  int rawlen;
  PostWord *pw;

  wlen = strlen(w);
  rawlen = strlen(raw);
  if (attr_s) alen=strlen(attr_s); else alen = -1;
  pw = malloc(sizeof(PostWord)+wlen+1+rawlen+1+alen+1);
  pw->w = ((unsigned char*)pw)+sizeof(PostWord);
  strcpy(pw->w,w);

  pw->raw = ((unsigned char*)pw)+sizeof(PostWord)+wlen+1;
  strcpy(pw->raw, raw);

  if (attr_s) {
    pw->attr_s = ((unsigned char*)pw)+sizeof(PostWord)+wlen+1;
    strcpy(pw->attr_s, attr_s);
  } else pw->attr_s = NULL;
  pw->next = NULL;
  pw->xpos = pw->xdraw = -1; pw->ligne=-1; pw->xwidth = 0; 
  pw->attr = attr;
  pw->parent = parent;
  return pw;
}

static void
pw_add_style(PostWord *pw, FontStyle *fs)
{
  if (fs && pw) {
    if (fs->underlined) { pw->attr |= PWATTR_U; }
    if (fs->slanted) { pw->attr |= PWATTR_IT; }
    if (fs->bold) { pw->attr |= PWATTR_BD; }
    if (fs->teletype) { pw->attr |= PWATTR_TT; }
  }
}

/* remplace de maniere +/- aleatoire un mot par plop, grouik etc.. 
   en regardant aussi le mot precedent
*/
static void
plopify_word(unsigned char *s_src, unsigned sz, int bidon)
{
  unsigned char s_simple[sz], s_dest[sz];

  static char *not_plop[] = {"alors", 
			     "amha", 
			     "apres",
			     "aura", 
			     "auras",
			     "auraient",
			     "aurais",
			     "aurait",
			     "aurez",
			     "auront",
			     "aussi",
			     "autant",
			     "avant", 
			     "avec", 
			     "avoir", 
			     "beaucoup",
			     "ben",
			     "bof",
			     "celle",
			     "ces", 
			     "cette", 
			     "ceux",
			     "chez", 
			     "comme",
			     "comment", 
			     "dans",
			     "deja",
			     "depuis", 
			     "des", 
			     "devez",
			     "devons",
			     "devrez",
			     "devrons",
			     "dire",
			     "dois",
			     "doivent",
			     "donc", 
			     "dont",
			     "elle", 
			     "elles",
			     "encore",
			     "entre", 
			     "est", 
			     "etait", 
			     "ete",
			     "etes", 
			     "etre",
			     "fait", 
			     "fallait",
			     "falloir",
			     "fallu",
			     "faudra",
			     "faudrait",
			     "faut",
			     "fera", 
			     "ils", 
			     "la", 
			     "les", 
			     "leur", 
			     "leurs",
			     "lui",
			     "mais",
			     "meme", 
			     "mes", 
			     "mien", 
			     "moi"
			     "moins", 
			     "mon", 
			     "non", 
			     "nos", 
			     "notre", 
			     "nous", 
			     "ont", 
			     "ouais", 
			     "oui", 
			     "par", 
			     "parce", 
			     "pas", 
			     "pas", 
			     "peu", 
			     "peux",
			     "plus", 
			     "pour", 
			     "pourquoi", 
			     "pourtant",
			     "puis", 
			     "quand", 
			     "que", 
			     "quel",
			     "qui",
			     "quoi", 
			     "sais",
			     "sait",
			     "sans", 
			     "sera", 
			     "ses", 
			     "sien",
			     "son", 
			     "sont", 
			     "sur", 
			     "tant", 
			     "tien", 
			     "toujours",
			     "tous", 
			     "tout", 
			     "toutes", 
			     "une", 
			     "vas",
			     "vais", 
			     "vos", 
			     "votre", 
			     "vous", 
			     NULL};

  static unsigned nb_not_plop = 0;
  static unsigned *not_plop_hached = NULL;

  unsigned i;
  unsigned hache_s = 0;
  unsigned src_pos, dest_pos, s_len;

  const char *s_bizarre = "'\",;:/!+=)]@^_\\-|([{}#~?.*$³²¹";

  /* comptage des mots à ne pas plopifier, et 'hachage' des ces mots
     (pour les detecter plus rapidement) 

     ici: initialisation des données (éxécuté au premier appel)
  */
  if (nb_not_plop == 0) {
    while (not_plop[nb_not_plop] != NULL) {
      nb_not_plop++;
    }
    ALLOC_VEC(not_plop_hached, nb_not_plop, int);
    for (i=0; i < nb_not_plop; i++) {
      not_plop_hached[i] = str_hache(not_plop[i], strlen(not_plop[i]));
    }
  }

  /* enleve les accents et vérifie que le mot ne contient que des lettres */
  
  src_pos = 0; dest_pos = 0;
  do {
    int do_plopify;

    do_plopify = 1;

    /* on copie les caractères bizarres qui peuvent preceder le mot */
    while (s_src[src_pos] && strchr(s_bizarre,s_src[src_pos]) && 
	   dest_pos < sz-1) { 
      s_dest[dest_pos++] = s_src[src_pos++]; 
    }
    if (s_src[src_pos]==0) break;
    if (dest_pos == sz-1) break;
    /* on copie le mot dans s_simple */
    s_len = 0;
    while (s_src[src_pos+s_len] && !strchr(s_bizarre, s_src[src_pos+s_len])) {
      s_simple[s_len] = s_src[src_pos+s_len]; s_len++;
    }
    s_simple[s_len] = 0;    assert(s_len > 0);

    //    strcpy(s_simple, s);
    str_noaccent_tolower(s_simple);
  
    
    for (i = 0; i < s_len; i++) {
      if (!(s_simple[i] >= 'a' && s_simple[i] <= 'z')) {
	do_plopify = 0; break;
      }
    }

    if (s_len < 3) do_plopify = 0;

    if (do_plopify) {
      hache_s = str_hache(s_simple, s_len);
      for (i=0; i < nb_not_plop; i++) {
	if (hache_s == not_plop_hached[i]) { do_plopify = 0; break; }
      }
    }

    /* j'ai pas été très inspiré en écrivant tout ça, l'est po clair */

    if (do_plopify) {
      /* longeur > 10 => substitution assurée,
         longuer 0 => 1/2 chance sur deux */
      int mod = Prefs.nb_plop_words + (10-MIN(s_len,10))*Prefs.nb_plop_words/10;

      hache_s = (hache_s ^ bidon) % mod;
      if (hache_s >= Prefs.nb_plop_words) do_plopify = 0;
    }
    
    if (do_plopify) {
      i = 0; 
      while (Prefs.plop_words[hache_s][i] && dest_pos < sz-1) {
	s_dest[dest_pos++] = Prefs.plop_words[hache_s][i++];
      }
      src_pos += s_len;
    } else {
      i = 0;
      while (i < s_len && dest_pos < sz) {
	s_dest[dest_pos++] = s_src[src_pos++]; i++;
      }
    }
  } while (s_src[src_pos]);
  s_dest[dest_pos] = 0;
  strcpy(s_src, s_dest);
}


/* construction d'un postvisual à partir du message 'mi' */
static PostVisual *
pv_tmsgi_parse(Pinnipede *pp, Board *board, board_msg_info *mi, int with_seconds, int html_mode, int nick_mode, int troll_mode, int disable_plopify, KeyList *hk_plop, KeyList *hk_hili) {
#define PVTP_SZ 768

  PostVisual *pv;
  PostWord *pw, *tmp;
  
  unsigned char s[PVTP_SZ];
  unsigned char raw[PVTP_SZ];
  unsigned char attr_s[PVTP_SZ];

  const unsigned char *p, *np;
  unsigned attr;
  int add_word;
  int has_initial_space; // indique si le prochain mot est collé au precedent

  ALLOC_OBJ(pv, PostVisual);

  pv->first = NULL;
  pv->nblig = -1;
  pv->ref_cnt = 0;
  pv->next = NULL;
  pv->id = mi->id;
  pv->tstamp = mi->timestamp;

  pv->sub_tstamp = mi->sub_timestamp;
  pv->is_my_message = mi->is_my_message;
  pv->is_answer_to_me = mi->is_answer_to_me;

  pv->is_skipped_id = board_find_id(board, mi->id.lid-1) ? 0 : 1;

  pv->is_hilight_key = 0;
  if (hk_hili) {
    pv->is_hilight_key = hk_hili->num+1; assert(hk_hili->num < NB_PP_KEYWORD_CATEG);
  }
  pv->is_plopified = 0;
  if (hk_plop) {
    pv->is_plopified = (disable_plopify ? 1 : hk_plop->num+2);
  }

  if (mi->in_boitakon) {
    pv->is_plopified = 1; /* pour afficher les messages boitakonnés
			     par contagion quand disable_plopify==1 */
    assert(disable_plopify==1); /* un bon piège à bug */
  } 
    
  /*
  printf("pv = %p\n", pv);
  printf("  pv = %d\n", pv->id);
  printf("  pv->is_my_message = %d\n", pv->is_my_message);
  printf("  pv->is_answer = %d\n", pv->is_answer_to_me);
  printf("  pv->is_high = %d\n", pv->is_hilight_key);
  */

  pw = NULL;

  if (troll_mode && Prefs.enable_troll_detector) {
    if (mi->troll_score > 0) {
      snprintf(s, PVTP_SZ, "%d", mi->troll_score);
    } else {
      snprintf(s, PVTP_SZ, "  ");
    }

    tmp = pw_create(s, s, PWATTR_TROLLSCORE | (mi->troll_score > 2 ? PWATTR_BD : 0), NULL, pv);
    if (pw == NULL) { pv->first = tmp; } else { pw->next = tmp; }
    pw = tmp;
  }

  /* affichage timestamp */
  time_t tnow = time(NULL);
  char sdate[20]; sdate[0] = 0;
  if ((tnow / (24*3600)) - (pv->tstamp / (24*3600)) >= 2) { /* on va mettre la date */
    struct tm *t = localtime(&pv->tstamp);
    snprintf(sdate,20,"%04d-%02d-%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday);
  }
  if (with_seconds) {
    snprintf(s, PVTP_SZ, "%s%02d:%02d:%02d",sdate, mi->hmsf[0], mi->hmsf[1], mi->hmsf[2]);
  } else {
    snprintf(s, PVTP_SZ, "%s%02d:%02d",sdate, mi->hmsf[0], mi->hmsf[1]);
  }
  
  tmp = pw_create(s, s, PWATTR_TSTAMP | (pw == NULL ? 0 : PWATTR_HAS_INITIAL_SPACE), NULL, pv);  
  if (pw == NULL) { pv->first = tmp; } else { pw->next = tmp; }
  pw = tmp;
  pw_add_style(pw, &board->site->prefs->pp_clock_style);

  if (nick_mode) {
    char *p;
    //#define SUA_SZ 15
    //char short_ua[SUA_SZ];
    int is_login;

    /*

    popopo !! a reparer

    if (mi->tatouage) {
      p = mi->tatouage->name;
      if (strcmp(mi->tatouage->name, "?") == 0) {
    */
    //make_short_name_from_ua(mi->useragent, short_ua, SUA_SZ);
	p = mi->miniua.name;
	/*      }
    } else p = "[???]";
	*/

    is_login = 0;
    if (nick_mode == 4 && strlen(mi->login) != 0) {
      p = mi->login; is_login = 1;
    }
    
    if (nick_mode == 1 || nick_mode == 3 || nick_mode == 4) {
      tmp = pw_create(p, p, (is_login == 0 ? PWATTR_NICK : PWATTR_LOGIN) | PWATTR_HAS_INITIAL_SPACE, NULL, pv);
      if (pw == NULL) { pv->first = tmp; } else { pw->next = tmp; }
      pw = tmp;
      pw_add_style(pw, (is_login == 0 ? &board->site->prefs->pp_ua_style : &board->site->prefs->pp_login_style));
    }

    if ((nick_mode == 2 || nick_mode == 3) && strlen(mi->login)) {
      tmp = pw_create(mi->login, mi->login, PWATTR_LOGIN | PWATTR_HAS_INITIAL_SPACE, NULL, pv);
      if (pw == NULL) { pv->first = tmp; } else { pw->next = tmp; }
      pw = tmp;      
      pw_add_style(pw, &board->site->prefs->pp_login_style);
    }
  }

  p = mi->msg;
  attr = 0;

  if (pv->is_plopified == 3) p = "plop"; /* bienvenue dans le monde de la hard plopification */
#ifdef NO_BITFIELDS
  assert(pv->is_plopified <= 3);
#else
  ;
#endif

  has_initial_space = 1;
  while (p) {
    add_word = 1;
    if (board_get_tok(&p,&np,s,PVTP_SZ, &has_initial_space) == NULL) { break; }

    /* nouveau (v2.3.5) tous les '<' et '>' provenant d'authentiques tags
       html ont été préfixés d'une tabulation par 'convert_to_ascii', ce qui évite
       de se retrouver à interpreter à moitié un message du style "<a href='prout'>" 
       (par contre, y'a quelques feintes au trollo qui marcheront plus ;)
    */

    if (s[0] == '\t' && html_mode) {
      add_word = 0;
      if (strcasecmp(s, "\t<i\t>")==0) {
	attr |= PWATTR_IT; 
      } else if (strcasecmp(s,"\t</i\t>")==0) {
	attr &= (~PWATTR_IT);
      } else if (strcasecmp(s,"\t<b\t>")==0) {
	attr |= PWATTR_BD; 
      } else if (strcasecmp(s,"\t</b\t>")==0) {
	attr &= (~PWATTR_BD);
      } else if (strcasecmp(s,"\t<u\t>")==0 ||
		 strcasecmp(s, "\t<span style=\"text-decoration: underline\"\t>")==0) {
	attr |= PWATTR_U;
      } else if (strcasecmp(s,"\t</u\t>")==0) {
	attr &= (~PWATTR_U);
      } else if (strcasecmp(s,"\t<s\t>")==0 ||
		 strcasecmp(s, "\t<span style=\"text-decoration: line-through\"\t>")==0) {
	attr |= PWATTR_S; 
      } else if (strcasecmp(s, "\t</span\t>")==0) { /* lequel choisir... fait chier :-/ */
	if (attr & PWATTR_S) attr &= ~PWATTR_S; /* dtc si c pas le bon ordre */
	else if (attr & PWATTR_U) attr &= ~PWATTR_U;
      } else if (strcasecmp(s,"\t</s\t>")==0) {
	attr &= (~PWATTR_S);
      } else if (strcasecmp(s,"\t<tt\t>")==0) {
	attr |= PWATTR_TT;
      } else if (strcasecmp(s,"\t</tt\t>")==0) {
	attr &= (~PWATTR_TT);
      } else if (strncasecmp(s,"\t<a href=\"", 10)==0) {
	int i;
	char *url;
	attr |= PWATTR_LNK;
        /* MODIF lo v2.4.6a 05/08/03 ITEM#FFE902  (j'ai presque l'impression d'%/1€Œiso8859-15ê
           tre au taff) */
        /* Oooops. S'il y a un target qui traine, tout est p%/1€Œiso8859-15ét%/1€Œiso8859-15é
         * Alors on va plut%/1€Œiso8859-15ôt chercher la fin depuis le d%/1€Œiso8859-15ébut
         * (sisi, suivez moi bien
         */
        i=strlen("\t<a href=\"") + 1;
        while (s[i] && s[i] != '\"') i++;
        /* FIN_MODIF lo v2.4.6a 05/08/03 ITEM#FFE902 */
	s[i] = 0;
	url = s+10;
	if (url[0] == '.') { /* chemin relatif :-/ */	  
	  char *tmp = strdup(board->site->relative_urls_base);
	  url_au_coiffeur(tmp, 1); /* vire le nom du backend */
	  tmp = str_cat_printf(tmp, "/%s", url);
	  url_au_coiffeur(tmp, 0);
	  snprintf(attr_s, PVTP_SZ, "%s", tmp);
	  free(tmp);
	} else {
	  strncpy(attr_s, url, PVTP_SZ); 
	}
      } else if (strcasecmp(s,"\t</a\t>")==0) {
	attr &= (~PWATTR_LNK);
      } else {
	fprintf(stderr, "un tag qui pue ? '%s'\n", s);
	add_word = 1;
      }
    }
    if (add_word) {
      strcpy(raw, s);
      int is_ref;

      if ((attr & PWATTR_LNK) == 0) {
        check_for_horloge_ref(board->boards, mi->id, s,attr_s, PVTP_SZ, &is_ref, NULL);
        if (is_ref) {
          attr |= PWATTR_REF;

          if (strlen(s) > 10) {
              if (s[10] == 'T') {
                  // les horloges de devnewton< indiquent la date à tort et à travers
                  // on va simplement ne pas l'afficher.
                  strcpy(s, s + 11);
              }
          }
        } else {
            check_for_id_ref(board->boards, mi->id, s, attr_s, PVTP_SZ, &is_ref);

            if (is_ref) {
              attr |= PWATTR_REFID;
            }
        }
      }

      if (has_initial_space) {
	attr |= PWATTR_HAS_INITIAL_SPACE;
      } else {
	attr &= (~PWATTR_HAS_INITIAL_SPACE);
      }

      if (pv->is_plopified) {
	attr &= ~(PWATTR_U | PWATTR_BD | PWATTR_S | PWATTR_IT);
	
	if (pv->is_plopified >1) {
	  int i;
	  for (i=0; s[i]; i++) { if (s[i] >= 'A' && s[i] <= 'Z') s[i] = s[i]+'a'-'A'; }
	  if (strlen(s) >= 3 && Prefs.nb_plop_words > 0) {
	    plopify_word(s, PVTP_SZ,  (char*)p - (char*)mi->msg);
	  }
	}
      }

      if ((attr & PWATTR_LNK) && attr_s[0]) {
	if (pp_visited_links_find(pp, attr_s)) attr |= PWATTR_VISITED;
      }

      if (Prefs.board_enable_hfr_pictures &&
          s[0] == '[' && s[1] == ':' && strlen(s) > 4 && s[strlen(s)-1] == ']') { /* totoz ? */
        const char *st = "bug...";
        int z = pp_totoz_img_status(pp,s);
        switch (z) {
        case PP_TOTOZ_STATUS_UNKNOWN: attr |= PWATTR_TOTOZ_UNKNOWN; st = "unknown"; break;
        case PP_TOTOZ_STATUS_DOWNLOADING: attr |= PWATTR_TOTOZ_DOWNLOADING; st = "download in progress"; break;
        case PP_TOTOZ_STATUS_NOTFOUND: attr |= PWATTR_TOTOZ_NOTFOUND; st = "not found"; break;
        case PP_TOTOZ_STATUS_FOUND: attr |= PWATTR_TOTOZ_FOUND; st = "found"; break;
        }
        BLAHBLAH(2,myprintf("TOTOZ: %<GRN %s> [status = %<GRN %s>]\n",s,st));
      }

      pw->next = pw_create(s, raw, attr, (attr & PWATTR_LNK) ? attr_s : NULL, pv);
      has_initial_space = 0;
      attr &= ~(PWATTR_REF | PWATTR_REFID | PWATTR_VISITED | PWATTR_TOTOZ) ;
      //      printf("ADD(id=%d): '%s'\n", mi->id, s);
      pw = pw->next;
    }
    p=np;
  }
  return pv;
}

static CCFontId
pv_get_font(Pinnipede *pp, unsigned attr)
{
  CCFontId fn;
  if ((attr & PWATTR_BD) && (attr & PWATTR_IT)) {
    fn = pp->fn_itbd;
  } else if (attr & PWATTR_IT) {
    fn = pp->fn_it;
  } else if (attr & (PWATTR_BD)) {
    fn = pp->fn_bd;
  } else if (attr & (PWATTR_TT)) {
    fn = pp->fn_mono;
  } else {
    fn = pp->fn_base;
  }
  return fn;
}

static void
pv_justif(Pinnipede *pp, PostVisual *pv, int x0, int width) {
  int x, start_of_line;
  PostWord *pw;
  CCFontId fn;
  int trollscore_width = 0;
  short xoffset;

  pw = pv->first;
  start_of_line = 1;
  x = x0;
  fn = pp->fn_base;
  pv->nblig = 0;

  if (pp->trollscore_mode) {
    fn = pv_get_font(pp, PWATTR_BD);
    trollscore_width = ccfont_text_width_utf8(fn, "0", 1);
  } else {
    trollscore_width = 0;
  }

  while (pw) {
    if (start_of_line) {
      pv->nblig++; 
    } else x += (pw->attr & PWATTR_HAS_INITIAL_SPACE) ? pp->fn_base_space_w : 0;
    fn = pv_get_font(pp, pw->attr);
    assert(pw->w); assert(strlen(pw->w));

    pw->xwidth = ccfont_text_xbox_utf8(fn, pw->w, strlen(pw->w), &pw->xpos, &xoffset);
    //pw->xwidth = ccfont_text_width_utf8(fn, pw->w, strlen(pw->w));
    
    if (pw->attr & PWATTR_TROLLSCORE) {
      pw->xwidth = MAX(pw->xwidth,trollscore_width);
    }

    pw->ligne = pv->nblig;
    pw->xdraw = x;
    pw->xpos += x; 
    x += xoffset;
    if (x > width-3 && start_of_line == 0) { 
      x = 13; start_of_line = 1;       
    } else {
      pw = pw->next;
      start_of_line = 0;
    }
  }
}

static PostVisual *
pp_find_pv(Pinnipede *pp, id_type id)
{
  PostVisual *pv;

  pv = pp->pv;
  while (pv) {
    if (id_type_eq(pv->id,id)) return pv;
    pv = pv->next;
  }
  return NULL;
}

/* ajout (si necessaire) du message 'id' dans la liste */
PostVisual *
pp_pv_add(Pinnipede *pp, Boards *boards, id_type id)
{
  PostVisual *pv;
  int with_seconds = 1;

  pv = pp_find_pv(pp, id);
  if (pv) {
    pv->ref_cnt++;
  } else {
    board_msg_info *mi;

    mi = boards_find_id(boards,id);
    if (mi == NULL) {
      return NULL;
    }

    if (pp->show_sec_mode == 0) {
      with_seconds = mi->hmsf[3];
    }

    pv = pv_tmsgi_parse(pp, boards->btab[mi->id.sid], mi, with_seconds, 1, 
			pp->nick_mode, pp->trollscore_mode, pp->disable_plopify,
			board_key_list_test_mi(boards, mi, Prefs.plopify_key_list),
			board_key_list_test_mi(boards, mi, Prefs.hilight_key_list)); 
    pv_justif(pp, pv, 11, pp->zmsg_w); // ZL win_width - (pp->sc ? SC_W-1 : 0));
    assert(pv);
    pv->next = pp->pv;
    pp->pv = pv;
  }
  return pv;
}

static int
pp_pv_uncount(Pinnipede *pp) {
  PostVisual *pv;
  int cnt = 0;

  pv = pp->pv;
  while (pv) {
    //    assert(pv->ref_cnt > 0);
    pv->ref_cnt = 0;
    pv = pv->next;
  }
  return cnt;
}

static int
pp_pv_garbage_collect(Pinnipede *pp) {
  PostVisual *pv, *npv, *ppv;
  int cnt = 0;

  pv = pp->pv;
  ppv = NULL;
  while (pv) {
    //    printf("pv = %d, next = %p\n", pv->id, pv->next);
    assert(pv->ref_cnt >= 0);
    if (pv->ref_cnt == 0) {
      npv = pv->next;
      if (pv == pp->pv) pp->pv = npv;
      pv_destroy(pv);
      if (ppv) ppv->next = npv;
      pv = npv;
      cnt++;
    } else {
      ppv = pv;
      pv = pv->next;
    }
  }
  return cnt;
}

void
pp_pv_destroy(Pinnipede *pp) {
  PostVisual *pv;

  pv = pp->pv;  
  while (pv) { pv->ref_cnt = 0; pv = pv->next; }
  pp_pv_garbage_collect(pp);
  if (pp->lignes) { free(pp->lignes); pp->lignes = NULL; }
}


static int 
pp_pv_count_horloges(PostVisual *pv) {
  PostWord *pw = pv->first;
  int cnt = 0;
  while (pw) {
    if ((pw->attr & PWATTR_REF)) ++cnt;
    pw = pw->next;
  }
  return cnt;
}

static PostWord *
pp_pv_get_nth_horloge(PostVisual *pv, int n) {
  PostWord *pw = pv->first;
  while (n && pw) {
    if ((pw->attr & PWATTR_REF)) {
      n--;
      if (n == 0) return pw;
    }
    pw = pw->next;
  }
  return NULL;
}

void
pp_selection_unselect(Pinnipede *pp) {
  if (pp->lignes_sel) free(pp->lignes_sel);
  pp->lignes_sel = NULL;
  pp->sel_l0 = pp->sel_l1 = 0;
  pp->sel_anchor_x = pp->sel_head_x = 0;
  pp->sel_anchor_y = pp->sel_head_y = 0;
}

unsigned long
pp_get_win_bgcolor(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;

  if (pp->active_tab) {
    int sid = pp->active_tab->site->site_id; assert(sid>=0);
    return cccolor_pixel(pp->win_bgcolor[sid]);
  } else return WhitePixel(dock->display, dock->screennum);
}



/* mise à jour du contenu du pinnipede (reparse tous les messages affichés etc...
   c'est une des fonctions les plus importantes) */
/* adjust: param interne, appeler toujours avec adjust  = 0 */
void
pp_update_content(Dock *dock, id_type id_base, int decal, int adjust, int update_scrollbar_bounds)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;

  int cur_lig, plig;
  id_type id;
  PostVisual *pv;

  if (flag_updating_board) return; // on repousse un peu le refresh


  if (pp->lignes_sel) {
    pp_selection_unselect(pp);
  }
  if (pp->last_selected_text) {
    free(pp->last_selected_text); pp->last_selected_text = NULL;
  }

  //  printf("[colle_en_bas=%d] ..", pp->colle_en_bas);
  if (id_type_is_invalid(id_base) ||
      id_type_eq(id_base, get_last_id_filtered(boards, &pp->filter))) {
    pp->colle_en_bas = 1;
  } else pp->colle_en_bas = 0;


  pp->last_post_id = boards_last_id(boards);

  /* pour determiner la position de la zone d'affichage */
  pp_widgets_set_pos(dock);

/*   printf("id_base = %d/%d, pp->id_base=%d, last_id=%d, colle_en_bas=%d decal=%d\n", */
/*   	 id_base.sid,id_base.lid, pp->id_base.lid,  */
/* 	 get_last_id_filtered(boards, &pp->filter).lid, pp->colle_en_bas, decal); */

  pp->nb_lignes = pp->zmsg_h / pp->fn_h;
  cur_lig = pp->nb_lignes;

  /* on s'aligne sur le premier message qui n'est pas dans la boitakon */
  if (pp->disable_plopify == 0) {
    board_msg_info *mi;
    mi = boards_find_id(boards, id_base);
    while (mi && mi->in_boitakon) {
      decal = 0;
      id_base = get_prev_id_filtered(boards, id_base, &mi, &pp->filter);
    }
  }

  id = id_base;


  /* 'scroll down' */
  //printf("entree update_content: id_base = %d (%d), decal = %d (%d), adjust=%d\n",
  //id_base,pp->id_base, decal, pp->decal_base, adjust);

  pp->id_base = id_type_invalid_id();

  /* 'reduit' le decalage */
  while (decal > 0) {
    id_type nid;
    nid = get_next_id_filtered(boards, id, NULL, &pp->filter);
    if (id_type_is_invalid(nid)) { decal = 0; break; }
    id = nid;
    pv = pp_pv_add(pp, boards, id);
    if (pv == NULL) { decal = 0; break; }
    decal -= pv->nblig;
  }

  /* assignation des lignes */
  pp_pv_uncount(pp);
  if (pp->lignes) { free(pp->lignes); pp->lignes=NULL; }
  ALLOC_VEC(pp->lignes, pp->nb_lignes, PostWord*);

  while (cur_lig >= 0) {
    pv = pp_pv_add(pp, boards, id); 
    //    printf("cur_lig = %d, id=%d pv=%p\n", cur_lig, id.lid, pv);
    if (pv == NULL) { /* on n'arrive pas a remplir jusqu'en haut ? */
      if (adjust == 0) {
	if (!id_type_is_invalid(pp->id_base)) {
	  pp_update_content(dock,pp->id_base,pp->decal_base+cur_lig, 1, update_scrollbar_bounds); /* pas joli-joli */
	} else {
	  id = get_first_id_filtered(boards, &pp->filter); /* premier id affichable */

	  if (!id_type_is_invalid(id)) { /* sinon ça veut dire que la tribune est comptelemt vide */
	    pp_update_content(dock,id,0, 0, update_scrollbar_bounds); /* pas joli-joli */
	  }
	}
      }
      break;
    }
    pv->ref_cnt = 0;
    id = get_prev_id_filtered(boards, id, NULL, &pp->filter);

    //printf("pv->nblig=%d, , pid=%d\n",pv->nblig, id);
    if (decal + pv->nblig > 0) { // test si le 'scroll up a été suffisant
      PostWord *pw;

      if (id_type_is_invalid(pp->id_base)) { /* on sauve l'id_base et le decal soigneusement calculé */
	pp->id_base = pv->id;
	pp->decal_base = decal;
      }

      cur_lig -= pv->nblig + decal;
      decal = 0;
      plig = 0;
      pw = pv->first;
      for (plig = 0; plig < pv->nblig && cur_lig+plig < pp->nb_lignes; plig++) {
	int l;
	if (cur_lig + plig >= 0) {
	  //	  printf("ajout: ligne %d, pw->w='%s'\n", cur_lig+plig, pw->w);
	  pp->lignes[cur_lig+plig] = pw;
	  pv->ref_cnt = 1;
	}
	l = pw->ligne;
	while (pw && l == pw->ligne) pw = pw->next;
      }
    } else {      
      decal += pv->nblig;
    }
  }
  pp_pv_garbage_collect(pp); // rhooooo
  if (update_scrollbar_bounds) { pp_scrollcoin_update_bounds(dock); }
  if (pp->sc)
    scrollcoin_setpos(pp->sc, get_id_count_filtered(boards, &pp->filter, pp->id_base));
}


void
pp_clear_win_area(Dock *dock, int x, int y, int w, int h)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp->transparency_mode == 0 && w > 0 && h > 0) {
    //    XClearArea(dock->display, pp->win, x, y, w, h, False);
    XSetForeground(dock->display, dock->NormalGC, pp_get_win_bgcolor(dock));
    XFillRectangle(dock->display, pp->win, dock->NormalGC, x, y, w, h);
  } else {
    XCopyArea(dock->display, pp->lpix, pp->win, dock->NormalGC, x, y+pp->lpix_h0, 
	      w, h, x, y);
  }
}

/* dessine une ligne */
PostWord *
pp_draw_line(Dock *dock, Pixmap lpix, PostWord *pw, 
	     unsigned long bgpixel, PinnipedeLignesSel *sel_info, int use_bg_pixmap, int dest_y)
{
  Pinnipede *pp = dock->pinnipede;
  int pl;
  unsigned long pixel; //, old_pixel;
  CCColorId color;
  int y;
  int site_num = -1;

  if (pw) {
    assert(pw->parent);
    site_num = id_type_sid(pw->parent->id);
  } else {
    assert(pp->active_tab);
    site_num = pp->active_tab->site->site_id;
  }
  assert(site_num >=0 && site_num < MAX_SITES);

  XSetForeground(dock->display, dock->NormalGC, bgpixel);
  if (use_bg_pixmap == 0) {
    XFillRectangle(dock->display, lpix, dock->NormalGC, 0, 0, pp->zmsg_w, pp->fn_h);
  } else {
    XCopyArea(dock->display, pp->lpix, pp->lpix, dock->NormalGC, pp->zmsg_x1, dest_y+pp->lpix_h0, pp->zmsg_w, pp->fn_h, 0, 0);
  }


  if (!use_bg_pixmap) {
    XSetForeground(dock->display, dock->NormalGC, pp_get_win_bgcolor(dock));
    XFillRectangle(dock->display, lpix, dock->NormalGC, 0, 0, 4, pp->fn_h);
    //    XFillRectangle(dock->display, lpix, dock->NormalGC, pp->win_width-3-(pp->sc ? SC_W : 0), 0, 2, pp->fn_h);
  }

  /* verifie si c'est un nouveau message */
  if (pw) {
    Board *board = dock->sites->boards->btab[id_type_sid(pw->parent->id)];
    if (id_type_lid(pw->parent->id) > board->last_post_id_prev) {
      /*unsigned long pixel = 0x8080ff;
      if ((wmcc_tic_cnt - board->wmcc_tic_cnt_last_check) > 10*(1000/WMCC_TIMER_DELAY_MS)) {
	pixel = ((((bgpixel & 0xff0000) + (pixel & 0xff0000))/2 & 0xff0000) + 
		 (((bgpixel & 0x00ff00) + (pixel & 0x00ff00))/2 & 0x00ff00) + 
		 (((bgpixel & 0x0000ff) + (pixel & 0x0000ff))/2 & 0x0000ff));
      }
      XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(pixel));
      XFillRectangle(dock->display, lpix, dock->NormalGC, 0, 0, 4, pp->fn_h);*/
      unsigned long pixel = 0x106fe5; // un bleu qui claque
      int y = 0 - dest_y%4;
      XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(pixel));
      while (y < pp->fn_h) {
        XFillRectangle(dock->display, lpix, dock->NormalGC, 2, y, 2, 2);
        y += 4;
      }
    }
  }

  /* couleur de la zone selectionnée */
  if (sel_info) {
    if (sel_info->x0 < sel_info->x1) {
      XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->sel_bgcolor));
      XFillRectangle(dock->display, lpix, dock->NormalGC, 
		     sel_info->x0, 0, 
		     sel_info->x1-sel_info->x0, pp->fn_h);
    }
  }

  /* dessine une ligne horizontale pour signifier qu'il y a eu une coupure dans le
     "flux" des messages (puisque le msg id-1 n'existe pas / n'a pas été récupéré) */
  if (pw && Prefs.site[site_num]->mark_id_gaps && pw->ligne == 1 && pw->parent->is_skipped_id) {
    XGCValues xgc;
    xgc.foreground = IRGB2PIXEL(0xff0000);
    xgc.line_style = LineOnOffDash;
    
    XChangeGC(dock->display, dock->NormalGC, GCForeground | GCLineStyle, &xgc);
    XDrawLine(dock->display, lpix, dock->NormalGC, 5, 0, pp->zmsg_w, 0);
    xgc.line_style = LineSolid;
    XChangeGC(dock->display, dock->NormalGC, GCLineStyle, &xgc);
  }

  XSetBackground(dock->display, dock->NormalGC, bgpixel);
  //  old_pixel = bgpixel;

  pixel = 0L;
  y = ccfont_ascent(pp->fn_base)-1;
  if (pw) {
    CCFontId fn;
    pl = pw->ligne;
    /*
      printf("pw->parent = %d\n", pw->parent->id);
      printf("pw->parent->is_my_message = %d\n", pw->parent->is_my_message);
      printf("pw->parent->is_answer = %d\n", pw->parent->is_answer_to_me);
      printf("pw->parent->is_high = %d\n", pw->parent->is_hilight_key);
    */
    
    /* affichage du petit crochet sur la gauche de certains messages */
    if (pw->parent->is_my_message || 
	pw->parent->is_answer_to_me || 
	pw->parent->is_hilight_key) {
      PostWord *pw2;
      int first_line, last_line;
      int do_hilight = 1;
      if (pw == pw->parent->first) { first_line = 1; } else first_line = 0;
      pw2 = pw;
      while (pw2 && pw2->ligne == pl) pw2 = pw2->next;
      if (pw2 == NULL) last_line = 1; else last_line = 0;

      
      if (pw->parent->is_my_message && pp->hilight_my_message_mode) { 
	pixel = cccolor_pixel(pp->hilight_my_msg_color);
      } else if (pw->parent->is_answer_to_me && pp->hilight_answer_to_me_mode) { 
	pixel = cccolor_pixel(pp->hilight_answer_my_msg_color);
      } else if (pw->parent->is_hilight_key && pp->hilight_key_mode) { 
	pixel = cccolor_pixel(pp->hilight_keyword_color[pw->parent->is_hilight_key-1]);
      } else do_hilight = 0;
      
      if (do_hilight) {
	XSetForeground(dock->display, dock->NormalGC, pixel);
	XFillRectangle(dock->display, lpix, dock->NormalGC, 4, 0, 3, pp->fn_h);
	XDrawLine(dock->display, lpix, dock->NormalGC, 6,first_line,  6, pp->fn_h-1-last_line);
	if (first_line) XDrawLine(dock->display, lpix, dock->NormalGC, 7,0,9, 0);
	if (last_line) XDrawLine(dock->display, lpix, dock->NormalGC, 7,pp->fn_h-1,9, pp->fn_h-1);

	if (first_line) {
	  pw2 = pw;
	  while (pw2 && (pw2->attr & PWATTR_TSTAMP) == 0) pw2 = pw2->next;
	  if (pw2) {
	    int x0,width;
	    x0 = pw2->xpos-2; width = pw2->xwidth+3;
	    XDrawLine(dock->display, lpix, dock->NormalGC, 10, 0, x0, 0);
	    XDrawRectangle(dock->display, lpix, dock->NormalGC, x0,0,width,pp->fn_h-1);
	  }
	}

	//old_pixel = pixel;
      }
    }

    /* la boucle sur tous les mots */
    while (pw && pw->ligne == pl) {
      int force_bold = 0;
      if (pw->attr & PWATTR_TSTAMP) {
	color = pp->timestamp_color[site_num];
//	if (pw->parent->is_my_message) { pixel = IRGB2PIXEL(0x000080); }
      } else if (pw->attr & (PWATTR_LNK|PWATTR_REF|PWATTR_REFID)) {
	if ((pw->attr & PWATTR_VISITED) == 0) {
	  color = pp->lnk_color[site_num];
	} else {
	  color = pp->visited_lnk_color[site_num];
	}
      } else if (pw->attr & PWATTR_NICK) {
	color = pp->useragent_color[site_num];
      } else if (pw->attr & PWATTR_LOGIN) {
	color = pp->login_color[site_num];
      } else if (pw->attr & PWATTR_TROLLSCORE) {
	color = pp->trollscore_color[site_num];
      } else {
	color = pp->txt_color[site_num];
      }

      if (pw->attr & (PWATTR_TOTOZ_UNKNOWN)) {
        color = pp->totoz_unknown_color; 
      } else if (pw->attr & (PWATTR_TOTOZ_DOWNLOADING)) {
        color = pp->totoz_downloading_color; 
      } else if (pw->attr & (PWATTR_TOTOZ_NOTFOUND)) {
        color = pp->plopify_color;
      } else if (pw->attr & (PWATTR_TOTOZ_FOUND)) {
        color = pp->totoz_found_color; 
      }

      if (pw->parent->is_plopified) {
	color = pp->plopify_color;
      }

      if (pw->attr & (PWATTR_TMP_EMPH)) {
	XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->popup_bgcolor));
	XFillRectangle(dock->display, lpix, dock->NormalGC,pw->xpos, 1, pw->xwidth, pp->fn_h-1);
	color = pp->popup_fgcolor;
	//XSetForeground(dock->display, dock->NormalGC, pixel);
      }

      /*if (pixel != old_pixel) {
	XSetForeground(dock->display, dock->NormalGC, pixel);
	old_pixel = pixel;
        }*/

      fn = pv_get_font(pp, pw->attr | (force_bold ? PWATTR_BD : 0));


      //XDrawString(dock->display, lpix, dock->NormalGC, pw->xpos, y, pw->w, strlen(pw->w));
      ccfont_draw_string_utf8(fn, color, lpix, pw->xpos, y, pw->w, -1);

      if (pw->attr & PWATTR_U) {
	int x1;
	if (pw->next && pw->next->ligne == pw->ligne && (pw->next->attr & PWATTR_U)) {
	  x1 = pw->next->xpos;
	} else {
	  x1 = pw->xpos+pw->xwidth-1;
	}
        XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(color));
	XDrawLine(dock->display, lpix, dock->NormalGC, pw->xpos, y+1, x1, y+1);
      }
      if (pw->attr & (PWATTR_TMP_EMPH)) {
        XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->popup_fgcolor));
	XDrawLine(dock->display, lpix, dock->NormalGC, pw->xpos, y+1,  pw->xpos+pw->xwidth-1, y+1);
	if (pw->xwidth > 2) {
	  XDrawLine(dock->display, lpix, dock->NormalGC, pw->xpos+1, y+2, pw->xpos+pw->xwidth-2, y+2);
	}
      }

      if (pw->attr & PWATTR_S) {	  
	int x1;
	pixel = cccolor_pixel(pp->txt_color[site_num]);
	XSetForeground(dock->display, dock->NormalGC, pixel);
	if (pw->next && pw->next->ligne == pw->ligne && (pw->next->attr & PWATTR_S)) {
	  x1 = pw->next->xpos;
	} else {
	  x1 = pw->xpos+pw->xwidth-1;
	}
	XDrawLine(dock->display, lpix, dock->NormalGC, pw->xpos, y-ccfont_height(fn)/2, x1, y-ccfont_height(fn)/2);
      }
      pw->attr &= ~PWATTR_TMP_EMPH;
      pw = pw->next;
    }
  }


  return pw;
}

void pp_refresh_hilight_refs(Pinnipede *pp, Boards *boards, int sid, time_t timestamp, int sub_timestamp, board_msg_info *mi) {
  int l;

  for (l=0; l < pp->nb_lignes; l++) {
    if (pp->lignes[l]) {
      board_msg_info *ref2_mi;
      int pl;
      PostWord *pw;

      pw = pp->lignes[l];
      pl = pw->ligne;
      while (pw && pl == pw->ligne) {
	if (pw->attr & PWATTR_REFID) {
	  int is_ref;

	  ref2_mi = check_for_id_ref(boards, pw->parent->id, pw->raw, NULL, 0, &is_ref);
      assert(is_ref);
	  if (ref2_mi && mi && mi->id.lid == ref2_mi->id.lid) {
          pw->attr |= PWATTR_TMP_EMPH;
	  }
	} else if (pw->attr & PWATTR_REF) {
	  int bidon, ref2_num;

	  ref2_mi = check_for_horloge_ref(boards, pw->parent->id, pw->raw, NULL, 0, &bidon, &ref2_num); assert(bidon);
	  if (ref2_mi && ref2_mi->timestamp == timestamp) { /* test sur timestamp pour les situation où +sieurs msg ont le même */
	    if (id_type_sid(ref2_mi->id) == sid) {
	      if (ref2_num == -1                                 /* ref à plusieurs posts */
		  || (ref2_num==0 && sub_timestamp <= 0) /* au cas on a mis un ¹ inutile par inadvertance */
		  || (ref2_num == sub_timestamp)) {
		pw->attr |= PWATTR_TMP_EMPH;
	      }
	    }
	  }
	}
	pw = pw->next;
      }
    }	
  }
}

/* 
   pw_ref: si non null, c'est que le pointeur survole une référence (c'est a d qu'il
   est utilise quand on survole une reference à un post precedent, 
   ou bien quand on survole un timestamp)
   
   cette fonction est malheureusement devenue un vrai sac de noeud

*/
void
pp_refresh(Dock *dock, Drawable d, PostWord *pw_ref)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;

  int l;
  board_msg_info *ref_mi;
  unsigned char ref_comment[200];
  int ref_in_window = 0; /* mis a 1 si le message souligné par pw_ref est affiché parmi
			    les autres messages. sinon, on l'affiche en haut, dans une petite fenetre */

#define MAXANTIREF 60
  id_type anti_ref_id[MAXANTIREF];
  /* les antireferences : si on survolle un timestamp, on emphasize les commentaires qui
     lui font reference */
  int nb_anti_ref;
  int ref_num; /* utilise pour les ref précises dans les post multiples (ie qui ont le meme timestamp) 
		  par defaut, vaut -1 (cad désactivé)
                  peut valoir -2 : la ref est précise, mais on ne se sert que de l'id, pas des timestamps
		*/

  pp->flag_pp_refresh_request = 0;
  if (pp->lignes == NULL) return; /* ça peut arriver pendant que flag_updating_board != 0 */

  /*  {
    time_t t=time(NULL);
    printf("%02d:%02d:%02d in pp_refresh...\n", (t/3600)%24, (t/60)%60, t%60);
    dump_backtrace();
  }
  */
  ref_num = -1;
  ref_comment[0] = 0;

  /* effacage des bandes blanches */
  {
    int y,h;
    /* en haut */
    y = pp->zmsg_y1;
    h = LINEY0(0)-y;
    if (h>0) {
      pp_clear_win_area(dock, pp->zmsg_x1, y, pp->zmsg_w, h);
    }

    /* en bas */
    y = LINEY0(pp->nb_lignes);
    h = (pp->zmsg_y2 - y) + 1;
    if (h>0) {
      pp_clear_win_area(dock, pp->zmsg_x1, y, pp->zmsg_w, h);
    }
  }

  ref_mi = NULL;
  nb_anti_ref = 0;
  
  /* premier cas: on survole une reference */
  if (pw_ref && (pw_ref->attr & PWATTR_REF)) {
    int is_ref;
    ref_mi = check_for_horloge_ref(boards, pw_ref->parent->id, pw_ref->raw, ref_comment, 200, &is_ref, &ref_num);
    assert(is_ref);
  } else if (pw_ref && (pw_ref->attr & PWATTR_REFID)) {
    int is_ref;
    ref_mi = check_for_id_ref(boards, pw_ref->parent->id, pw_ref->raw, ref_comment, 200, &is_ref);
    assert(is_ref);
  } else if (pw_ref && (pw_ref->attr & PWATTR_LNK)) {
    char *link = strdup(pw_ref->attr_s);
    char *anchor = strrchr(link, '#');
    if (anchor && anchor > strrchr(link, '/')) *anchor = 0;
    RSSBonusInfo *ri = rss_find_from_link(boards, link);
    if (ri) {
      ref_mi = boards_find_id(boards, ri->id); 
      ref_num = -2; // pour eviter les multiples hilights
      if (ref_mi) pw_ref->attr |= PWATTR_TMP_EMPH;
    } else printf("this is not possible\n");
    free(link);
  } else if (pw_ref && (pw_ref->attr & PWATTR_TSTAMP)) {
    pp_refresh_hilight_refs(pp, boards, id_type_sid(pw_ref->parent->id), 
			    pw_ref->parent->tstamp, pw_ref->parent->sub_tstamp, pw_ref->parent);
  }
  if (ref_mi) { 
    board_msg_info *mi;
    
    /* on verifie que la ref apparait *entierement* dans la fenetre 
       -> on boucle pour les situation ou il y a plusieurs messages qui ont le meme timestamp 
    */
    mi = ref_mi;
    while (mi && mi->timestamp == ref_mi->timestamp && 
           id_type_sid(mi->id) == id_type_sid(ref_mi->id)) {
      ref_in_window = 0;
      for (l=0; l < pp->nb_lignes; l++) {
        if (pp->lignes[l]) {
          if (id_type_eq(pp->lignes[l]->parent->id, ref_mi->id)) {
            if (ref_in_window == 0) {
              /* sale ruse... si toutes les lignes sont là, a la fin du FOR on obtient ref_in_window = 1 */
		ref_in_window = pp->lignes[l]->parent->nblig;
            } else {
              ref_in_window--;
            }
          }
        }
      }
      if (ref_in_window != 1) {
        ref_in_window = 0;
        break;
      }
      get_next_id_filtered(boards, mi->id, &mi, NULL);
      if (ref_num >= 0) break; /* si c'est pas une ref à un multipost sans précision, break (chuis pas clair) */
    }
    
    /* et maintenant on detecte toutes les autres references vers ce message pour les afficher
       temporairement en gras (ça c vraiment pour faire le cakos)*/
    pp_refresh_hilight_refs(pp, boards, id_type_sid(ref_mi->id), 
                            ref_mi->timestamp, ref_mi->sub_timestamp, ref_mi);
  }

  /* 
     affichage du contenu de la tribune 
  */

  for (l=0; l < pp->nb_lignes; l++) {
    PostWord *pw;
    unsigned long bgpixel;
    int opaque_bg;
    
    pw = pp->lignes[l];
    opaque_bg = 0;
    bgpixel = cccolor_pixel(pp->win_bgcolor[pp->active_tab->site->site_id]);
    
    if (pw) {
      int site_num;
      int i;
      
      site_num = id_type_sid(pw->parent->id);
      bgpixel = cccolor_pixel(pp->win_bgcolor[site_num]);
      
      /* if (pw->parent->is_answer_to_me) bgpixel = pp->answer_my_msg_bgpixel; */
      /*      if (pw->parent->is_my_message) bgpixel = pp->my_msg_bgpixel;*/
      
      if (ref_mi) {
       if (ref_num == -1) {
	 if (pw->parent->tstamp == ref_mi->timestamp && ref_in_window
	     && id_type_sid(pw->parent->id) == id_type_sid(ref_mi->id)) {
	   bgpixel = cccolor_pixel(pp->emph_color); opaque_bg = 1;
	 }
       } else {
	 if (id_type_eq(pw->parent->id, ref_mi->id) && ref_in_window) {
	   bgpixel = cccolor_pixel(pp->emph_color); opaque_bg = 1;
	 }
       }
     }
     for (i = 0; i < nb_anti_ref; i++) {
       if (id_type_eq(anti_ref_id[i], pw->parent->id)) {
	 bgpixel = cccolor_pixel(pp->emph_color); opaque_bg = 1;
       }
     }
     if (pw->parent && id_type_eq(pw->parent->id, pp->kbnav_current_id)) {
       bgpixel = cccolor_pixel(pp->sel_bgcolor);
     }
   }
   
   pp_draw_line(dock, pp->lpix, pw, bgpixel, NULL, 
                pp->transparency_mode && !opaque_bg, LINEY0(l));

    XCopyArea(dock->display, pp->lpix, d, dock->NormalGC, 0, 0, pp->zmsg_w, pp->fn_h, pp->zmsg_x1, LINEY0(l));
  }

  if (pw_ref && ref_in_window == 0) {
    int y;
    board_msg_info *mi;
    
    y = 3;
    /* affichage de la reference tout en haut du pinnipede */

    /* 
       on boucle pour les situation ou il y a plusieurs messages qui ont le meme timestamp 
    */
    mi = ref_mi;
    while (mi && mi->timestamp == ref_mi->timestamp
	   && id_type_sid(mi->id) == id_type_sid(ref_mi->id)) {
      PostVisual *pv;
      if (mi->in_boitakon == 0 || pp->disable_plopify) {
	pv = pp_pv_add(pp, boards, mi->id);
	if (pv) {
	  PostWord *pw = pv->first;
	  while (pw) {
	    pw = pp_draw_line(dock, pp->lpix, pw, cccolor_pixel(pp->emph_color), NULL, 0, y); 
	    XCopyArea(dock->display, pp->lpix, d, dock->NormalGC, 0, 0, pp->zmsg_w, pp->fn_h, pp->zmsg_x1, y);
	    y += pp->fn_h;
	  }
	}
      }
      get_next_id_filtered(boards, mi->id, &mi, NULL);
      if (ref_num != -1) break;
    }

    /* affichage du commentaire (optionnel) */
    if (ref_mi || strlen(ref_comment)) {
      if (strlen(ref_comment) && (ref_mi==NULL || (ref_mi->in_boitakon && pp->disable_plopify == 0))) {
	XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->emph_color)); //WhitePixel(dock->display, dock->screennum));
	XFillRectangle(dock->display, pp->lpix, dock->NormalGC, 0, 0, pp->win_width, pp->fn_h);
	XSetBackground(dock->display, dock->NormalGC, cccolor_pixel(pp->emph_color)); //WhitePixel(dock->display, dock->screennum));
	//XSetFont(dock->display, dock->NormalGC, pp->fn_it->fid);
        ccfont_draw_string_utf8(pp->fn_it, pp->timestamp_color[pp->active_tab->site->site_id], 
                            pp->lpix, 5, ccfont_ascent(pp->fn_it), ref_comment, strlen(ref_comment));
	//XDrawString(dock->display, pp->lpix, dock->NormalGC, 5, pp->fn_base->ascent, ref_comment, strlen(ref_comment));
	XCopyArea(dock->display, pp->lpix, d, dock->NormalGC, 0, 0, pp->zmsg_w, pp->fn_h, pp->zmsg_x1, y);
	y += pp->fn_h;
      }
      XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->emph_color)); //WhitePixel(dock->display, dock->screennum));
      XFillRectangle(dock->display, pp->win, dock->NormalGC, pp->zmsg_x1, 0, pp->win_width, 3);
      XFillRectangle(dock->display, pp->win, dock->NormalGC, pp->zmsg_x1, y, pp->win_width, 3);
      y+=3;
      XSetForeground(dock->display, dock->NormalGC, BlackPixel(dock->display, dock->screennum));
      XDrawLine(dock->display, pp->win, dock->NormalGC, pp->zmsg_x1, y, pp->win_width, y);
    }
  }

  /* toutes les boards affichées à l'ecran sont marquées comme completement lues */
  /*{
    int sid = -1;
    for (l=0; l < pp->nb_lignes; l++) {
      PostWord *pw = pp->lignes[l];
      if (pw && pw->parent) {
        if (id_type_sid(pw->parent->id) != sid) {
          sid = id_type_sid(pw->parent->id);
          if (sid >= 0 && boards->btab[sid]) board_set_viewed(boards->btab[sid]);
        }
      }
    }
  }
  */
  pp_widgets_refresh(dock);
}

#define MAXAGE 30
/*
void
pp_hilight_newest_messages(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  
  int l = 0;




  if (pp->lignes == NULL) return;
  while (l < pp->nb_lignes) {
    PostVisual *pv;
    if (pp->lignes[l] == NULL) { l++; continue; }
    pv = pp->lignes[l]->parent;
    if (pv->new_decnt) {
      int rw,rh,rx0,ry0;
      int spot;
      pv->new_decnt--;
      rx0 = 2; ry0 = LINEY0(l-pp->lignes[l]->ligne) - pp->fn_h;
      rw = pp->win_width - 2*rx0 - (pp->sc ? SC_W : 0);
      rh = pv->nblig * pp->fn_h;

      //myprintf("id=%d %<yel update decnt %d>\n", id_type_lid(pv->id), pv->new_decnt);


      for (spot = 0; spot < 10; spot++) {
	int pos[2], x[3], y[3];
	int side[2];
	int halfperim, j;
//	
//
//          _______0_______
//         |               | 
//       3 |_______________|1
//                 2


 	unsigned long pix;
	halfperim = (rw-2+rh);
	pos[0] = ((spot * 2* halfperim)/10 + 100000 - pv->new_decnt);
	pos[1] = pos[0] + 200;

	for (j=0; j < 2; j++) {
	  pos[j] %= 2*halfperim;
	  side[j] = ((pos[j] / (halfperim)) % 2)*2 + ((pos[j] % (halfperim)) < rw-1 ? 0 : 1);
	  switch (side[j]) {
	  case 0: x[j] = rx0 + pos[j]; y[j] = ry0; break;
	  case 1: x[j] = rx0 + rw-1; y[j] = ry0+(pos[j]-rw+1); break;
	  case 2: x[j] = rx0 + rw-1 - (pos[j]-halfperim); y[j] = ry0+rh-1; break;
	  case 3: x[j] = rx0; y[j] = ry0+rh - (pos[j]-halfperim-rw)-2; break;
	  }
	}
	
	j = 0;

	pix = RGB2PIXEL(0xff,0,0);
	XSetForeground(dock->display, dock->NormalGC, pp_get_win_bgcolor(dock));
	XDrawPoint(dock->display, pp->win, dock->NormalGC, x[0], y[0]);
	XDrawPoint(dock->display, pp->win, dock->NormalGC, x[0], y[0]+1);
	XSetForeground(dock->display, dock->NormalGC, pix);
	XDrawPoint(dock->display, pp->win, dock->NormalGC, x[1], y[1]);
	XDrawPoint(dock->display, pp->win, dock->NormalGC, x[1], y[1]+1);
      }
    }

    do {
      l++;
    } while (l < pp->nb_lignes && (pp->lignes[l]==NULL || pv == pp->lignes[l]->parent));
  }
}
*/


/* appelée depuis wmcoincoin.c, pour gèrer l'autoscroll et rafraichir l'affichage */
void
pp_check_board_updated(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;

  if (pp && pp->mapped && flag_updating_board == 0) {
    /* test si on scrolle qutomatiquement pour afficher le nouveau message */
    //    if (trib->last_post_id != pp->last_post_id && pp->last_post_id == pp->id_base && pp->decal_base == 0) {
    if (pp->flag_board_updated) {
      id_type last_id;

      /* eh oui, il faut pas autoscroller ou rafraichir alors qu'une selection est active 
	 (le update_content deselectionne automatiquement, mais faut être sûr que
	 l'utilisateur n'est pas justement *en train* de selectionner)
       */
      if (pp->lignes_sel) {
	if (difftime(time(NULL),pp->time_sel) > 20.) { /* on accorde 20 sec. de delai */
	  pp_selection_unselect(pp);
	} else {
	  return;
	}
      }

      pp_scrollcoin_update_bounds(dock); 

      last_id = get_last_id_filtered(boards, &pp->filter);
      if ((!id_type_eq(last_id, pp->last_post_id)) && 
	  pp->colle_en_bas) { // && pp->decal_base == 0) {
	//	myprintf("pp_check_board_updated, on %<yel colle> de %d à %d\n", pp->last_post_id, trib->last_post_id);
	
	pp_pv_destroy(pp); /* force le rafraichissement complet */
	pp_update_content(dock, last_id, 0, 0, 0);
      } else {
	/*	if (trib->last_post_id != pp->last_post_id)
		printf("pp_check_board_updated, on laisse filer de %d à %d (pos=%d/%d)\n", pp->last_post_id, trib->last_post_id, pp->id_base, pp->decal_base);*/
	pp_pv_destroy(pp); /* force le rafraichissement complet */
	pp_update_content(dock, pp->id_base, pp->decal_base, 0, 0);
      }
      pp_refresh(dock, pp->win, NULL);
      pp->flag_board_updated = 0;
    }
  }
}

void
pp_animate(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp && pp->mapped /*&& flag_updating_board == 0*/) {
    /* pour affichage du temps restant avant refresh */
    if (pp->use_minibar) {
      pp_minib_refresh(dock);
      pp_tabs_refresh(dock);
    }
    if (pp->flag_pp_update_request && !flag_updating_board) {
      pp->flag_pp_update_request = 0;
      pp_pv_destroy(pp);
      pp_update_content(dock, get_last_id_filtered(dock->sites->boards, &pp->filter), 100,0,1);
      pp_refresh(dock, pp->win, NULL);   
    } else if (pp->flag_pp_refresh_request) {
      pp_refresh(dock, pp->win, NULL);
    }
  }
}

void pp_update_and_redraw(Dock *dock, id_type id_base, int decal, int adjust, int update_scrollbar_bounds) 
{
  Pinnipede *pp = dock->pinnipede;
  if (!flag_updating_board) {
    pp_pv_destroy(pp);
    pp_update_content(dock, id_base, decal, adjust, update_scrollbar_bounds);
    pp_refresh(dock, pp->win, NULL);
    pp->flag_pp_update_request = 0; /* a tout hasard */
  }
  else pp->flag_pp_update_request = 1; 
}

void
pp_update_bg_pixmap(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  assert(pp->win != None);

  if (pp->lpix != None) {
    XFreePixmap(dock->display, pp->lpix); pp->lpix = None;
  }
  
  pp->lpix_h0 = MAX(MAX(MINIB_H, pp->fn_h), PPT_MAX_H);

  if (pp->transparency_mode) {
    int xpos, ypos;
    xpos = pp->win_real_xpos; ypos = pp->win_real_ypos;
    //    get_window_pos_with_decor(dock->display, pp->win, &xpos, &ypos);
    //printf("window pos without: %d, %d (pp_xpos=%d, pp_ypos=%d)\n", xpos, ypos, pp->win_real_xpos, pp->win_real_ypos);
    pp->lpix = XCreatePixmap(dock->display, pp->win, pp->win_width, pp->win_height+pp->lpix_h0, DefaultDepth(dock->display,dock->screennum));
    if (pp->lpix == None || extract_root_pixmap_and_shade(dock->rgba_context,
                                                          xpos, ypos, 
                                                          pp->win_width, pp->win_height,
                                                          &Prefs.pp_transparency, 
                                                          Prefs.use_fake_real_transparency, pp->lpix, 0, pp->lpix_h0) != 0) {
      myprintf(_("%<yel impossible to use the pseudo-transparency> (probable solution: relaunch wmsetbg or its equivalent)\n"));
      XFreePixmap(dock->display, pp->lpix); pp->lpix = None;
      pp_change_transparency_mode(dock, 0);
    }
  }
  if (pp->lpix == None) {
    pp_change_transparency_mode(dock, 0);
    pp->lpix = XCreatePixmap(dock->display, pp->win, pp->win_width, pp->lpix_h0, DefaultDepth(dock->display,dock->screennum));
  }
}

static int
pp_load_fonts(Pinnipede *pp, char *fn_family, int fn_size)
{
  char base_name[512];
  char ital_name[512];
  char bold_name[512];
  char itbd_name[512];
  char mono_name[512];
#if 0

  /* police de base ... si on ne la trouve pas, c'est une erreur fatale */
  pp->font_base = ccfont_get("times-12");

  snprintf(base_name, 512, "-*-%s-medium-r-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, Prefs.font_encoding);
  pp->fn_base = XLoadQueryFont(display, base_name);
  if (!pp->fn_base) {
    fprintf(stderr, _("XLoadQueryFont: failed loading font '%s'\n"), base_name);
    fprintf(stderr, _("Please choose another font.\n"));
    return -1;
  }

  /* police italique -> on cherche d'abord la police oblique */
  snprintf(ital_name, 512, "-*-%s-medium-o-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, Prefs.font_encoding);
  pp->fn_it = XLoadQueryFont(display, ital_name);
  if (!pp->fn_it) {
    /* puis la police italique */
    BLAHBLAH(1, fprintf(stderr, _("Slanted font '%s' not found -> we're looking for the italic font\n"), ital_name));
    snprintf(ital_name, 512, "-*-%s-medium-i-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, Prefs.font_encoding);
    pp->fn_it = XLoadQueryFont(display, ital_name);
    if (!pp->fn_it) {
      myfprintf(stderr, _("%<RED WARNING>: error while looking for the italic font: '%s'\n"), ital_name);
      myfprintf(stderr, _("We'll use the base font.\n"));

      /* pas de copie de pointer pour pas poser de pbs dans picohtml_destroy */
      pp->fn_it = XLoadQueryFont(display, base_name); assert(pp->fn_it);
    }
  }

  /* police bold */
  snprintf(bold_name, 512, "-*-%s-bold-r-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, Prefs.font_encoding);
  pp->fn_bd = XLoadQueryFont(display, bold_name);
  if (!pp->fn_bd) {
    myfprintf(stderr, _("%<RED WARNING>: error while looking for the bold font: '%s'\n"), bold_name);
    myfprintf(stderr, _("We'll use the base font.\n"));
    pp->fn_bd = XLoadQueryFont(display, base_name); assert(pp->fn_bd);
  }

  /* police bold oblique */
  snprintf(itbd_name, 512, "-*-%s-bold-o-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, Prefs.font_encoding);
  pp->fn_itbd = XLoadQueryFont(display, itbd_name);
  if (!pp->fn_itbd) {
    /* puis la police bold italique */
    BLAHBLAH(1, fprintf(stderr, _("Bold slanted font '%s' not found -> we're looking for the italic font\n"), itbd_name));
    snprintf(itbd_name, 512, "-*-%s-bold-i-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, Prefs.font_encoding);
    pp->fn_itbd = XLoadQueryFont(display, itbd_name);
    if (!pp->fn_itbd) {
      myfprintf(stderr, _("%<RED WARNING>: error while looking for the italic font: '%s'\n"), itbd_name);
      myfprintf(stderr, _("We'll use the base font.\n"));

      /* pas de copie de pointer pour pas poser de pbs dans picohtml_destroy */
      pp->fn_itbd = XLoadQueryFont(display, base_name); assert(pp->fn_itbd);
    }
  }
  pp->fn_h = pp->fn_base->ascent + pp->fn_base->descent+1;


#endif
  if ((pp->fn_minib = ccfont_get("sans:pixelsize=10")) == (CCFontId)(-1)) {
    printf("your fonts suck\n"); exit(1);
  }
  snprintf(base_name, 512, "%s:pixelsize=%d", fn_family, fn_size);
  snprintf(ital_name, 512, "%s:pixelsize=%d:slant=italic,oblique", fn_family, fn_size);
  snprintf(bold_name, 512, "%s:pixelsize=%d:bold", fn_family, fn_size);
  snprintf(itbd_name, 512, "%s:pixelsize=%d:bold:slant=italic,oblique", fn_family, fn_size);
  snprintf(mono_name, 512, "%s:pixelsize=%d:monospace", fn_family, fn_size);
  pp->fn_base = ccfont_get(base_name);
  pp->fn_it = ccfont_get(ital_name);
  pp->fn_bd = ccfont_get(bold_name);
  pp->fn_itbd = ccfont_get(itbd_name);
  pp->fn_mono = ccfont_get(mono_name);
  
  if (pp->fn_base == (CCFontId)(-1)) {
    return -1;
  }
  if (pp->fn_it == (CCFontId)(-1)) pp->fn_it = ccfont_incref(pp->fn_base);
  if (pp->fn_bd == (CCFontId)(-1)) pp->fn_bd = ccfont_incref(pp->fn_base);
  if (pp->fn_itbd == (CCFontId)(-1)) pp->fn_itbd = ccfont_incref(pp->fn_it);
  if (pp->fn_mono == (CCFontId)(-1)) pp->fn_mono = ccfont_incref(pp->fn_base);

  pp->fn_base_space_w = ccfont_text_width_utf8(pp->fn_base, "  ", 2);
  pp->fn_h = ccfont_height(pp->fn_base)+1;
  pp->fn_h = MAX(pp->fn_h, ccfont_height(pp->fn_it));
  pp->fn_h = MAX(pp->fn_h, ccfont_height(pp->fn_bd));
  pp->fn_h = MAX(pp->fn_h, ccfont_height(pp->fn_itbd));
  return 0;
}


static void
pp_free_fonts(Pinnipede *pp)
{
  ccfont_release(&pp->fn_base);
  ccfont_release(&pp->fn_it);
  ccfont_release(&pp->fn_bd);
  ccfont_release(&pp->fn_itbd);
  ccfont_release(&pp->fn_mono);
  ccfont_release(&pp->fn_minib);
  /*
  if (pp->fn_base)
    XFreeFont(display, pp->fn_base); 
  pp->fn_base = NULL;
  if (pp->fn_it)
    XFreeFont(display, pp->fn_it); 
  pp->fn_it = NULL;
  if (pp->fn_bd) 
    XFreeFont(display, pp->fn_bd); 
  pp->fn_bd = NULL;
  if (pp->fn_itbd)
    XFreeFont(display, pp->fn_itbd);
  pp->fn_itbd = NULL;
  */
}


#define GET_BICOLOR(x) (pp->transparency_mode ? x.transp : x.opaque)
enum { INIT_COLORS, RELEASE_COLORS, RESET_COLORS };
#define INIT_OR_RESET_OR_FREE_COLOR(a,b) { if (mode == INIT_COLORS) a = cccolor_get(b); \
  else if (mode == RELEASE_COLORS) cccolor_release(&a); else cccolor_reset(&a,b); }

#define S_INIT_OR_RESET_OR_FREE_COLOR(a,b) { INIT_OR_RESET_OR_FREE_COLOR(a, (Prefs.site[i]) ? (b) : 0x000000) }
static void
pp_set_prefs_colors(Pinnipede *pp, int mode) 
{
  int i;

  INIT_OR_RESET_OR_FREE_COLOR(pp->ccc_black, 0);
  INIT_OR_RESET_OR_FREE_COLOR(pp->popup_fgcolor, GET_BICOLOR(Prefs.pp_popup_fgcolor));
  INIT_OR_RESET_OR_FREE_COLOR(pp->plopify_color, GET_BICOLOR(Prefs.pp_plopify_color));
  INIT_OR_RESET_OR_FREE_COLOR(pp->totoz_unknown_color, 0xa00000);
  INIT_OR_RESET_OR_FREE_COLOR(pp->totoz_found_color, 0x0000a0);
  INIT_OR_RESET_OR_FREE_COLOR(pp->totoz_downloading_color, 0x00a000);
  INIT_OR_RESET_OR_FREE_COLOR(pp->minib_dark_color, GET_BICOLOR(Prefs.pp_buttonbar_fgcolor));

  for (i=0; i <MAX_SITES; i++) {
    //if (Prefs.site[i] == NULL) continue;
    S_INIT_OR_RESET_OR_FREE_COLOR(pp->win_bgcolor[i], Prefs.site[i]->pp_bgcolor);
    //S_INIT_OR_RESET_OR_FREE_COLOR(pp->strike_color[i], GET_BICOLOR(Prefs.site[i]->pp_strike_color));
    S_INIT_OR_RESET_OR_FREE_COLOR(pp->timestamp_color[i], GET_BICOLOR(Prefs.site[i]->pp_tstamp_color));
    S_INIT_OR_RESET_OR_FREE_COLOR(pp->useragent_color[i], GET_BICOLOR(Prefs.site[i]->pp_useragent_color));
    S_INIT_OR_RESET_OR_FREE_COLOR(pp->login_color[i], GET_BICOLOR(Prefs.site[i]->pp_login_color));
    S_INIT_OR_RESET_OR_FREE_COLOR(pp->lnk_color[i], GET_BICOLOR(Prefs.site[i]->pp_url_color));
    S_INIT_OR_RESET_OR_FREE_COLOR(pp->visited_lnk_color[i], GET_BICOLOR(Prefs.site[i]->pp_visited_url_color));
    S_INIT_OR_RESET_OR_FREE_COLOR(pp->txt_color[i], GET_BICOLOR(Prefs.site[i]->pp_fgcolor));
    S_INIT_OR_RESET_OR_FREE_COLOR(pp->trollscore_color[i], GET_BICOLOR(Prefs.site[i]->pp_trollscore_color));
  }
  INIT_OR_RESET_OR_FREE_COLOR(pp->popup_bgcolor, GET_BICOLOR(Prefs.pp_popup_bgcolor));
  INIT_OR_RESET_OR_FREE_COLOR(pp->minib_color, GET_BICOLOR(Prefs.pp_buttonbar_bgcolor));
  INIT_OR_RESET_OR_FREE_COLOR(pp->minib_msgcnt_color, GET_BICOLOR(Prefs.pp_buttonbar_msgcnt_color));
  INIT_OR_RESET_OR_FREE_COLOR(pp->minib_updlcnt_color, GET_BICOLOR(Prefs.pp_buttonbar_updlcnt_color));
  INIT_OR_RESET_OR_FREE_COLOR(pp->progress_bar_color, GET_BICOLOR(Prefs.pp_buttonbar_progressbar_color));
  INIT_OR_RESET_OR_FREE_COLOR(pp->sel_bgcolor, GET_BICOLOR(Prefs.pp_sel_bgcolor));
  INIT_OR_RESET_OR_FREE_COLOR(pp->emph_color, GET_BICOLOR(Prefs.pp_emph_color));
  INIT_OR_RESET_OR_FREE_COLOR(pp->hilight_my_msg_color, GET_BICOLOR(Prefs.pp_my_msg_color));
  INIT_OR_RESET_OR_FREE_COLOR(pp->hilight_answer_my_msg_color, GET_BICOLOR(Prefs.pp_answer_my_msg_color));
  for (i=0; i < NB_PP_KEYWORD_CATEG; i++) {
    INIT_OR_RESET_OR_FREE_COLOR(pp->hilight_keyword_color[i], GET_BICOLOR(Prefs.pp_keyword_color[i]));
  }

  if (mode != RELEASE_COLORS && pp->sc) {
    scrollcoin_change_colors(pp->sc, pp->transparency_mode);
  }
}

void
pp_change_transparency_mode(Dock *dock, int on_off)
{
  Pinnipede *pp = dock->pinnipede;
  pp->transparency_mode = on_off;
  pp_set_prefs_colors(pp, RESET_COLORS);
}


/* initialisation */
void
pp_build(Dock *dock) 
{
  Pinnipede *pp;
  ALLOC_OBJ(pp, Pinnipede);
  dock->pinnipede = pp;

  pp->mapped = 0;

  pp->fn_base = pp->fn_it = pp->fn_itbd = pp->fn_bd = pp->fn_mono = pp->fn_minib = (CCFontId)(-1);
  
  pp_set_prefs_colors(pp, INIT_COLORS);

  pp->lpix_h0 = 0;
  pp_change_transparency_mode(dock, Prefs.pp_start_in_transparency_mode);

  pp->id_base = id_type_invalid_id(); pp->decal_base = 0;
  pp->colle_en_bas = 1;

  pp->flag_pp_update_request = 0;
  pp->flag_pp_refresh_request = 0;

  pp->win_width = Prefs.pp_width;
  pp->win_height = Prefs.pp_height;
  pp->win_decor_xpos = Prefs.pp_xpos;
  pp->win_decor_ypos = Prefs.pp_ypos;
  pp->win_real_xpos = -1;
  pp->win_real_ypos = -1;

  pp->lignes = NULL;
  pp->nb_lignes = 0;
  pp->lignes_sel = NULL;
  pp->sel_l0 = pp->sel_l1 = 0;
  pp->sel_anchor_x = pp->sel_head_x = 0;
  pp->sel_anchor_y = pp->sel_head_y = 0;
  pp->last_selected_text = NULL;

  pp_unset_kbnav(dock);

  pp->sc = NULL;

  //  pp->html_mode = Prefs.pp_html_mode;
  pp->nick_mode = Prefs.pp_nick_mode;
  pp->show_sec_mode = Prefs.pp_show_sec_mode;
  pp->trollscore_mode = Prefs.pp_trollscore_mode;
  pp->disable_plopify = 0;
  pp->hilight_my_message_mode = 1;
  pp->hilight_answer_to_me_mode = 1;
  pp->hilight_key_mode = 1;

  pp->filter.filter_mode = 0;
  pp->filter.filter_name = NULL;
  pp->filter.anything = NULL;
  pp->filter.ua = NULL;
  pp->filter.login = NULL;
  pp->filter.word = NULL;
  pp->filter.nid = 0; pp->filter.id = NULL;
  pp->filter.filter_boitakon = 1;

  pp->pv = NULL;
  pp->survol_hash = 0;

  pp->use_minibar = Prefs.pp_minibar_on;

  //  if (pp->use_minibar)
  //    pp_minib_initialize(pp);

  //pp->fn_minib = dock->fixed_font;

  pp_totoz_build(dock);
  pp_rebuild(dock,1);

  pp->flag_board_updated = 0;

  char s_xpm_bgcolor[30];
  snprintf(s_xpm_bgcolor, 30, "  \tc #%06X", GET_BICOLOR(Prefs.pp_buttonbar_bgcolor));
  miniduck_xpm[1] = s_xpm_bgcolor;
  pp->miniduck_pixmap = RGBACreatePixmapFromXpmData(dock->rgba_context, miniduck_xpm); assert(pp->miniduck_pixmap);
}

void
pp_destroy(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;
  if (pp->mapped) pp_unmap(dock);
  assert(pp->pv == NULL); assert(pp->sc == NULL); 
  assert(pp->lignes_sel == NULL); assert(pp->lignes == NULL);
  pp_free_fonts(pp);
  pp_set_prefs_colors(pp, RELEASE_COLORS);
  pp_tabs_destroy(pp);
  pp_totoz_destroy(dock);
  free(pp); dock->pinnipede = NULL;
}

void
pp_rebuild(Dock *dock, int destroy_tabs)
{
  Pinnipede *pp = dock->pinnipede;
  pp_pv_destroy(pp);
  pp->survol_hash = 0;
  pp_free_fonts(pp);
  if (pp_load_fonts(pp, Prefs.pp_fn_family, Prefs.pp_fn_size)) {
    myprintf(_("Failed to load the '%s' fonts with size '%d'\nLet's try with sans/12.\n"),Prefs.pp_fn_family, Prefs.pp_fn_size);
    if (pp_load_fonts(pp, "sans", 12)==-1) {
      myprintf(_("Uuuurg !! No helvetica, I shoot my nose.\n")); exit(-1);
    }
  }
  pp_set_prefs_colors(pp, RESET_COLORS);
  if (pp_ismapped(dock)) {
    pp_scrollcoin_set(dock,0);
    pp_update_bg_pixmap(dock);
    pp_scrollcoin_set(dock,1);
  }
  if (destroy_tabs) {
    pp_tabs_rebuild(dock);
  }
  pp_widgets_set_pos(dock);  
  if (pp_ismapped(dock)) {
    pp_update_content(dock, pp->colle_en_bas ? 
		      get_last_id_filtered(dock->sites->boards, &pp->filter) : 
		      id_type_invalid_id(), 0, 0, 1);
    pp_refresh(dock, pp->win, NULL);
  }
  pp_totoz_rebuild(dock);
}


/*
  un petit mot: j'ai enfin compris comment faire apparaitre cette fenetre
   ou je veux: il suffit de donner la position dans xcreatewindow
   et d'activer USPosition dans les WM_HINTS (MAIS, les valeurs indiquées dans
   les champs x et y n'ont aucune influence)
*/
void
pp_show(Dock *dock)
{
  Boards *boards = dock->sites->boards;
  XSetWindowAttributes wa;
  Pinnipede *pp = dock->pinnipede;
  int xpos, ypos;
  int wrong_pos = 1, xiscr;
  /*
  {
    board_msg_info *mi = boards->first;
    int cnt = 1;
    while (mi) {
      myprintf("%3d sid=%2d lid=%<YEL %5d> login=%.10s msg=%.60s\n", cnt++,
	       id_type_sid(mi->id), id_type_lid(mi->id), mi->login, mi->msg);
      mi = mi->g_next;
    }
  }
  */

  if (pp->active_tab == NULL) {
    msgbox_show(dock, "looks like you fucked your options file, no board ");
    return;
  }

  if (pp->win_decor_xpos == -10000 && pp->win_decor_ypos == -10000) {
    xpos = dock->xiscreen[0].x_org; ypos = dock->xiscreen[0].y_org; /* ça n'a d'effet que sur certain windowmanagers rustiques (genre pwm) */
  } else {
    xpos = pp->win_decor_xpos;
    ypos = pp->win_decor_ypos;
  }
  /* verifie la visibilité du pinni */
  for (xiscr=0; xiscr < dock->nb_xiscreen; ++xiscr) {
    int x0=dock->xiscreen[xiscr].x_org, y0=dock->xiscreen[xiscr].y_org;
    int x1=x0+dock->xiscreen[xiscr].width,y1=y0+dock->xiscreen[xiscr].height;
    //printf("[%d-%dx%d-%x] %d %d\n", x0,x1,y0,y1);
    if (MIN(xpos + pp->win_width,x1) - MAX(xpos,x0)  > 20 &&
        MIN(ypos + pp->win_height,y1) - MAX(ypos,y0)  > 20)
      wrong_pos = 0;
  }
  if (wrong_pos) {
    pp->win_decor_xpos = pp->win_decor_ypos = -10000;
    xpos = dock->xiscreen[0].x_org; ypos = dock->xiscreen[0].y_org;
  }
  

  if (dock->pp_win) { /* on recycle tjs la meme fenetre ? 
                         inconvenient: (avec wmaker) la fenetre est toujours remappée dans le même workspace :/
                       */
    pp->win = dock->pp_win;
  } else {
    pp->win = XCreateSimpleWindow (dock->display, dock->rootwin, 
                                   xpos, ypos, pp->win_width,pp->win_height, 0,
                                   WhitePixel(dock->display, dock->screennum),
                                   pp_get_win_bgcolor(dock));
    kb_create_input_context_for(dock, pp->win, KB_PINNIPEDE);
  }
  XMoveResizeWindow(dock->display, pp->win, xpos, ypos, pp->win_width, pp->win_height);

  wa.event_mask =
    ButtonPressMask | 
    ButtonReleaseMask | 
    ExposureMask | 
    PointerMotionMask | 
    StructureNotifyMask |
    EnterWindowMask | 
    //    ResizeRedirectMask |
    KeyPressMask | 
    KeyReleaseMask | 
    FocusChangeMask |
    LeaveWindowMask;
    
  /* ça sera a changer .. pour l'instant ça ira */
  //  if (pp->transparency_mode) {
  //    wa.override_redirect = True ;
  //  } else
  wa.override_redirect = False ;
    
  
  //wa.background_pixmap = ParentRelative;
  //wa.override_redirect = False ;
  XChangeWindowAttributes (dock->display, pp->win,
			   //CWBackPixmap | 
			   CWEventMask | CWOverrideRedirect, &wa);

  {
    XWMHints *wm_hint;
      
    set_window_title(dock->display, pp->win, "pinnipede teletype", "pinnipede");
    set_window_sizepos_hints(dock->display, pp->win, xpos, ypos, 
                          200, 300, -1,
                          80 , 455, -1);
    /* au premier lancement, la pos n'est pas connue (sauf si specifee
       dans les options ) */
    /*    if (pp->win_decor_xpos == -10000 && pp->win_decor_ypos == -10000) {
    } else {
      set_window_pos_hints(dock->display, pp->win, xpos, ypos);
      }*/
    set_window_class_hint(dock->display, pp->win, "wmcoincoin", "pinnipede_teletype");
      
    wm_hint = XAllocWMHints(); assert(wm_hint);
    wm_hint->icon_pixmap = dock->wm_icon_pix;
    wm_hint->icon_mask = dock->wm_icon_mask;
    wm_hint->flags = IconPixmapHint | IconMaskHint;
    XSetWMHints(dock->display, pp->win, wm_hint);
    XFree(wm_hint);
  }
    
  /* pour etre informé de la fermeture de la fenetre demandee par le windowmanager */
  {
    Atom p[2]; p[0] = dock->atom_WM_DELETE_WINDOW; p[1] = dock->atom_WM_TAKE_FOCUS;
    /*XSetWMProtocols(dock->display, pp->win, &dock->atom_WM_DELETE_WINDOW, 1);*/
    XSetWMProtocols(dock->display, pp->win, p, 2);
  }
  /*
    {
    XWMHints *mwh = XAllocWMHints();
    mwh->initial_state = WithdrawnState;
    XSetWMHints(dock->display, pp->win, mwh);
    }*/

  pp_update_bg_pixmap(dock);

  XSetWindowBackground(dock->display, pp->win, cccolor_pixel(pp->minib_color));
  XMapRaised(dock->display, pp->win);


  assert(pp->sc == NULL);

  pp_scrollcoin_set(dock,1);

  //  XMoveWindow(dock->display, pp->win, 100, 100);
  pp->mapped = 1;
  pp_update_content(dock, pp->colle_en_bas ? 
		    get_last_id_filtered(boards, &pp->filter) : 
		    pp->id_base, 0, 0, 1);
    
  pp->survol_hash = 0;
  //XReparentWindow(dock->display, DOCK_WIN(dock), pp->win, 5, 5);
}


static board_msg_info*
pp_find_message_at_xy(Dock *dock, int x, int y) {
  Pinnipede *pp = dock->pinnipede;
  board_msg_info *mi = NULL;
  if (y >= pp->zmsg_y1 && y <= pp->zmsg_y2 && 
      x >= pp->zmsg_x1 && x <= pp->zmsg_x2) {
    int l;
    for (l=0; l < pp->nb_lignes; l++) {
      if (y >= LINEY0(l) && y <= LINEY1(l) && pp->lignes[l]) {
	mi = boards_find_id(dock->sites->boards, pp->lignes[l]->parent->id);
	return mi;
      }
    }
  }
  return NULL;
}

static PostWord *
pp_get_pw_at_xy(Pinnipede *pp,int x, int y)
{
  PostWord *pw;
  int l;
  pw = NULL;
  x -= pp->zmsg_x1;
  if (y >= pp->zmsg_y1 && y <= pp->zmsg_y2) {
    int trouve = 0;
    for (l=0; l < pp->nb_lignes; l++) {
      pw = pp->lignes[l];
      
      while (pw && pw->ligne == pp->lignes[l]->ligne) {
	if (x >= pw->xpos && x < pw->xpos+pw->xwidth &&
	    y >= LINEY0(l) && y <= LINEY1(l)) 
	  {
	    trouve = 1;
	    break;
	  }
	pw = pw->next;
      }
      if (trouve) break;
    }
    if (trouve == 0) pw = NULL; /* raa le bug tout naze */
  }
  return pw;
}

/* affiche le texte en haut du pinnipede (dans le style très dépouillé des useragents) */
void
pp_popup_show_txt(Dock *dock, unsigned char *txt)
{
  Pinnipede *pp = dock->pinnipede;
  int l,cnt;
  CCFontId fn;
  char *s;
  int ry0, ry1;
  int fn_h;
  if (txt == NULL) return;
  if (strlen(txt) == 0) return;

  fn = pp->fn_bd;
  fn_h = ccfont_height(pp->fn_bd); // ccfont_ascent(pp->fn_bd) + ccfont_descent(pp->fn_bd) + 1;
  //XSetFont(dock->display, dock->NormalGC, fn->fid);
  l = 0; s = txt;
  while (*s) {
    cnt = 0;
    while (s[cnt] && s[cnt] != '\n') {
      cnt++;
      if (ccfont_text_width_utf8(fn, s, cnt) > pp->zmsg_w-16) {
	cnt--; break;
      }
    }
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(pp->popup_bgcolor));
    ry0 = (l == 0 ? 0 : (l+1)*fn_h); // - ccfont_ascent(fn));
    ry1 = (l+1)*fn_h + fn_h; //ccfont_descent(fn) + (s[cnt]==0 ? 6 : 0);
    XFillRectangle(dock->display, pp->win, dock->NormalGC, pp->zmsg_x1, ry0,
		   pp->zmsg_w, ry1 - ry0+1);
    if (s[cnt]==0) {
      XSetForeground(dock->display, dock->NormalGC, BlackPixel(dock->display, dock->screennum));
      XDrawLine(dock->display, pp->win, dock->NormalGC, pp->zmsg_x1, ry1, pp->zmsg_x1+pp->zmsg_w-1, ry1);
    }
    ccfont_draw_string_utf8(pp->fn_bd, pp->popup_fgcolor, pp->win, pp->zmsg_x1+8, (l+1)*fn_h,
                        s, cnt);
    s += cnt;
    if (*s == '\n') s++;
    if (*s) l++;
  }
}

/* renvoie le nombre de references vers le message base_mi (sauf ipot) */
/* to do: comptage inter-sites */
static void
pp_count_backrefs(Boards *b, board_msg_info *base_mi, int *nrep, int *nrep_bak)
{
  board_msg_info *mi;

  *nrep = *nrep_bak = 0;
  if (base_mi == NULL) return;

  /* on parcourt tous les message postérieurs à base_mi */
  mi = b->first; //base_mi->next;
  while (mi) {
    int i;
    /* on regarde toutes ses references */
    for (i = 0; i < mi->nb_refs; i++) {
      board_msg_info *ref_mi;
      int j;

      /* pour chaque ref, on regarde la liste (generalement de taille 1 ou 0) des messages pointés */
      for (j = 0, ref_mi = mi->refs[i].mi; j < mi->refs[i].nbmi; j++, ref_mi=ref_mi->next) {
	/* on notera qu'on fait bien ref_mi->next et pas ref_mi->g_next */
	assert(ref_mi);
	
	/* si on pointe vers le bon */
	if (ref_mi == base_mi) {
	  /*
	    printf("%s/%5d backref: %s/%5d [cnt=%d]\n", Prefs.site[base_mi->id.sid]->site_name, base_mi->id.lid,
	    Prefs.site[mi->id.sid]->site_name, mi->id.lid,
	    nb_backrefs+1);
	  */
	  (*nrep)++;
          if (mi->in_boitakon) (*nrep_bak)++;
	  break; /* si le message contient deux refs vers base_mi, on ne le compte qu'une fois */
	}
      }
    }
    mi = mi->g_next;
  }
  return;
}




/* celle la est tordue ...
   il s'agit de verifier si on survole (avec la souris) une info interessante, 
   et d'agir le cas echeant (de maniere un peu désordonnée)

   force_refresh : -1 => on n'appelle jamais pp_refresh
                    0 => on appelle si il faut
                   +1 => on appelle toujours
*/
void
pp_check_survol(Dock *dock, PostWord *pw, int force_refresh)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  char survol[1024];
  char *p;
  intptr_t survol_hash;
  int is_a_ref = 0;

  survol[0] = 0;
  if (pw) {
    if (pw->attr_s && (pw->attr & PWATTR_REF)==0) { /* pour les [url] */
      strncpy(survol, pw->attr_s, 1024); survol[1023] = 0;
      RSSBonusInfo *ri;
      if ((ri=rss_find_from_link(boards, survol))) {
        if (!id_type_eq(ri->id,pw->parent->id)) {
          survol[0] = 0; is_a_ref = 1;
        }
      }
    } else if (pw->attr & PWATTR_TSTAMP) {
      board_msg_info *mi;
      char blah[1024], snrep[1024];
      char *s;
      int blah_sz = 1024;

      int nrep, nrep_bak;
      KeyList *hk;
      mi = boards_find_id(boards, pw->parent->id);

      s = blah; s[0] = 0;
      if (mi->is_my_message) {
	snprintf(s, blah_sz, _("\n[you posted this message]")); blah_sz -= strlen(s); s += strlen(s);
      }
      if (mi->is_answer_to_me && blah_sz>30) {
	snprintf(s, blah_sz, _("\n[this message answers to one of yours]")); blah_sz -= strlen(s); s += strlen(s);
      }
      hk = board_key_list_test_mi(boards, mi, Prefs.hilight_key_list);
      if (hk && blah_sz > 60) {
	snprintf(s, blah_sz, _("\nmessage 'boxed' because: ")); blah_sz -= strlen(s); s += strlen(s);
	while (hk && blah_sz > 30) {
	  snprintf(s, blah_sz, " {%s='%.20s'}", 
		   key_list_type_name(hk->type), hk->key); blah_sz -= strlen(s); s += strlen(s);
	  hk = board_key_list_test_mi(boards, mi, hk->next);
	}
      }
      hk = board_key_list_test_mi(boards, mi, Prefs.plopify_key_list);
      if (hk && blah_sz > 60) {
	snprintf(s, blah_sz, _("\nmessage plopified (level %d) because: "), hk->num); blah_sz -= strlen(s); s += strlen(s);
	while (hk && blah_sz > 30) {
	  snprintf(s, blah_sz, " {%s='%.20s'}", 
		   key_list_type_name(hk->type), hk->key); blah_sz -= strlen(s); s += strlen(s);
	  hk = board_key_list_test_mi(boards, mi, hk->next);
	}
      } else if (mi->contagious_boitakon) {
	snprintf(s, blah_sz, _("\nmessage plopified (level 3) because the boitakon is hungry"));
	blah_sz -= strlen(s); s += strlen(s);
      }
      pp_count_backrefs(boards, mi, &nrep, &nrep_bak);

      if (nrep_bak == 0) {
        snprintf(snrep, 1024, "%d %s",nrep, (nrep > 1) ? _("answers") : _("answer"));
      } else {
        snprintf(snrep, 1024, _("%d %s (and %d plop%s from the boitakon)"),nrep-nrep_bak, (nrep-nrep_bak > 1) ? _("answers") : _("answer"), nrep_bak, nrep_bak > 1 ? "s" : "");
      }
      char *stime = strdup(asctime(localtime(&mi->timestamp)));
      while (stime && stime[0] && stime[strlen(stime)-1] == '\n') stime[strlen(stime)-1] = 0;
      snprintf(survol, 1024, "[%s] id=%d ua=%s\n%s%s\ntimestamp=(%s)", 
	       Prefs.site[pw->parent->id.sid]->site_name,
	       pw->parent->id.lid, 
	       (mi ? mi->useragent : ""), 
	       snrep, blah, stime);
      free(stime);
      is_a_ref = 1;
    } else if (pw->attr & PWATTR_REF) {
      is_a_ref = 1;
    } else if (pw->attr & PWATTR_REFID) {
      is_a_ref = 1;
    }
  }
  
  survol_hash = 0;
  if (is_a_ref == 0) {
    p = survol;
    while (*p) { survol_hash += (((unsigned)*p) * 253) + 23; p++; }
  } else {
    survol_hash = (intptr_t)pw; // pourquoi pas...
  }
  
  //  survol_hash = (int)pw; // ca c'est pas bon, on fait trop de refresh

  /* on evite de reafficher tant qu'on survolle le meme objet (pour eviter le clignotement) */
  if (pp->survol_hash != survol_hash) {
    if (force_refresh != -1 && (is_a_ref || strlen(survol) == 0 || force_refresh == +1)) {
      pp_refresh(dock, pp->win, is_a_ref ? pw : NULL);
    }
    pp_popup_show_txt(dock, survol);
    pp->survol_hash = survol_hash;
  }
}

void
pp_unmap(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;

  pp_selection_unselect(pp);
  pp_totoz_unmap(dock);

  /* le pp_refresh a juste pour but de 'delocker' le PostVisual sauvé dans le cache
     oui c'est de la bidouille qui sent les remugles nauséabonds */

  /*  d'ailleurs je le vire, c'est incompatible avec l'update des prefs */
  //  pp_refresh(dock, pp->win, NULL);

  /* on sauve la position de la fenetre (en prenant en compte les decorations du WM ) */
  get_window_pos_with_decor(dock->display, pp->win, &pp->win_decor_xpos, &pp->win_decor_ypos);

  if (pp->lpix != None) XFreePixmap(dock->display, pp->lpix); 
  pp->lpix = None;

  if (dock->pp_win) {
    XUnmapWindow(dock->display, pp->win);
  } 
  XDestroyWindow(dock->display, pp->win);
  kb_release_input_context(KB_PINNIPEDE);
  pp->win = None;

  pp->mapped = 0;

  pp_scrollcoin_set(dock,0);
  if (pp->last_selected_text) { free(pp->last_selected_text); pp->last_selected_text = NULL; }

  pp_pv_destroy(pp);
}

int pp_ismapped(Dock *dock) {
  return dock->pinnipede->mapped;
}


void
pp_balloon_help(Dock *dock, int x, int y)
{
  Pinnipede *pp = dock->pinnipede;

  //  balloon_test(dock, x, y, pp->win_xpos, pp->win_ypos, 15000,
  //	       0, 0, pp->win_width, pp->win_height,
  balloon_show(dock, pp->win_real_xpos + x, pp->win_real_ypos + y, 40, 40, 
	       _("<p align=center> Welcome to the <b><font color=#008000>Pinnipede Teletype</font></b></p>"
		 "This window was specially designed by the greatest experts to offer you "
		 "optimal mouling conditions.<br>"
		 "Here is a summary of its functionalities:<br><br>"
		 "<b>To scroll</b>, use the mouse wheel, or 'drag' "
		 "while clicking with the middle button. "
		 "Scrolling is automatic when a new message appears.<br><br>"
		 "To bring the <b>button bar</b> or make it disappear, use the middle clic. "
		 "<br><br>"
		 "The available actions on the <b>clock</b> near each message are:<br>"
		 "<font color=blue>Left Click</font><tab>: opens the palmipede editor, and inserts a reference to the message<br>"
		 "If you click on an <b>[url]</b>, the result will be:<br>"
		 "<font color=blue>Left Click</font><tab>: opens the url in the external browser (if it has been "
		 "defined in the ~/.wmcoincoin/options file). <b>Warning</b>, even if precautions have been taken"
		 "since the inominious wmcoincoin 2.0, this kind of things is generally considered as a security weakness...<br>"
		 "<font color=blue>Middle Click</font><tab>: opens the url with the second browser (the http.browser2 option)<br>"
		 "When the pointer is over a <b>reference to a previous post</b>, it will be underlined. If you click:<br>"
		 "<font color=blue>Left Click</font><tab>: brings the referenced message<br>"
		 "You can use the right clic everywhere to bring a popup menu, which gives your more choices, such as "
		 "putting a message in the boitakon, plopifying, filtering, copying in clipboard etc.<br>"
		 "Some basic filtering is available with the help of <font color=blue>Right Click</font> "
		 "on a word, login, useragent... Use the blue button to cancel the filter.<br>"
		 "If you want to emphasize the messages of a given user, or those containing a given word, use the "
		 " emphasize entry of the contextual menu.<br>"
		 "Someone is annoying you ? You have launched a troll that you don't control ? Then <b>plopify</b> the "
		 "evil with a <font color=blue>Right Click/Plopify</font> (or put in boitakon) on his login/useragent. "
		 "You can later remove the boitakonned message from the boitakon with a right clic on the left-most gray "
		 "button.<br><br>"
		 "You can take a 'shot' of the board (the so-called seafood tray), with <font color=blue>Ctrl+Middle Click</font><br><br>"
		 "In order to understand the display of the <b>useragents</b> activated by the dark red button "
		 "(about fifteen pixels on your left), you can see "
		 "the <tt>~/.wmcoincoin/options</tt><br> file (hint: the button has 5 different positions)<br><br>"
		 "The pinnipede teletype wishes you a nice mouling."), 500);
}


#define THREAD_FILTER_SZ_REALLOC 10

/* pas glop */
static void
pp_thread_filter_add_refs(Boards *boards, struct _PinnipedeFilter *f, board_msg_info *base_mi)
{
  int i,j;
  if (base_mi == NULL) return;

  /* on inspecte toutes les references */
  for (i = 0; i < base_mi->nb_refs; i++) {
    board_msg_info *mi;
    
    mi = base_mi->refs[i].mi;

    for (j = 0; j < base_mi->refs[i].nbmi; j++) {
      /* realloc la liste si il faut */
      if ((f->nid)%THREAD_FILTER_SZ_REALLOC == 0) {
	f->id = (id_type*)realloc(f->id, (f->nid+THREAD_FILTER_SZ_REALLOC) * sizeof(id_type)); assert(f->id);
      }
      /* si la ref n'etait pas déjà dans la liste, on l'ajoute */
      if (pp_thread_filter_find_id(f, mi->id)==0) {
	//myprintf("    ref(%d): ajout de %<YEL %d> <-- %<MAG %d>\n", f->nid, mi->id, base_mi->id);
	f->id[f->nid++] = mi->id;
	/* et on recurse ... */
	pp_thread_filter_add_refs(boards, f, mi);
      }
      mi = mi->next; /* et pas g_next, car multi-ref sur un même site  */
    }
  }
}

/* rolala y'a des features qui font mal à la tête */
static void
pp_thread_filter_add_backrefs(Boards *boards, struct _PinnipedeFilter *f, board_msg_info *base_mi)
{
  board_msg_info *mi;
  
  if (base_mi == NULL) return;

  /* on parcourt tous les message postérieurs à base_mi */
  mi = base_mi->g_next;
  while (mi) {
    int i;
    /* on regarde toutes ses references */
    for (i = 0; i < mi->nb_refs; i++) {
      board_msg_info *ref_mi;
      int j;

      /* pour chaque ref, on regarde la liste (generalement de taille 1 ou 0) des messages pointés */
      for (j = 0, ref_mi = mi->refs[i].mi; j < mi->refs[i].nbmi; j++, ref_mi=ref_mi->next) {
	assert(ref_mi);

	/* si on pointe vers le bon */
	if (ref_mi == base_mi) {
	  /* realloc de la liste si necessaire */
	  if ((f->nid)%THREAD_FILTER_SZ_REALLOC == 0) {
	    f->id = (id_type*)realloc(f->id, (f->nid+THREAD_FILTER_SZ_REALLOC)*sizeof(id_type)); assert(f->id);
	  }
	  
	  /* si le message n'a pas encore ete traite on l'ajoute */
	  if (pp_thread_filter_find_id(f, mi->id)==0) {
	    //myprintf("backref(%d): ajout de %<YEL %d> --> %<MAG %d>\n", f->nid, mi->id, base_mi->id);
	    
	    f->id[f->nid++] = mi->id;
	    /* et hop ça recurse un coup */
	    pp_thread_filter_add_backrefs(boards, f, mi);
	  }
	}
      }
    }
    mi = mi->g_next;
  }
}

void
pp_set_thread_filter(Dock *dock, id_type base_id)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;

  char fname[200];
  board_msg_info *mi;

  mi = boards_find_id(boards, base_id);
  if (mi == NULL) return;

  if (mi->hmsf[3] == 0) {
    snprintf(fname, 200, "thread: %02d:%02d", mi->hmsf[0], mi->hmsf[1]);
  } else {
    snprintf(fname, 200, "thread: %02d:%02d:%02d", mi->hmsf[0], mi->hmsf[1], mi->hmsf[2]);
  }

  pp_unset_filter(&pp->filter);
  pp->filter.filter_mode = 1;
  pp->filter.filter_name = strdup(fname);

  pp->filter.nid = 1;
  pp->filter.id = (id_type*)calloc(THREAD_FILTER_SZ_REALLOC, sizeof(id_type)); assert(pp->filter.id);
  pp->filter.id[0] = base_id;


  pp_thread_filter_add_refs(boards, &pp->filter, mi);
  pp_thread_filter_add_backrefs(boards, &pp->filter, mi);


  BLAHBLAH(2,printf(_("Activating the filter [%s]\n"), pp->filter.filter_name));
  pp_update_content(dock, id_type_invalid_id(), 0, 0, 1);
  pp_refresh(dock, pp->win, NULL);
}


void
pp_set_login_filter(Dock *dock, char *login)
{
  Pinnipede *pp = dock->pinnipede;

  char fname[200];

  snprintf(fname, 200, "login: <%s>", login);
  pp_unset_filter(&pp->filter);
  pp->filter.filter_mode = 1;
  pp->filter.filter_name = strdup(fname);
  pp->filter.login = strdup(login);

  BLAHBLAH(2,printf(_("Activating the filter [%s]\n"), pp->filter.filter_name));
  pp_update_content(dock, id_type_invalid_id(), 0, 0, 1);
  pp_refresh(dock, pp->win, NULL);	  
}

void
pp_set_ua_filter(Dock *dock, char *ua)
{
  Pinnipede *pp = dock->pinnipede;
  char fname[200];

  snprintf(fname, 200, "ua: [%.20s]", ua);
  pp_unset_filter(&pp->filter);
  pp->filter.filter_mode = 1;
  pp->filter.filter_name = strdup(fname);
  pp->filter.ua = strdup(ua);

  BLAHBLAH(2,printf(_("Activating the filter [%s]\n"), pp->filter.filter_name));
  pp_update_content(dock, id_type_invalid_id(), 0, 0, 1);
  pp_refresh(dock, pp->win, NULL);
}

void
pp_set_word_filter(Dock *dock, char *word)
{
  Pinnipede *pp = dock->pinnipede;
  char fname[200];

  snprintf(fname, 200, "mot: '%s'", word);
  pp_unset_filter(&pp->filter);
  pp->filter.filter_mode = 1;
  pp->filter.filter_name = strdup(fname);
  pp->filter.word = strdup(word);

  BLAHBLAH(2,printf(_("Activating the filter [%s]\n"), pp->filter.filter_name));
  pp_update_content(dock, id_type_invalid_id(), 0, 0, 1);
  pp_refresh(dock, pp->win, NULL);	  
}

void
pp_set_anything_filter(Dock *dock, char *word)
{
  Pinnipede *pp = dock->pinnipede;
  char fname[200];
  char *s = strdup(word); /* pour les cas où word == filter.anything et unset_filter va le free-er */
  snprintf(fname, 200, "isearch: '%s'", word);
  pp_unset_filter(&pp->filter);
  pp->filter.filter_mode = 1;
  pp->filter.filter_name = strdup(fname);
  pp->filter.anything = s;
  BLAHBLAH(2,printf(_("Activating the filter [%s]\n"), pp->filter.filter_name));
  pp_update_content(dock, id_type_invalid_id(), 0, 0, 1);
  pp_refresh(dock, pp->win, NULL);	  
}

void pp_change_filter_mode(Dock *dock, int zero_or_one) {
  Pinnipede *pp = dock->pinnipede;
  pp->filter.filter_mode = zero_or_one;
  /* reset du scroll (necessaire, faut etre que le post 'id_base' 
     soit bien affiché par le filtre) */
  /*if (pp->filter.filter_mode) pp->id_base = id_type_invalid_id(); 
    pp_update_and_redraw(dock, pp->id_base, pp->decal_base,0,1);*/
  pp_update_and_redraw(dock, get_last_id_filtered(dock->sites->boards, &pp->filter), 0,0,1);
}

int pp_get_filter_mode(Dock *dock) {
  return dock->pinnipede->filter.filter_mode;
}

static void
gogole_search(Dock *dock, int mx, int my, char *w, int quote_all, int browser_num)
{
  Pinnipede *pp = dock->pinnipede;

  if (Prefs.gogole_search_url == NULL) return;
  if (w) {
    char *s, *s0;
    char *ww, *ww0;
    ww0 = str_preencode_for_http(w);
    if (quote_all) {
      ww = str_printf("%%22%s%%22",ww0); free(ww0);
    } else ww = ww0;
    if (strlen(ww)>512) ww[512] = 0; /* faut pas pousser grand mère */
    s0 = str_substitute(Prefs.gogole_search_url, "%22%s%22", ww);
    s = str_substitute(s0, "%s", ww);
    free(s0);
    open_url(s, pp->win_real_xpos + mx-5, pp->win_real_ypos+my-10, browser_num);
    free(s); free(ww); 
  }
}


static void
wikipedia_search(Dock *dock, int mx, int my, char *w, int quote_all, int browser_num)
{
  Pinnipede *pp = dock->pinnipede;

  if (Prefs.wikipedia_search_url == NULL) return;
  if (w) {
    char *s, *s0;
    char *ww, *ww0;
    ww0 = str_preencode_for_http(w);
    if (quote_all) {
      ww = str_printf("%%22%s%%22",ww0); free(ww0);
    } else ww = ww0;
    if (strlen(ww)>512) ww[512] = 0; /* faut pas pousser grand mère */
    s0 = str_substitute(Prefs.wikipedia_search_url, "%22%s%22", ww);
    s = str_substitute(s0, "%s", ww);
    free(s0);
    open_url(s, pp->win_real_xpos + mx-5, pp->win_real_ypos+my-10, browser_num);
    free(s); free(ww); 
  }
}


static char *str_simplif(char *s)
{
  char *p, *p2;
  
  /* simplification du mot */
  p = s + strlen(s) -1;
  while (p > s && !isalnum((unsigned char)*p)) p--;
  //  if (p > s) *(p+1) = 0;
  p2 = s;
  while (p2 < p && !isalnum((unsigned char)*p2)) p2++;
  if (p2 != p) {
    *(p+1)=0;
    return p2;
  }
  else return s;
}

void
pp_handle_button3_press(Dock *dock, XButtonEvent *event) {
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  char *descr = NULL;
  char *txt = NULL;
  PostWord *pw = NULL;
  int mx, my, choice, cnt, redraw;
  board_msg_info *mi = NULL;
  KeyList *hk_plop = NULL, *hk_emph = NULL, *hk;
  enum { WORD, UA_WITH_LOGIN, UA_NO_LOGIN, LOGIN, TSTAMP, THREAD, NOTHING } what_clicked;
  int hk_what_clicked = -1, plop_lvl, emph_lvl;
  enum { PUP_PLOPIF, PUP_SUPERPLOPIF, PUP_BOITAKON, PUP_HUNGRY_BOITAKON, 
	 PUP_FILTER, PUP_GOGOLE, PUP_WIKIPEDIA, PUP_COPY_URL, PUP_COPY_UA, 
         PUP_DO_TOTOZ, PUP_DO_TOTOZ_BOOKMARK, PUP_DO_TOTOZ_UNBOOKMARK, 
	 PUP_EMPH0, PUP_EMPH1, PUP_EMPH2, PUP_EMPH3, PUP_EMPH4, PUP_TOGGLE_MINIB, PUP_SITE_CONFIG, PUP_TOGGLE_BIGORNO1, PUP_TOGGLE_BIGORNO2, 
	 PUP_UNEMPH=10000, PUP_UNPLOP=20000
	 };
  char s_thread[20]; /* 'txt' peut pointer dessus */
  char s_word[512];
  char s_id[30];
  plopup_unmap(dock);
  mx = event->x; my = event->y;
  what_clicked = NOTHING;
  hk_what_clicked = -1;
  if (pp->last_selected_text && strlen(pp->last_selected_text) < 512) {
    snprintf(s_word, 512, "%s", pp->last_selected_text);
    txt = s_word;
    if (strlen(txt)) {
      descr = str_printf("The selected <font color=blue>text</font> is '<font color=#800000>%s</font>'",
			 txt);
      what_clicked = WORD;
      hk_what_clicked = HK_WORD;
    }
  } else {
    pw = pp_get_pw_at_xy(pp, event->x, event->y);
    if (pw && (pw->attr & PWATTR_TROLLSCORE)) {
      what_clicked = THREAD;
    } else if (mx < 20) { /* si on clique à gauche de l'horloge, on considère une action sur tout le
			     thread (le pb est qu'il n'y a pas forcement de mot à cet endroit) */
      pw = pp_get_pw_at_xy(pp, 30, my); /* le 20 est une ruse de chacal puant */
      if (pw) what_clicked = THREAD;
    }
    if (what_clicked == THREAD) {
      if (mi) {
	descr = str_printf(_("The selected element is the <font color=blue>thread</font> '<font color=#800000>id=%d</font>'"), id_type_lid(mi->id));
	snprintf(s_thread, 20, "%d", id_type_to_int(mi->id));
	txt = s_thread;
	hk_what_clicked = HK_THREAD;
      } else {
	what_clicked = NOTHING;
      }
    }
    if (pw) {
      mi = boards_find_id(boards, pw->parent->id);
    } else mi = NULL;
    if (pw && descr == NULL) {
      if (pw->attr & PWATTR_NICK) {
	descr = str_printf(_("The selected element is the <font color=blue>useragent</font> '<font color=#800000>%s</font>'"), pw->w);
	txt = mi->useragent;
	if (mi->login[0]) {
	  what_clicked = UA_WITH_LOGIN;
	  hk_what_clicked = HK_UA;
	} else {
	  what_clicked = UA_NO_LOGIN;
	  hk_what_clicked = HK_UA_NOLOGIN;
	}
      } else if (pw->attr & PWATTR_LOGIN) {
	descr = str_printf(_("The selected element is the <font color=blue>login</font> '<font color=#800000>%s</font>'"), pw->w);
	txt = pw->w;
	what_clicked = LOGIN;
	hk_what_clicked = HK_LOGIN;
      } else if (pw->attr & PWATTR_TSTAMP) {
	snprintf(s_id, 30, "%d", id_type_to_int(pw->parent->id));
	descr = str_printf(_("The selected element is the <font color=blue>message</font> #id=<font color=#800000>%d</font>"), id_type_lid(pw->parent->id));
	txt = s_id;
	what_clicked = TSTAMP;
	hk_what_clicked = HK_ID;
      } else if (pw && strlen(pw->w)) {
	snprintf(s_word, 512, "%s", pw->w);
	txt = str_simplif(s_word);
	if (strlen(txt)) {
	  descr = str_printf(_("The selected element is the <font color=blue>word</font> '<font color=#800000>%s</font>'"), txt);
	  what_clicked = WORD;
	  hk_what_clicked = HK_WORD;
	}
      }
    }
  }
  
  if (what_clicked != NOTHING) {
    plopup_set_description(dock, descr);
    plopup_pushentry(dock, _("<b>plop</b>ify it"), PUP_PLOPIF);
    plopup_pushentry(dock, _("<b>superplop</b>ify it"), PUP_SUPERPLOPIF);
    plopup_pushentry(dock, _("put in <b>boitakon</b>"), PUP_BOITAKON);
    plopup_pushentry(dock, _("put in <i>hungry</i> <b>boitakon</b>"), PUP_HUNGRY_BOITAKON);
  }

  if (what_clicked != NOTHING) {
    plopup_pushentry(dock, _("mark it (categ 0)"), PUP_EMPH0);
    plopup_pushentry(dock, _("mark it (categ 1)"), PUP_EMPH1);
    plopup_pushentry(dock, _("mark it (categ 2)"), PUP_EMPH2);
    plopup_pushentry(dock, _("mark it (categ 3)"), PUP_EMPH3);
    plopup_pushentry(dock, _("mark it (categ 4)"), PUP_EMPH4);
    plopup_pushentry(dock, _("filter it"), PUP_FILTER);
  }

  if (what_clicked != NOTHING && strlen(txt) && Prefs.gogole_search_url) {
    plopup_pushentry(dock, _("gogole search"), PUP_GOGOLE);
  }

  if (what_clicked != NOTHING && strlen(txt) && Prefs.wikipedia_search_url) {
    plopup_pushentry(dock, _("wikipedia search"), PUP_WIKIPEDIA);
  }

  if (pw && (pw->attr & PWATTR_LNK)) {
    if (pw->attr_s && strlen(pw->attr_s)) {
      plopup_pushentry(dock, _("copy link in X clipboard"), PUP_COPY_URL);
    }
  }
  if (pw && (pw->attr & (PWATTR_TSTAMP | PWATTR_NICK | PWATTR_LOGIN)) && mi) {
    plopup_pushentry(dock, _("copy useragent in X clipboard"), PUP_COPY_UA);
  }
  if (pw && (pw->attr & PWATTR_TOTOZ)) {
    plopup_pushentry(dock, _("Try to download the correspounding picture"), PUP_DO_TOTOZ);
    plopup_pushentry(dock, _("Bookmark this picture"), PUP_DO_TOTOZ_BOOKMARK);
    plopup_pushentry(dock, _("Remove this picture from bookmarks"), PUP_DO_TOTOZ_UNBOOKMARK);
  }

  plopup_pushsepar(dock);

  if (mi == NULL) { /* on cherche qd même le message */
    mi = pp_find_message_at_xy(dock, event->x, event->y);
  }
  
  if (mi) {
    hk_plop = board_key_list_get_mi_positive_list(boards, mi, Prefs.plopify_key_list, 1);
    hk_emph = board_key_list_get_mi_positive_list(boards, mi, Prefs.hilight_key_list, 0);
  }
  
  for (hk = hk_plop, cnt = 0; hk && cnt < 10; hk = hk->next, cnt++) {
    char *splop[4] = {"plopify list", "superplopify list", "boitakon", "hungry boitakon"};    
    char s[512];
    snprintf(s, 512, _("remove %s:'<font color=blue>%.15s</font>' from %s"), key_list_type_name(hk->type), hk->key, splop[hk->num]);
    plopup_pushentry(dock, s, PUP_UNPLOP + cnt);
  }
  plopup_pushsepar(dock);
  for (hk = hk_emph, cnt = 0; hk && cnt < 10; hk = hk->next, cnt++) {
    char s[512];
    snprintf(s, 512, _("remove %s:'<font color=blue>%.15s</font> from hilight categ %d"), key_list_type_name(hk->type), hk->key, hk->num);
    plopup_pushentry(dock, s, PUP_UNEMPH + cnt);
  }

  plopup_pushsepar(dock);
  plopup_pushentry(dock, pp->use_minibar ? 
		   _("hide the button bar / tabs bar") : 
		   _("show the button bar / tabs bar"), PUP_TOGGLE_MINIB);

  if (mi) {
    plopup_pushentry(dock, _("Change colors for this site"), PUP_SITE_CONFIG);
  }

  if (Prefs.post_cmd[0]) {
    if (!Prefs.post_cmd_enabled[0]) {
      plopup_pushentry(dock, _("<b>enable</b> the bigornophone"), PUP_TOGGLE_BIGORNO1);
    } else {
      plopup_pushentry(dock, _("<b>disable</b> the bigornophone"), PUP_TOGGLE_BIGORNO1);
    }
  }
  if (Prefs.post_cmd[1]) {
    if (!Prefs.post_cmd_enabled[1]) {
      plopup_pushentry(dock, _("<b>enable</b> the secondary bigornophone"), PUP_TOGGLE_BIGORNO2);
    } else {
      plopup_pushentry(dock, _("<b>disable</b> the secondary bigornophone"), PUP_TOGGLE_BIGORNO2);
    }
  }


  plop_lvl = 3;
  emph_lvl = 4;
  redraw = 1;
  switch ((choice = plopup_show_modal(dock, event->x_root, event->y_root))) {
  case -1: redraw = 0; break;
  case PUP_PLOPIF: plop_lvl--; 
  case PUP_SUPERPLOPIF: plop_lvl--;
  case PUP_BOITAKON: plop_lvl--;
  case PUP_HUNGRY_BOITAKON: {
    assert(hk_what_clicked>=0);
    Prefs.plopify_key_list = key_list_swap(Prefs.plopify_key_list, txt, hk_what_clicked, plop_lvl);
    if (plop_lvl >= 2) boards_update_boitakon(boards);
  } break;
  case PUP_EMPH0: emph_lvl--;
  case PUP_EMPH1: emph_lvl--;
  case PUP_EMPH2: emph_lvl--;
  case PUP_EMPH3: emph_lvl--;
  case PUP_EMPH4: {
    assert(hk_what_clicked>=0);
    Prefs.hilight_key_list = key_list_swap(Prefs.hilight_key_list, txt, hk_what_clicked, emph_lvl);
  } break;
  case PUP_FILTER: {
    if (mi) {
      switch (what_clicked) {
      case WORD: pp_set_word_filter(dock, txt); break;
      case UA_WITH_LOGIN:
      case UA_NO_LOGIN: pp_set_ua_filter(dock, mi->useragent); break;
      case LOGIN: pp_set_login_filter(dock, mi->login); break;
      case THREAD: pp_set_thread_filter(dock, mi->id); break;
      case TSTAMP: pp_set_thread_filter(dock, mi->id); break;
      default: break;
      }
    }
  } break;
  case PUP_GOGOLE: {
    gogole_search(dock, mx, my, txt, 1, 1);
  } break;
  case PUP_WIKIPEDIA: {
    wikipedia_search(dock, mx, my, txt, 1, 1);
  } break;
  case PUP_COPY_URL: {
    assert(pw);
    editw_cb_copy(dock, pp->win, pw->attr_s, strlen(pw->attr_s));
  } break;
  case PUP_COPY_UA: {
    assert(mi); assert(pw);
    if (mi->useragent && strlen(mi->useragent)) {
      editw_cb_copy(dock, pp->win, mi->useragent, strlen(mi->useragent));	  
    }
  } break;
  case PUP_DO_TOTOZ: {
    assert(pw);
    pp_totoz_download(dock, pw->w);
  } break;
  case PUP_DO_TOTOZ_BOOKMARK:
  case PUP_DO_TOTOZ_UNBOOKMARK: {
    if (pw && strlen(pw->w)) {
      totoz_bookmark_insert(dock, pw->w);
      totoz_bookmark_save(dock, 1);
      totoz_bookmark_save_html(dock);
    }
  } break;
  case PUP_TOGGLE_MINIB: {
    if (pp->use_minibar == 0) {
      pp_minib_show(dock);
    } else {
      pp_minib_hide(dock);
    }
  } break;
  case PUP_SITE_CONFIG: {
    launch_wmccc(dock, "-site-colors", "-site", Prefs.site[id_type_sid(mi->id)]->site_name,NULL);
  } break;
  case PUP_TOGGLE_BIGORNO1: {
    Prefs.post_cmd_enabled[0] = 1-Prefs.post_cmd_enabled[0];
  } break;
  case PUP_TOGGLE_BIGORNO2: {
    Prefs.post_cmd_enabled[1] = 1-Prefs.post_cmd_enabled[1];
  } break;
  default: {
    if (choice >= PUP_UNPLOP && choice <= PUP_UNPLOP+10) {
      int updtbak = 0;
      cnt = choice - PUP_UNPLOP;
      for (hk = hk_plop; hk && cnt > 0; hk = hk->next, cnt--) {
	/*plop*/
      }
      assert(hk);
      //printf("plop remove '%s' %d\n", hk->key, hk->type);
      if (hk->num >= 2) updtbak = 1;
      Prefs.plopify_key_list = key_list_remove(Prefs.plopify_key_list, hk->key, hk->type);
      if (updtbak) boards_update_boitakon(boards);
    } else if (choice >= PUP_UNEMPH && choice <= PUP_UNEMPH+10) {
      cnt = choice - PUP_UNEMPH;
      for (hk = hk_emph; hk && cnt > 0; hk = hk->next, cnt--) {
	/*plop*/
      }
      assert(hk);
      //printf("hili remove '%s' %d\n", hk->key, hk->type);
      Prefs.hilight_key_list = key_list_remove(Prefs.hilight_key_list,hk->key, hk->type);
    }
  } break;
  }

  if (descr) free(descr);
  key_list_destroy(hk_plop); key_list_destroy(hk_emph);
  if (redraw) {
    /* force le rafraichissement complet */
    pp_update_and_redraw(dock, pp->id_base, pp->decal_base,0,0);
  }
}

static void
pp_open_login_home_in_browser(Dock *dock, int sid UNUSED, int mx, int my, char *w, int browser_num) {
  /*Pinnipede *pp = dock->pinnipede;
  char *s;
  assert(w);
  assert(Prefs.site[sid]);
  s = str_printf("http://%s:%d/%s~%s/", 
		 Prefs.site[sid]->site_root, 
		 Prefs.site[sid]->site_port, 
		 Prefs.site[sid]->site_path, w);
  open_url(s, pp->win_real_xpos + mx-5, pp->win_real_ypos+my-10, bnum);
  free(s);*/
  char *url = Prefs.site[sid]->backend_url;
  SplittedURL su; 
  if (url && split_url(url,&su) == 0) {
    char s[500]; snprintf(s, 500, "\"%s\" site:%s", w, su.host);
    gogole_search(dock, mx, my, s, 0, browser_num);
  } else gogole_search(dock,mx,my,w, 1, browser_num);
}


static void 
pp_open_url(Dock *dock, char *url, int mx, int my, int num) {
  Pinnipede *pp = dock->pinnipede;
  open_url(url, pp->win_real_xpos + mx-5, pp->win_real_ypos+my-10, num);
  pp_visited_links_add(pp, url);
  pp_update_and_redraw(dock, pp->id_base, pp->decal_base,0,1);
}

static void
pp_open_palmi_for_reply(Dock *dock, PostWord *pw) {
  char s_ts[300];
  int is_rss = 0;
  assert(pw->attr & PWATTR_TSTAMP);
  s_ts[0] = 0;
  is_rss = board_is_rss_feed(dock->sites->boards->btab[id_type_sid(pw->parent->id)]);
  if (is_rss) {
    RSSBonusInfo *ri = rss_find_from_id(dock->sites->boards, pw->parent->id);
    if (ri) snprintf(s_ts, sizeof s_ts, "%s", ri->link);
  }
  if (s_ts[0] == 0) {
    char s_subts[3];
    char *pwstart = strchr(pw->w, '#'); if (!pwstart) pwstart = pw->w; else pwstart++;
    s_subts[0] = s_subts[1] = s_subts[2] = 0;
    switch(pw->parent->sub_tstamp) {
    case -1: break;
    case 0: s_subts[0] = '¹'; break;
    case 1: s_subts[0] = '²'; break;
    case 2: s_subts[0] = '³'; break;
    default: s_subts[0] = ':'; s_subts[1] = '1' + pw->parent->sub_tstamp;
    }
    snprintf(s_ts, 30, "%s%s", pw->w, s_subts);
  }
  if (editw_ismapped(dock->editw) == 0) {
    char *username = Prefs.site[id_type_sid(pw->parent->id)]->user_name;
    if (username) {
      snprintf(dock->coin_coin_message, MESSAGE_MAXMAX_LEN, "%s %s ",
               username, s_ts);
    } else {
      snprintf(dock->coin_coin_message, MESSAGE_MAXMAX_LEN, "%s ",
               s_ts);
    }
    //	  strncpy(dock->coin_coin_message, pw->w, MESSAGE_MAX_LEN);
    // strncat(dock->coin_coin_message, " ", MESSAGE_MAX_LEN);
    dock->coin_coin_message[MESSAGE_MAXMAX_LEN] = 0;
    editw_show(dock, is_rss ? NULL : Prefs.site[id_type_sid(pw->parent->id)], 0);
    editw_move_end_of_line(dock->editw, 0);
    editw_refresh(dock, dock->editw);
  } else {
    char s[300];
    if (editw_get_site_id(dock) == id_type_sid(pw->parent->id) || is_rss) {
      snprintf(s, sizeof s, "%s ", s_ts);
    } else { 
      snprintf(s, sizeof s, "%s@%s ", s_ts, Prefs.site[id_type_sid(pw->parent->id)]->site_name); 
    }
    editw_insert_string(dock->editw, s);
    editw_refresh(dock, dock->editw);
  }
}

void pp_show_message_from_id(Dock *dock, id_type id) {
  Pinnipede *pp = dock->pinnipede;
  pp_update_content(dock, id, 0, 0, 0);
  pp_refresh(dock, pp->win, NULL);
}
static void
pp_handle_left_clic(Dock *dock, int mx, int my)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  PostWord *pw;

  /* affichage/masquage du 'crochet' à gauche des messages mis en valeur */
  if (mx < 5) {
    pw = pp_get_pw_at_xy(pp, 20, my); /* le 20 est une ruse de chacal puant */
    if (pw) {
      int changed = 1;
      if (pw->parent->is_my_message) {
	/* désactivé car inutile */
	/* pp->hilight_my_message_mode = 1-pp->hilight_my_message_mode; */
      } else if (pw->parent->is_answer_to_me) {
	/* pareil */
	/* pp->hilight_answer_to_me_mode = 1-pp->hilight_answer_to_me_mode; */
      } else if (pw->parent->is_hilight_key) {
	board_msg_info *mi;
	KeyList *hk;
	mi = boards_find_id(boards, pw->parent->id);
	if (mi && (hk = board_key_list_test_mi(boards, mi, Prefs.hilight_key_list))) {
	  Prefs.hilight_key_list = key_list_remove(Prefs.hilight_key_list, hk->key, hk->type);
	}
      } else changed = 0;
      if (changed) {
	/* force le rafraichissement complet */
	pp_update_and_redraw(dock, pp->id_base, pp->decal_base,0,0);
      }
    }
  }
  
  pw = pp_get_pw_at_xy(pp, mx, my);
  if (pw) {
    /* clic gauche sur une url , on affiche le truc dans le browser externe numero 1 */
    if (pw->attr & PWATTR_LNK) {
      if (strlen(pw->attr_s)) {
	pp_open_url(dock, pw->attr_s, mx, my, 1);
      }
    } else if (pw->attr & PWATTR_TSTAMP) {
      /* clic sur l'holorge -> ouverture du palmipede */
      pp_open_palmi_for_reply(dock, pw);
    } else if (pw->attr & PWATTR_REFID) {
      /* clic sur une reference, on va essayer de se déplacer pour afficher la ref en bas du
	 pinnipede */
      board_msg_info *mi;
      int is_a_ref;

      mi = check_for_id_ref(boards, pw->parent->id, pw->raw, NULL, 0, &is_a_ref);
      assert(is_a_ref);

      if (mi) {
	if (!id_type_is_invalid(mi->id)) {
	  int i;
	  for (i=0; i < pp->nb_tabs; i++) {
	    if (pp->tabs[i].site->prefs == Prefs.site[id_type_sid(mi->id)]) {
	      if (pp->tabs[i].selected == 0) {
		pp->tabs[i].selected = 1;
		pp_tabs_set_visible_sites(pp);
	      }
	    }
	  }
	}
        pp_show_message_from_id(dock, mi->id);
      }
    } else if (pw->attr & PWATTR_REF) {
      /* clic sur une reference, on va essayer de se déplacer pour afficher la ref en bas du
	 pinnipede */
      board_msg_info *mi;
      int bidon;
      int ref_num;

      mi = check_for_horloge_ref(boards, pw->parent->id, pw->raw, NULL, 0, &bidon, &ref_num); assert(bidon);



      if (mi) {
	/* si la reference désigne plusieurs post de la même heure ,
	   on se déplace vers le dernier du bloc
	*/
	if (ref_num == -1) {
	  while (mi->next && mi->next->timestamp == mi->timestamp) {
	    mi = mi->next;
	  }
	}

	if (!id_type_is_invalid(mi->id)) {
	  int i;
	  for (i=0; i < pp->nb_tabs; i++) {
	    if (pp->tabs[i].site->prefs == Prefs.site[id_type_sid(mi->id)]) {
	      if (pp->tabs[i].selected == 0) {
		pp->tabs[i].selected = 1;
		pp_tabs_set_visible_sites(pp);
	      }
	    }
	  }
	}
        pp_show_message_from_id(dock, mi->id);
      }
    } else if (pw->attr & PWATTR_LOGIN) {
      pp_open_login_home_in_browser(dock, id_type_sid(pw->parent->id),mx, my, pw->w,1);
    }
  } /* if (pw) */  
}

/* gestion du relachement du bouton souris (si on n'est pas en train de 'tirer' la fenetre, 
   et si on n'a pas cliqué sur la barre de petits boutons */
void 
pp_handle_button_release(Dock *dock, XButtonEvent *event)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  int mx,my;
  static Time previous_clic = 0;
  static int q = 1;

  mx = event->x; my = event->y;
  /* acceleration de la roulette */
  if (event->time - previous_clic < 50) {
    q = 4;
    if (event->time - previous_clic < 20) {
      q = 10;
    }
  } else q = 2;
  //printf("t=%ld, pt=%ld, q=%d\n", event->time, previous_clic, q);
  previous_clic = event->time;
  
  if (event->button == Button4) {
    /* un coup de roulette */
    pp_update_content(dock, pp->id_base, pp->decal_base-q,0,0);
    pp_refresh(dock, pp->win, NULL);
    //    printf("scroll up  : id=%d %d\n",pp->id_base, pp->decal_base);
  } else if (event->button == Button5) {
    
    pp_update_content(dock, pp->id_base, pp->decal_base+q,0,0);
    pp_refresh(dock, pp->win, NULL);
    //printf("scroll down: id=%d %d\n",pp->id_base, pp->decal_base);
  } else if (event->button == Button1) {
    if (event->state & ShiftMask) {
      // pp_handle_shift_clic(dock, &Prefs.hilight_key_list, mx, my, 0);
    } else if (event->state & ControlMask) {
      //      pp_handle_control_left_clic(dock, mx, my);
    } else if (event->state & (Mod1Mask | Mod4Mask) ) { /* on est gentil, les deux marchent */
      //      pp_handle_alt_clic(dock, event);
    } else {
      pp_handle_left_clic(dock, mx, my);
    }
  } else if (event->button == Button2) {
    if ((event->state & (ControlMask|ShiftMask|Mod1Mask|Mod4Mask))==0) {
      PostWord *pw;

      pw = pp_get_pw_at_xy(pp, mx, my);
      /* middle clic sur une horloge, on copie le contenu du message dans le clipboard */
      if (pw && (pw->attr & PWATTR_TSTAMP)) {
	board_msg_info *mi;
	mi = boards_find_id(boards, pw->parent->id);
	if (mi) {
	  if (mi->msg && strlen(mi->msg)) {
	    editw_cb_copy(dock, pp->win, mi->msg, strlen(mi->msg));
	  }
	}
      } else if (pw && pw->attr & PWATTR_LNK) {
	/* clic milieu sur une url , on affiche le truc dans le browser externe numero 2 */
	if (strlen(pw->attr_s)) {
	  pp_open_url(dock, pw->attr_s, mx, my,2);
	}
      } else if (pw && pw->attr & PWATTR_LOGIN) {
	pp_open_login_home_in_browser(dock, id_type_sid(pw->parent->id), mx, my, pw->w,2);
      } else {
	if (pp->use_minibar == 0) {
	  pp_minib_show(dock);
	} else {
	  pp_minib_hide(dock);
	}    
        pp_update_and_redraw(dock, pp->id_base, pp->decal_base,0,0);
      }
    } else if (event->state & (Mod1Mask|Mod4Mask)) { /* les 2 touches marchent */
      // pp_handle_alt_clic(dock, event);
    } else if (event->state & ControlMask) {
      //int save_all = (event->state & ShiftMask) ? 1 : 0;
      int use_js = (event->state & ShiftMask) ? 1 : 0;
      /* Ctrl+Middle clic: Et un scrinechote, un ! */
      pp_boardshot_kikoooo(dock, 0, 1, use_js);
    }
  }
}

int 
flush_expose(Dock *dock, Window w) {
  XEvent dummy;
  int i=0;

  while (XCheckTypedWindowEvent(dock->display, w, Expose, &dummy))
    i++;
  return i;
}


/* vilaine tentative pour 'flusher' les mouvements de souris consécutifs (cad 
   ne se situant pas de part et d'autre d'un evenement du style mouse-clic)

   hypothèse: XCheckIfEvent parcourt la file depuis l'ev le + vieux vers le + récent
   (ça semble naturel)

   ça a l'air de bien marcher :)
*/

int fcmmp_state; /* Mal !! */
Bool
flush_consecutive_mouse_motions_predicate(Display *display UNUSED, XEvent *ev, XPointer pw)
{
  //  printf("-- test: ev->type = %d\n", ev->type);
  if (fcmmp_state == 1) return False;
  else if (ev->type == MotionNotify && ev->xmotion.window == *((Window*)pw)) return True;
  else if (ev->type == ButtonPress || ev->type == ButtonRelease || ev->type == LeaveNotify || ev->type == EnterNotify) { fcmmp_state = 1; return False; }
  return False;
}

XEvent *
flush_consecutive_mouse_motions(Dock *dock, Window w, XEvent *ev_init) {
  static XEvent evtmp, ev; /* static == c mal (pas reentrant) */
  int cnt = 0;

  //  printf("debut\n");
  fcmmp_state = 0;
  while (XCheckIfEvent(dock->display, &evtmp, flush_consecutive_mouse_motions_predicate, (XPointer)&w)) {
    ev = evtmp; cnt++;
    assert(ev.type == MotionNotify);
    //    printf("** trouvé: x=%d, y=%d\n", ev.xmotion.x, ev.xmotion.y);
    fcmmp_state = 0;
  }
  //  printf("fin\n");

  if (cnt) return &ev; else return ev_init;
}

/* 
   pour la selection (reperer quel mots sont à tel position, et quel caractère précisement.. )
*/
int
pp_selection_find_pos(Dock *dock, PostWord *first_pw, int mouse_x, PostWord **sel_pw, int *sel_pw_char_num)
{
  Pinnipede *pp = dock->pinnipede;
  PostWord *pw = first_pw;
  int x_sel;
  int x = mouse_x - pp->zmsg_x1;
  if (sel_pw) *sel_pw = NULL;
  if (sel_pw_char_num) *sel_pw_char_num = 0;
  x_sel = 0;

  if (first_pw == NULL) return 0;

  /* sans commentaires c'est mieux */
  while (pw && pw->ligne == first_pw->ligne) {
    x_sel = (pw == first_pw) ? 0 : pw->xpos;
    if (x < pw->xpos) 
      break;
    if (x < pw->xpos + pw->xwidth) {
      int i, len;
      CCFontId fn;

      fn = pv_get_font(pp, pw->attr);
      len = strlen(pw->w);
      for (i = 1; i <= len; i++) {
	int xx;
	xx = pw->xpos+ccfont_text_width_utf8(fn, pw->w, i);
	if (x < xx) {
	  break;
	}
	x_sel = xx;
      }
      if (sel_pw_char_num) *sel_pw_char_num = i-1;
      break;
    }
    if (pw->next==NULL || 
	pw->next->ligne != first_pw->ligne || 
	x < pw->next->xpos) {
      x_sel = pw->xpos+pw->xwidth;
      if (sel_pw_char_num) *sel_pw_char_num = strlen(pw->w);
      break;
    }
    pw = pw->next;
  }
  if (sel_pw) *sel_pw = pw;
  return x_sel;
}

/* affichage de la zone de selection (on ne réaffiche que ce qui est nécessaire) */
void
pp_selection_refresh(Dock *dock)
{
  Pinnipede *pp = dock->pinnipede;

  int l, l0, l1;
  int mouse_x0, mouse_x1;



  /* début de selection ? */
  if (pp->lignes_sel == NULL) {
    ALLOC_VEC(pp->lignes_sel, pp->nb_lignes, PinnipedeLignesSel);
  }

  l0 = pp->sel_anchor_y; l0 = MAX(l0, LINEY0(0)) - LINEY0(0);
  l0 = l0 / (LINEY0(1)-LINEY0(0));
  l0 = MIN(l0, pp->nb_lignes-1);
  

  l1 = pp->sel_head_y; l1 = MAX(l1, LINEY0(0)) - LINEY0(0);
  l1 = l1 / pp->fn_h;//(LINEY0(1)-LINEY0(0));
  l1 = MIN(l1, pp->nb_lignes-1);
  
  if (l1 < l0) {
    pp->sel_l0 = l1; mouse_x0 = pp->sel_head_x;
    pp->sel_l1 = l0; mouse_x1 = pp->sel_anchor_x;
  } else if (l0 < l1) {
    pp->sel_l0 = l0; mouse_x0 = pp->sel_anchor_x;
    pp->sel_l1 = l1; mouse_x1 = pp->sel_head_x;
  } else {
    pp->sel_l0 = pp->sel_l1 = l0;
    mouse_x0 = MIN(pp->sel_anchor_x, pp->sel_head_x);
    mouse_x1 = MAX(pp->sel_anchor_x, pp->sel_head_x);
  }

  BLAHBLAH(2,myprintf("%<YEL selection refresh: [%d:%d] -- [%d:%d] l0=%d, l1=%d>\n", 
		      pp->sel_anchor_x, pp->sel_anchor_y, pp->sel_head_x, pp->sel_head_y,pp->sel_l0,pp->sel_l1));
  
  for (l=0; l < pp->nb_lignes; l++) {
    int sel_x0, sel_x1;
    PostWord *first, *last;
    int first_pos, last_pos;

    sel_x0 = sel_x1 = 0;
    first_pos = last_pos = 0;
    first = last = NULL;

    if (l>=pp->sel_l0 && l<=pp->sel_l1 && pp->lignes[l]) {
      first = pp->lignes[l];
      sel_x0 = 0; //first->xpos;
      last = first; 
      while (last->next && last->next->ligne == last->ligne) last = last->next;
      sel_x1 = last->xpos+last->xwidth;
      first_pos = 0;
      last_pos = strlen(last->w);
    }
    if (l==pp->sel_l0) {
      sel_x0 = pp_selection_find_pos(dock, pp->lignes[l], mouse_x0, &first, &first_pos);
    }
    if (l==pp->sel_l1) {
      sel_x1 = pp_selection_find_pos(dock, pp->lignes[l], mouse_x1, &last, &last_pos);
    }

    /* marque les lignes qui étaient selectionnes pour qu'elles soient redessinnées */
    if (pp->lignes_sel[l].x0 != sel_x0 || pp->lignes_sel[l].x1 != sel_x1) {
      pp->lignes_sel[l].trashed = 1;
      pp->lignes_sel[l].x0 = sel_x0;              pp->lignes_sel[l].x1 = sel_x1;
      pp->lignes_sel[l].first_pw = first;         pp->lignes_sel[l].last_pw = last;
      pp->lignes_sel[l].first_pw_pos = first_pos; pp->lignes_sel[l].last_pw_pos = last_pos;
    } else {
      pp->lignes_sel[l].trashed = 0;
    }
    BLAHBLAH(3,printf("l=%2d, %d first=%s [%d], last=%s [%d] x0=%d, x1=%d\n", l, 
		      pp->lignes_sel[l].trashed, first ? first->w : NULL, first_pos,
		      last ? last->w : NULL, last_pos, 
		      pp->lignes_sel[l].x0, pp->lignes_sel[l].x1 = sel_x1));
  }
  for (l=0; l < pp->nb_lignes; l++) {

    if (pp->lignes_sel[l].trashed) {
      unsigned long bgpixel = cccolor_pixel(pp->win_bgcolor[pp->active_tab->site->site_id]);
      if (pp->lignes[l]) { bgpixel = cccolor_pixel(pp->win_bgcolor[id_type_sid(pp->lignes[l]->parent->id)]); }
    
      pp_draw_line(dock, pp->lpix, pp->lignes[l], bgpixel, 
		   &pp->lignes_sel[l], pp->transparency_mode, LINEY0(l));
      XCopyArea(dock->display, pp->lpix, pp->win, dock->NormalGC, 0, 0, 
		pp->zmsg_w, pp->fn_h, pp->zmsg_x1, LINEY0(l));
    }
  }
}


/* copie la zone selectionnée dans un buffer et renvoie la
   taille de cette zone (on l'appelle une premiere fois pour connaitre la taille,
   puis une autre pour faire la copie effective)
*/
int
pp_selection_copy(Dock *dock, char *buff)
{
  Pinnipede *pp = dock->pinnipede;
  int l, nc;

  
  if (pp->lignes_sel == NULL) return 0;

  nc = 0;
  for (l=pp->sel_l0; l <= pp->sel_l1; l++) {
    PostWord *pw;
    /*    printf("first=%d, last=%d\n", pp->lignes_sel[l].first_pw_pos, pp->lignes_sel[l].last_pw_pos); */
    pw = pp->lignes_sel[l].first_pw;
    while (pw) {
      int i, i0, i1;
      i0 = 0; i1 = strlen(pw->w);
      if (pw == pp->lignes_sel[l].first_pw) {
	i0 = pp->lignes_sel[l].first_pw_pos;
      } 
      if (pw == pp->lignes_sel[l].last_pw) {
	i1 = pp->lignes_sel[l].last_pw_pos;
      }
      if (pw != pp->lignes_sel[l].first_pw && (pw->attr & PWATTR_HAS_INITIAL_SPACE)) {
	if (buff) buff[nc] = ' ';
	nc++;
      }
      for (i=i0; i < i1; i++) {
	if (buff) buff[nc] = pw->w[i];
	nc++;
      }
      if (pw == pp->lignes_sel[l].last_pw) break;
      pw = pw->next;
    }
    if (l < pp->sel_l1 && nc) {
      if (buff) buff[nc] = (pw == NULL || pw->next == NULL) ? '\n' : ' ';
      nc++;
    }
  }
  if (buff) buff[nc] = 0;
  nc++;
  return nc;
}

void
kbnav_move(Dock *dock, int dir) {
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  PostVisual *pv = NULL;
  id_type id = pp->kbnav_current_id;
  int restart = 0;
  PostWord *refpw = NULL;
  if (!pp->lignes) return;
  while (pv == NULL) {
    int i;
    for (i=0; i < pp->nb_lignes && pv == 0; ++i) {
      if (pp->lignes[i] && id_type_eq(pp->lignes[i]->parent->id,id)) { 
        pv = pp->lignes[i]->parent; 
      }
    }
    if (pv == NULL && restart == 0) {
      restart = 1;
      id = pp->id_base;
    } else break;
  }

  if (pv == NULL) return;
  if (restart) {
    pp->kbnav_current_tstamp = pp_pv_count_horloges(pv)-1;
  } else {
    int pv_nb_clock = pp_pv_count_horloges(pv);
    if (dir == -1) {
      if (pp->kbnav_current_tstamp > 0) {
        pp->kbnav_current_tstamp = MIN(pp->kbnav_current_tstamp-1, pv_nb_clock-1);
      } else {
        id = get_prev_id_filtered(boards,id,NULL,&pp->filter);
        pp->kbnav_current_tstamp = 10000;
      }
    } else if (dir == +1) {
      if (pp->kbnav_current_tstamp < pv_nb_clock-1) {
        pp->kbnav_current_tstamp++;
      } else {
        id = get_next_id_filtered(boards,id,NULL,&pp->filter);
        pp->kbnav_current_tstamp = -1;
      }
    }
  }
  pp->kbnav_current_id = id;
  
  if (!id_type_is_invalid(id)) {
    int lbefore = 0, lafter = 0, i;
    for (i=0; i < pp->nb_lignes; ++i) {
      if (pp->lignes[i] && pp->lignes[i]->parent && id_type_eq(pp->lignes[i]->parent->id, id)) {
        if (lbefore == 0)
          lbefore = i;
        lafter = pp->nb_lignes - i;
      }
    }
    /*printf("dir=%d, lbefore = %d, lafter = %d, nb_lignes=%d\n", dir,lbefore, lafter, pp->nb_lignes);*/
    if (!restart && lafter < pp->nb_lignes/4 && dir > 0)
      pp_update_content(dock, pp->kbnav_current_id, pp->nb_lignes/2,0,0);
    else if (!restart && lbefore < pp->nb_lignes/4 && dir < 0)
      pp_update_content(dock, pp->kbnav_current_id, MIN(10,pp->nb_lignes/4),0,0);
    else
      pp_update_content(dock, pp->id_base, pp->decal_base,0,0);
    pv = pp_find_pv(pp,pp->kbnav_current_id);
    if (pv) {
      int pv_nb_clock = pp_pv_count_horloges(pv);
      if (pv_nb_clock == 0) pp->kbnav_current_tstamp = -1;
      else pp->kbnav_current_tstamp = MAX(0,MIN(pp->kbnav_current_tstamp, pv_nb_clock-1));
      refpw = pp_pv_get_nth_horloge(pv, pp->kbnav_current_tstamp+1);
      /*    if (refpw) refpw->attr |= PWATTR_TMP_EMPH;
            pw = pv->first;
            while (pw && (pw->attr & PWATTR_TSTAMP)==0) pw = pw->next;
            if (pw) pw->attr |= PWATTR_TMP_EMPH;*/
    }
    pp_refresh(dock, pp->win, refpw);
    {
      PostWord *pwts = 0;
      if (pv) { 
        PostWord *pw = pv->first;
        while (pw) {
          if ((pw->attr & PWATTR_TSTAMP)) { pwts = pw;  break; }
          pw = pw->next;
        }
      }
      pp_check_survol(dock,pwts,-1);
    }
  }
}


static void switch_search_mode(Dock *dock) {
  Pinnipede *pp = dock->pinnipede;
  static char *old_s = 0;
  if (pp->filter.filter_mode) { 
    if (pp->filter.anything && strlen(pp->filter.anything))
      pp->filter.filter_mode = 0; 
    else pp_set_anything_filter(dock, old_s ? old_s : "");
  }
  else { 
    if (pp->filter.anything) { if (old_s) free(old_s); old_s = strdup(pp->filter.anything); }
    pp_set_anything_filter(dock, ""); 
  }
  pp_update_content(dock, id_type_invalid_id(), 0, 0, 1);
  pp_refresh(dock, pp->win, NULL);
}

static void pp_update_viewed_messages(Dock *dock, int x, int y) {
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  int l;
  if (x >= pp->zmsg_x1 && x <= pp->zmsg_x2) {    
    for (l=0; l < pp->nb_lignes; l++) {
      if (y > LINEY0(l) && y <= LINEY1(l)) {
        PostWord *pw = pp->lignes[l];
        if (pw && pw->parent) {
          int sid = id_type_sid(pw->parent->id);
          if (sid >= 0 && boards->btab[sid]) 
            board_set_viewed(boards->btab[sid], id_type_lid(pw->parent->id));
        }
      }
    }
  }
}

#define FORWARD_KEY XSendEvent(dock->display, dock->rootwin, True, KeyPressMask, event); 

/* /!\ spaghettis */
int
pp_handle_keypress(Dock *dock, XEvent *event)
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  int ret = 0;

  /*if ((event->xkey.state & 0xdf60) || // c'était 0xdfe0 avant xfree 4.3 :-/ altgr est devenu "iso-levl3-shift" 
      (kb_state()->input_context && XFilterEvent(event, None))) {
    //printf("forward key: \n");
    FORWARD_KEY;
    return 1;
  }
  */
#ifdef OLD_KBCOINCOIN
  kb_lookup_string(dock, &event->xkey);
#else 
  //kb_xim_lookup_key(&event->xkey, KB_PINNIPEDE);
#endif
  /*printf("klen=%2d %08x %c state=%08x buff=%02x%02x%02x%02x\n", 
         kb_state()->klen, (int)kb_state()->ksym, (int)kb_state()->ksym, event->xkey.state,
         kb_state()->buff[0],kb_state()->buff[1],kb_state()->buff[2],kb_state()->buff[3]);*/
  if (event->xkey.state & Mod1Mask) {
    /* ALT-TOUCHE */
  } else if (event->xkey.state & ControlMask) {
    /* CTRL-TOUCHE */
    switch (kb_state()->ksym) {
    case ' ': { /* ctrl-espace : rafraichit tous les sites */
      Site *site;
      for (site = dock->sites->list; site; site = site->next) {
        if (site->prefs->check_board) {
          ccqueue_push_board_update(site->site_id);
          site->board->board_refresh_decnt = site->board->board_refresh_delay;
        }
      }
      ret++;
    } break;
    /* "ctrl-F" : active/desactive le filtre */
    case 'F':
    case 'f': if (!editw_ismapped(dock->editw)) {
      pp->filter.filter_mode = 1-pp->filter.filter_mode;
      pp_update_content(dock, pp->id_base, 0,0,0);
      pp_refresh(dock, pp->win, NULL);
    } break;
    /* ctrl-s : mode recherche -- emule le isearch-mode de emacs */
    case 'S':
    case 's': if (event->xkey.window == pp->win && !editw_ismapped(dock->editw)) { 
      switch_search_mode(dock);
      ret++;
    } break;
    case 'Z':
    case 'z': {
      //flag_discretion_request = +1; ret++;
    } break;
    /* CTRL-ENTER : ouvre le palmi pour répondre au message affiché en bas du pinni */
    case XK_Return:
    case XK_KP_Enter: {
      PostWord *pwts = NULL;
      PostVisual *pv;
      if ((pv = pp_find_pv(pp, pp->kbnav_current_id))) {
        PostWord *pw = pv->first;
        while (pw) {
          if ((pw->attr & PWATTR_TSTAMP)) { pwts = pw;  break; }
          pw = pw->next;
        }
      }
      if (pwts == NULL && pp->lignes) {
        int i=pp->nb_lignes-1;
        /* :(================ */
        while (i>=0 && !pwts) {
          if (pp->lignes[i]) {          
            PostWord *pw = pp->lignes[i];
            while (pw) {
              if ((pw->attr & PWATTR_TSTAMP)) { pwts = pw;  break; }
              pw = pw->next;
            }
          }
          i--;
        }
      }
      if (pwts)
        pp_open_palmi_for_reply(dock,pwts);
      ret++;
    } break;
    case XK_KP_Up:
    case XK_Up: {
      kbnav_move(dock,-1);
      ret++;
    } break;
    case XK_KP_Down:
    case XK_Down: {
      kbnav_move(dock,+1);
      ret++;
    } break;
    }
  } else if (pp->filter.filter_mode && pp->filter.anything && !editw_ismapped(dock->editw)) {
    /* TOUCHE NORMALE EN MODE RECHERCHE */
    int c = 0;
    //fprintf(stderr, "recherche : kb_state()->ksym = %04x (%c), klen=%04x, buff=%02x%02x%02x%02x\n", kb_state()->ksym, kb_state()->ksym, klen, buff[0],buff[1],buff[2],buff[3]);
    if (kb_state()->ksym >= ' ' && kb_state()->ksym < 255) { c = kb_state()->ksym; }
    else if (kb_state()->ksym >= XK_KP_0 && kb_state()->ksym <= XK_KP_9) { c = '0' + kb_state()->ksym - XK_KP_0; }
    else {
      switch (kb_state()->ksym) {
      case XK_KP_Decimal: c = '.'; break;
      case XK_KP_Subtract: c = '-'; break;
      case XK_KP_Add: c = '+'; break;
      case XK_KP_Divide: c = '/'; break;
      case XK_KP_Multiply: c = '*'; break;
      case XK_BackSpace: c = -2; break;
      case XK_Return:
      case XK_KP_Enter:
      case XK_KP_Up:
      case XK_Up:
      case XK_KP_Down:
      case XK_Down:
      case XK_KP_Left:
      case XK_Left:
      case XK_KP_Right:
      case XK_Right:
      case XK_KP_Page_Up:
      case XK_Page_Up:
      case XK_KP_Page_Down:
      case XK_Page_Down:
      case XK_KP_Home:
      case XK_Home:
      case XK_KP_End:
      case XK_End:
      case XK_Tab:
      case XK_Escape: c = -1; break; /* pffff liste a la con */
      }
    }
    if (c > 0) {
      char *s = str_printf("%s%c", pp->filter.anything, c);
      pp_set_anything_filter(dock, s);
      free(s);
    } else if (c == -2) {
      char *s = strdup(pp->filter.anything);
      if (s[0]) { s[strlen(s)-1] = 0; }
      pp_set_anything_filter(dock, s);
      free(s);
    } else if (c == -1) { 
      pp->filter.filter_mode = 0;
      pp_update_content(dock, id_type_invalid_id(), 0, 0, 1);
      pp_refresh(dock, pp->win, NULL);
    }
    ret++;
  } else {
    //kb_lookup_mb_string(dock, &event->xkey);
    //printf("(BIS) klen=%2d %08x %c\n", kb_state()->klen, kb_state()->ksym, kb_state()->ksym);
    /* TOUCHE NORMALE */
    switch (kb_state()->ksym) {
      /* fleche droite: active le tab de droite */
    case XK_KP_Left:
    case XK_Left: if (!editw_ismapped(dock->editw)) {
      if (event->xkey.window == pp->win) {
        pp_change_active_tab(dock,-1);
        ret++;
      }
    } break;
    /* fleche gauche: active le tabs de gauche */
    case XK_KP_Right:
    case XK_Right: if (!editw_ismapped(dock->editw)) {
      if (event->xkey.window == pp->win) {
        pp_change_active_tab(dock,+1);
        ret++;
      }
    } break;
    /* "tab" : switch entre un seul tab et tous les tabs simultanés */
    case XK_Tab: if (!editw_ismapped(dock->editw)) {
        pp_tabs_switch_all_selected(pp); pp_tabs_changed(dock);
    } break;
    /* "R" : active/desactive l'autorefresh */
    case 'R':
    case 'r': {
      if (event->xkey.window == pp->win && !editw_ismapped(dock->editw)) {
        if (pp->active_tab) {
          pp->active_tab->site->board->auto_refresh = 1-pp->active_tab->site->board->auto_refresh;
          pp_tabs_refresh(dock);
          ret++;
        }
      }
    } break;
    case '/':  if (event->xkey.window == pp->win && !editw_ismapped(dock->editw) && !(pp->filter.filter_mode && pp->filter.anything)) { 
      switch_search_mode(dock);
      pp_popup_show_txt(dock, "vi sux!!!");
      ret++;
    } break;
    case ' ': {
      if (event->xkey.window == pp->win && !editw_ismapped(dock->editw)) {
        if (pp->active_tab) {
          ccqueue_push_board_update(pp->active_tab->site->site_id);
          pp->active_tab->site->board->board_refresh_decnt = pp->active_tab->site->board->board_refresh_delay;
          ret++;
        }
      }
    } break;
    case XK_Return:
    case XK_KP_Enter: if (!editw_ismapped(dock->editw)) {
      if (editw_check_corse(dock, event->xkey.keycode)) {
        editw_show(dock, pp->active_tab ? pp->active_tab->site->prefs : 0, 0);
      }
      ret++;
    } break;
    case XK_KP_Up:
    case XK_Up: if (!editw_ismapped(dock->editw)) {
      pp_update_content(dock, pp->id_base, pp->decal_base-1,0,0);
      pp_refresh(dock, pp->win, NULL);
      ret++;
    } break;
    case XK_KP_Down:
    case XK_Down: if (!editw_ismapped(dock->editw)) {
      pp_update_content(dock, pp->id_base, pp->decal_base+1,0,0);
      pp_refresh(dock, pp->win, NULL);
      ret++;
    } break;
    case XK_KP_Page_Up:
    case XK_Page_Up: if (!editw_ismapped(dock->editw)) {
      pp_update_content(dock, pp->id_base, pp->decal_base-pp->nb_lignes,0,0);
      pp_refresh(dock, pp->win, NULL);
      ret++;
    } break;
    case XK_KP_Page_Down:
    case XK_Page_Down: if (!editw_ismapped(dock->editw)) {
      pp_update_content(dock, pp->id_base, pp->decal_base+pp->nb_lignes,0,0);
      pp_refresh(dock, pp->win, NULL);
      ret++;
    } break;
    case XK_KP_Home:
    case XK_Home: if (!editw_ismapped(dock->editw)) {
      pp_update_content(dock, get_nth_id_filtered(boards, &pp->filter, 0), 0,0,0);
      pp_refresh(dock, pp->win, NULL);
      ret++;
    } break;
    case XK_KP_End:
    case XK_End: if (!editw_ismapped(dock->editw)) {
      pp_update_content(dock, get_last_id_filtered(boards, &pp->filter), 0,0,0);
      pp_refresh(dock, pp->win, NULL);
      ret++;
    } break;
    case XK_Escape: if (!editw_ismapped(dock->editw)) {
      flag_cancel_task = 1;
      ret++;
    } break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': if (!editw_ismapped(dock->editw)) {
      int c = kb_state()->ksym - '1';
      if (c < pp->nb_tabs) {
        pp_tabs_cliquouille(pp, pp->tabs+c, PPT_MAY_SET_MAIN_TAB); pp_tabs_changed(dock);
      }
      ret++;
    } break;
    default: {
      ret = 0;
    } break;
    }
  }
  return ret;
}

int
pp_handle_keyrelease(Dock *dock, XEvent *event)
{
  (void) dock;
  (void) event;
  return 0;
}

int
pp_dispatch_event(Dock *dock, XEvent *event)
{
  Pinnipede *pp = dock->pinnipede;

  static int old_mouse_x = -1, old_mouse_y = -1;
  static int mouse_button_press_x = -1, mouse_button_press_y = -1;
  
  static int dragging = 0;
  static Time time_drag = 0;

  if (pp_totoz_dispatch_event(dock,event)) return 1;
  if (event->xany.window == None || event->xany.window != pp->win) return 0;

  /* le pinnipede ne fait RIEN quand la tribune est en cours de mise à jour ... */
  /*if (flag_updating_board) {
    printf("%s et merde .. event type %d pendant que flag_updating_board = %d\n", ctime(time(NULL)), event->type, flag_updating_board);
    dump_backtrace();
    return 0;
  }
  */
  switch (event->type) {
  case DestroyNotify: 
    {
      //      printf("destroy?\n");
    } break;
  case ButtonPress:
    {     
      pp_selection_unselect(pp);
      if (pp_widgets_handle_button_press(dock, &event->xbutton)) {
      } else {
	mouse_button_press_x = old_mouse_x = event->xbutton.x;
	mouse_button_press_y = old_mouse_y = event->xbutton.y;
	dragging = 0;
	time_drag = event->xbutton.time;
	if (event->xbutton.button == Button3
	    && (event->xbutton.state & (ShiftMask | ControlMask)) == 0) {
	  pp_handle_button3_press(dock, &event->xbutton);
	}
      }
    } break;
  case ButtonRelease:
    {
      old_mouse_x = event->xbutton.x;
      old_mouse_y = event->xbutton.y;

      if (pp->lignes_sel) {
	int blen;
	
	blen = pp_selection_copy(dock, NULL);
	if (blen) {
	  if (pp->last_selected_text) { free(pp->last_selected_text); pp->last_selected_text=NULL;}	  	  
	  pp->last_selected_text = malloc(blen); assert(pp->last_selected_text);
	  pp_selection_copy(dock, pp->last_selected_text);
	  editw_cb_copy(dock, pp->win, pp->last_selected_text, blen-1);
	  pp->time_sel = time(NULL);
	}
      } else {
	if (pp_widgets_handle_button_release(dock, &event->xbutton)) {
	  //printf("plop\n");
	} else if (dragging == 0) {
	  pp_handle_button_release(dock, &event->xbutton);
	}
      }
      dragging = 0;
      time_drag = event->xbutton.time;
    } break;
  case MotionNotify:
    {
      event = flush_consecutive_mouse_motions(dock, pp->win, event);

      if (pp_widgets_handle_motion(dock, &event->xmotion)) {
	/* plop */
      } else if (event->xmotion.state & Button1Mask && 
	  SQR(mouse_button_press_x - event->xbutton.x)+
	  SQR(mouse_button_press_y - event->xbutton.y) >= 6) {

	pp->time_sel = time(NULL);
	if (pp->lignes_sel == NULL) {
	  pp->sel_anchor_x = old_mouse_x; pp->sel_anchor_y = old_mouse_y;
	  pp_refresh(dock, pp->win, NULL);
	}
	pp->sel_head_x = event->xmotion.x; pp->sel_head_y = event->xmotion.y;
	pp_selection_refresh(dock);
      } else if ((event->xmotion.state & Button2Mask)==Button2Mask) {
	int decal_y;
	decal_y = event->xmotion.y - old_mouse_y;
	decal_y /= 8;
	if (decal_y && (event->xmotion.time-time_drag)>25) {
	  dragging = 1;
	  //	  printf("move (%ld) %ld!\n", event->xmotion.time, time_drag);
	  pp_update_content(dock, pp->id_base, pp->decal_base-decal_y,0,0);
	  pp_refresh(dock, pp->win, NULL);
	  old_mouse_x = event->xmotion.x;
	  old_mouse_y = event->xmotion.y;
	  time_drag = event->xmotion.time;
	}
      } else {
        PostWord *pw = pp_get_pw_at_xy(dock->pinnipede, event->xmotion.x, event->xmotion.y);
	pp_check_survol(dock, pw, 0);
        pp_check_totoz(dock, pw, event->xmotion.x, event->xmotion.y);/*event->xmotion.x_root, event->xmotion.y_root);*/
	old_mouse_x = event->xmotion.x;
	old_mouse_y = event->xmotion.y;
        pp_update_viewed_messages(dock, event->xmotion.x, event->xmotion.y);
      }
    } break;
  case ConfigureNotify:
    {
      XWindowAttributes wa;
      Window child;
      int dim_changed = 0;

      
      //printf("ConfigureNotify: w<-%d, h<-%d\n", event->xconfigure.width, event->xconfigure.height);

      XGetWindowAttributes(dock->display, pp->win, &wa);
      
      //      printf("expose: width = %d (%d), height=%d (%d)\n", 
      //	     wa.width,pp->win_width,wa.height,pp->win_height);
      get_window_pos_with_decor(dock->display, pp->win, &pp->win_decor_xpos, &pp->win_decor_ypos);
      
      XTranslateCoordinates(dock->display, pp->win, dock->rootwin, 
      			    0/*wa.x*/, 0/*wa.y*/, &pp->win_real_xpos, &pp->win_real_ypos, &child);
      
      //      printf(" -> xpos=%d, ypos=%d [%d %d]\n", pp->win_real_xpos,pp->win_real_ypos, wa.x, wa.y);
      dim_changed = (event->xconfigure.width != pp->win_width || event->xconfigure.height != pp->win_height);
      if (dim_changed || pp->transparency_mode) {
	pp->win_width = MAX(event->xconfigure.width,80);
	pp->win_height = MAX(event->xconfigure.height,80);
	
	/* eh oui, faut pas oublier ça.... */
	if (Prefs.use_fake_real_transparency == 0)
          pp_update_bg_pixmap(dock);
	pp_update_and_redraw(dock, pp->id_base, pp->decal_base, 0,1);
      }
    } break;
  case EnterNotify:
    {
    } break;
  case LeaveNotify:
    {
      pp->survol_tab = NULL;
    } break;
  case Expose:
    {
      static int zx0, zy0, zx1, zy1, zinit = 0;
      int x0, y0, x1, y1;

      x0 = event->xexpose.x;            y0 = event->xexpose.y;
      x1 = x0 + event->xexpose.width-1; y1 = y0 + event->xexpose.height -1;
      if (zinit == 0) {
	zx0 = x0; zy0 = y0; zx1 = x1; zy1 = y1;
      } else {
	zx0 = MIN(zx0, x0); zx1=MAX(zx1, x1);
	zy0 = MIN(zy0, y0); zy1=MAX(zy1, y1);
      }

      /*printf("expose_event: x=%d, y=%d, w=%d, h=%d cnt=%d --> z=[%d:%d]x[%d:%d]\n", 
	event->xexpose.x, event->xexpose.y, event->xexpose.width, event->xexpose.height, event->xexpose.count,
	zx0, zx1, zy0, zy1);
      */
      if (event->xexpose.count == 0) {
        //printf("REFRESH!\n");
	//pp_refresh(dock, pp->win, NULL);
        pp->flag_pp_refresh_request = 1;
	flush_expose(dock, pp->win);
      }
    } break;
  case MapNotify:
    {
    } break;
  case SelectionRequest: 
    {
      editw_cb_handle_selectionrequest(dock, &(event->xselectionrequest));
    } break;
  case ClientMessage:
    {
      if (event->xclient.message_type == dock->atom_WM_PROTOCOLS
	  && event->xclient.format == 32 
	  && (Atom)event->xclient.data.l[0] == dock->atom_WM_DELETE_WINDOW) {
	pp_unmap(dock);
      }
    } break;
  }
  return 1;
}

void pp_unset_kbnav(Dock *dock) {
  dock->pinnipede->kbnav_current_id = id_type_invalid_id();
  dock->pinnipede->kbnav_current_tstamp = -1;
}

Window pp_get_win(Dock *dock) {
  return dock->pinnipede->win;
}

void
pp_set_board_updated(Dock *dock)
{
  dock->pinnipede->flag_board_updated = 1;
}

/* sauvegarde de la position et des dimensions du pinni 
   (appelé par wmcc_save_state de wmcoincoin.c) 
 */
void
pp_save_state(Dock *dock, FILE *f) {
  Pinnipede *pp = dock->pinnipede;
  int i;
  fprintf(f, "%d %d %d %d %d\n", 
	  pp->mapped,
	  pp->win_decor_xpos, pp->win_decor_ypos, 
	  pp->win_width, pp->win_height);

  fprintf(f, "LINKS %d\n", pp->nb_visited_links);
  for (i=0; i < pp->nb_visited_links; i++)
    fprintf(f, "%d\n", pp->visited_links[i]);
  pp_tabs_save_state(dock,f);
}


void
pp_restore_state(Dock *dock, FILE *f) {
  Pinnipede *pp = dock->pinnipede;
  int mapped, win_xpos, win_ypos, win_width, win_height;
  int nlnk;

  if (fscanf(f, "%d %d %d %d %d\n", 
	     &mapped,
	     &win_xpos, &win_ypos, 
	     &win_width, &win_height) == 5) {

    /* on vérifie qu'on n'a pas spécifié de préferences dans le fichier d'options */
    if (Prefs.pp_xpos == -10000 && Prefs.pp_ypos == -10000) {
      if (win_xpos != -10000) {
	pp->win_decor_xpos = MAX(MIN(win_xpos,3000),-20);
	pp->win_decor_ypos = MAX(MIN(win_ypos,3000),-20);
	pp->win_width = MAX(MIN(win_width,3000),50);
	pp->win_height = MAX(MIN(win_height,3000),50);
      }
      pp_widgets_set_pos(dock);
      //      if (pp->use_minibar)
      //	pp_minib_initialize(pp);
    }

    if (fscanf(f, "LINKS %d\n", &nlnk)==1 && nlnk >= 0) {
      int i;
      pp->nb_visited_links = nlnk;
      for (i=0; i < pp->nb_visited_links; i++) {
        if (fscanf(f, "%d\n", &pp->visited_links[i]) == EOF)
          myfprintf(stderr, "fscanf() failed\n");
      }
    }
    pp_tabs_restore_state(dock,f);
  }  
}
