/*
  rcsid=$Id: picohtml.c,v 1.18 2004/02/29 19:01:27 pouaite Exp $
  ChangeLog:
  $Log: picohtml.c,v $
  Revision 1.18  2004/02/29 19:01:27  pouaite
  et hop

  Revision 1.17  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.16  2003/01/11 17:44:10  pouaite
  ajout de stats/coinping sur les sites

  Revision 1.15  2002/12/20 11:26:35  pouaite
  deux trois conneries

  Revision 1.14  2002/10/16 20:41:45  pouaite
  killall toto

  Revision 1.13  2002/10/15 23:17:28  pouaite
  rustinage à la truelle

  Revision 1.12  2002/09/22 23:16:33  pouaite
  i had a friend, but he does not move anymore

  Revision 1.11  2002/08/17 18:33:39  pouaite
  grosse commition

  Revision 1.10  2002/06/23 14:01:36  pouaite
  ouups, j'avais flingué les modifs depuis la v2.3.8b

  Revision 1.9  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.7  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

  Revision 1.6  2002/03/03 10:10:04  pouaite
  bugfixes divers et variés

  Revision 1.5  2002/02/27 00:32:19  pouaite
  modifs velues

  Revision 1.4  2002/01/12 19:03:54  pouaite
  bugfix de picohtml et raccourci altgr-e pour le symbole euro (gruik)

  Revision 1.3  2002/01/12 17:29:08  pouaite
  support de l'iso8859-15 (euro..)

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

*/

#include <libintl.h>
#define _(String) gettext (String)

#include "coincoin.h"
#include "coin_xutil.h"
#include "picohtml.h"

/* dans la famille des fonction pourries, je demande ... */
static char *
get_tok(const unsigned char **p, const unsigned char **np, 
	unsigned char *tok, int max_toklen)
{
  const unsigned char *start, *end;
  assert(p); 
  assert(*p); 
  assert(tok);

  start = *p; *np = NULL;
  // saute les espaces
  while (*start <= ' ' && *start) start++;
  end = start;
  if (*start == '<') {
    while (*end && *end != '>') end++;
    if (*end) end++;
  } else {
    while (*end && *end != '<' && *end > ' ') end++;
  }
  if (end == start) return NULL;
  else strncpy(tok, start, MIN(end-start, max_toklen-1));
  tok[MIN(end-start, max_toklen-1)] = 0;
  *p = start;
  *np = end;
  return tok;
}

void
picohtml_set_url_path(PicoHtml *ph, const char *s) {
  ph->url_path = strdup(s);
}

void
picohtml_unset_url_path(PicoHtml *ph) {
  COND_FREE(ph->url_path);
}

/* ajoute un mot dans la liste */
static PicoHtmlItem *
picohtml_additem(PicoHtml *ph, const unsigned char *s, int len, c_attr attrib, int x, int y, 
	       CCFontId fn, CCColorId color, char *link, int special_attr)
{
  PicoHtmlItem *it, *pit;
  int i;

  assert(len < 10000); /* restons raisonnables */
  /* recherche du dernier elt */
  it = ph->txt;
  if (it) {
    while (it) {
      pit = it;
      it = it->next;
    }
    ALLOC_OBJ(it, PicoHtmlItem);
    pit->next = it;
  } else {
    ALLOC_OBJ(it, PicoHtmlItem); ph->txt = it;
  }

  ALLOC_VEC(it->s, len+1, unsigned char);
  it->s[len] = 0;
  for (i=0; i < len; i++) it->s[i] = s[i];
  it->x = x; 
  it->y = y; 
  it->w = ccfont_text_width8(fn, s, len);
  it->h = ccfont_height(fn);
  it->attr = attrib;
  it->fn = fn;
  if (attrib & CATTR_LNK) {
    it->color = ph->url_color;
    //    printf("ajout item CATTR_LNK\n");
  } else {
    it->color = color;
  }
  it->link_str = link;
  it->special_attr = special_attr;
  BLAHBLAH(5, myprintf("{%<CYA %s>,x=%d, y=%d, w=%<YEL %d>, h=%d}\n", 
		       it->s, it->x, it->y,it->w,it->h));
  it->next = NULL;
  return it;
}

#define NORMAL 1
#define CENTER 2
#define RIGHT  3
#define LEFT   4

/* justification d'une ligne (rustique) */
static void
//justif_ligne(PicoHtml *ph, PicoHtmlItem *it_debut_ligne, int xpos_debut_ligne, int xmax, int parag_align)
justif_ligne(PicoHtmlItem *it_debut_ligne, int xmax, int parag_align)
{
  PicoHtmlItem *it;
  int nb_word, xjust, xdec, incxdec, xcheck;

  if (it_debut_ligne == NULL) return;

  if (parag_align == LEFT) return;

  it = it_debut_ligne;
  if (it->next == NULL && parag_align == NORMAL) return; /* des fois, y'a pas d'espoir */
  nb_word = 0; 
  xjust = -1;

  /* comptage de l'espace a combler */
  while (it) { 
    if (it->next == NULL) 
      xjust = xmax - (it->x + it->w);
    it = it->next; 
    nb_word++; 
  }

  if (xjust < 0) xjust = 0; /* ça arrive si le mot est plus large que la fenetre et qu'on essaye
			       deseperement de le centrer ... */
  //  assert(xjust >= 0); 
  if (xjust == 0) return;

  /* justification à droite et à gauche */
  if (parag_align == NORMAL) {
    if (xjust / nb_word > 10) return; /* pas de justification quand ça créé trop d'espaces entre les mots */
    it = it_debut_ligne->next;
    xdec = 0; xcheck = 0;
    while (it) {
      incxdec = xjust / (nb_word-1);
      xjust -= incxdec;
      xdec += incxdec;
      it->x += xdec;
      nb_word--;
      xcheck = it->x + it->w;
      it = it->next;
    }
  //printf("xcheck = %d, xmax = %d\n", xcheck, xmax);
    assert(xcheck == xmax);
    assert(nb_word = 1);
  } else if (parag_align == CENTER) {
    it = it_debut_ligne;
    while (it) {
      it->x += xjust/2;
      it = it->next;
    }
  } else if (parag_align == RIGHT) {
    it = it_debut_ligne;
    while (it) {
      it->x += xjust;
      it = it->next;
    }
  }
}


/* construction du texte avec ses quelques attributs */
void
picohtml_parse(PicoHtml *ph, const char *buff, int width)
{
  #define MAX_TOK_LEN 500
  const unsigned char *p, *np;
  unsigned char tok[MAX_TOK_LEN];
  c_attr attrib;
  char *cur_link; /* pointe vers l'url courante (ou bien NULL) */
  int special_attr; /* un attribut particulier utilisé pour besoins internes (cf newswin) */

  /* positionne quand on change de paragraphe */
  int new_parag;

  /* valable pour la ligne en cours de traitement,
     utilises pour la justification finale */
  //int xpos_debut_ligne;
  PicoHtmlItem *it_debut_ligne;

  int htext, space_width, parag_skip, line_skip, parag_align, next_parag_align;
  int flag_debut_ligne, flag_item_to_add;
  int x,y,w;
  CCFontId cur_fn=(CCFontId)(-1);

  CCColorId cur_color;

  int isListe = 0;
  int isOrderedListe = 0;
  int indexListe = 0;
  int isBlockquote = 0;

  if (buff == NULL) {
    fprintf(stderr, _("Bug! calling picohtml(NULL)!"));
    buff = _("Bug ! appel de picohtml(NULL) !");
  } 

  p = buff;
  htext  = ccfont_height(ph->fn_base);

  space_width = ccfont_text_width8(ph->fn_base, "  ", 2);
  parag_skip = (int)(htext * ph->parag_fskip+.5);
  line_skip = (int)(htext * ph->line_fskip+.5);
  
  attrib = 0;

  ph->required_width = -1;

  new_parag = 1;
  parag_align = NORMAL; next_parag_align = NORMAL;
  flag_debut_ligne = 1; 
  it_debut_ligne = NULL;

  cur_link = NULL;
  ph->nb_links = 0;
  special_attr = 0;

  x = 0; //- parag_skip;
  y = 0; //parag_skip;

  //xpos_debut_ligne = x;
  cur_color = ph->default_color;
  while (*p) {
    flag_item_to_add = 0;

    if (get_tok(&p, &np, tok, MAX_TOK_LEN) == NULL) break;



     /* il est pas beau le parser d'html ? */
    if (strncasecmp(tok, "<br", 3) == 0) {
      new_parag = 1;
 /* Patch lordOric */			
    } else if (strcasecmp(tok, "<ol>") == 0) {
			y+=4; isOrderedListe = 1; indexListe = 0;
    } else if (strcasecmp(tok, "</ol>") == 0) {
      new_parag = 1; y+=4; isOrderedListe = 0;
    } else if (strcasecmp(tok, "<ul>") == 0) {
			y+=4; 
    } else if (strcasecmp(tok, "</ul>") == 0) {
      new_parag = 1; y+=4;
		} else if (strcasecmp(tok, "<li>") == 0 ) {
			new_parag = 1; isListe = 1;	indexListe++;
		} else if (strcasecmp(tok, "</li>") == 0 ) {
			isListe = 0;  
    } else if (strcasecmp(tok, "<blockquote>") == 0) {
      new_parag = 1; next_parag_align = NORMAL; isBlockquote = 1;
    } else if (strcasecmp(tok, "</blockquote>") == 0) {
      new_parag = 1; next_parag_align = NORMAL; y += 10; isBlockquote = 0;
/* Patch lordOric */
   } else if (strcasecmp(tok, "<p>") == 0) {
      new_parag = 1; next_parag_align = NORMAL; y += 10; 
    } else if (strcasecmp(tok, "</p>") == 0) {
      new_parag = 1; next_parag_align = NORMAL; y += 4;
    } else if (strcasecmp(tok, "<p align=center>") == 0) {
      new_parag = 1; next_parag_align = CENTER; 
      ph->required_width = width; // presence d'alignement => interdiction de renoyer la largeur minimale

    } else if (strcasecmp(tok, "<p align=right>") == 0) {
      new_parag = 1; next_parag_align = RIGHT;
      ph->required_width = width; // presence d'alignement => interdiction de renoyer la largeur minimale
    } else if (strncasecmp(tok, "<tab",4) == 0 && 
	       (tok[4]=='>' || (isdigit(tok[4]) && tok[5] == '>'))) { /* extension proprietaire ;-) */
      int n = (tok[4]=='>' ? 1 : tok[4]-'0');
      if (n) 
	x = ((x + (n*ph->tabul_skip))/(n*ph->tabul_skip))*(n*ph->tabul_skip);
      parag_align = LEFT;
    } else if (strncasecmp(tok, "<!special=", 10)==0) { /* encore une extension (pour phv_title de newswin.c) */
      special_attr = atoi(tok+10);
      //      printf("special_attr <- %d\n", special_attr);
    } else if (strncasecmp(tok, "<a", 2) == 0) {
      char *s1, *s2;
      s1 = tok;
      while (*s1 != '"' && *s1) s1++;
      if (*s1) {
	s1++;
	s2 = s1;
	while (*s2 != '"' && *s2) s2++;
	if (*s2 == '"') {
	  char *url;
	  *s2 = 0;
	  url = NULL;
	  if (strncasecmp(s1, "http://", 7) == 0 ||
	      strncasecmp(s1, "https://", 8) == 0 ||
	      strncasecmp(s1, "ftp://", 6) == 0) {
	    url = strdup(s1);
	  } else if (ph->url_path
		     && (strncmp(s1, "./",2) == 0 ||
			 strncmp(s1, "../",3) == 0)) {
	    url = str_printf("%s%s", ph->url_path, s1);
	  }
	  if (url) {
	    if (ph->nb_links < PH_MAX_LINKS) {
	      attrib |= CATTR_LNK;
	      ph->links_array[ph->nb_links] = url; url = NULL;
	      cur_link = ph->links_array[ph->nb_links];
	      ph->nb_links++;
	    }
	    COND_FREE(url);
	  }
	}
      }

    } else if (strncasecmp(tok, "</a", 3) == 0) {
      attrib &= ~CATTR_LNK;
      cur_link = NULL;
    } else if (strcasecmp(tok, "<b>") == 0 || strcasecmp(tok,"<strong>") == 0) {
      attrib |= CATTR_BOLD;
    } else if (strcasecmp(tok, "</b>") == 0 || strcasecmp(tok,"</strong>") == 0 ) {
      attrib &= ~CATTR_BOLD;
    } else if (strcasecmp(tok, "<i>") == 0 || strcasecmp(tok,"<em>") == 0) {
      attrib |= CATTR_ITAL;
    } else if (strcasecmp(tok, "</i>") == 0 || strcasecmp(tok,"</em>") == 0 ) {
      attrib &= ~CATTR_ITAL;
    } else if (strcasecmp(tok, "<tt>") == 0) {
      attrib |= CATTR_TT;
    } else if (strcasecmp(tok, "</tt>") == 0) {
      attrib &= ~CATTR_TT;
    } else if (strncasecmp(tok, "<font color=", 12) == 0) {
      char *col, *c2;
      //XColor screen_col, exact_col;
      col = tok; while (*col != '=' && *col) col++; if (*col) col++;
      c2 = col; while (*c2 != '>' && *c2) c2++;
      *c2 = 0;
      if (strlen(col)) {
	if ((cur_color = cccolor_from_name(col)) == (CCColorId)(-1)) {
	  BLAHBLAH(2,myprintf(_("Allocation of '%s' failed\n"), col));
          cur_color = ph->default_color;
        }
#if 0
        if (XAllocNamedColor(dock->display, DefaultColormap(dock->display, dock->screennum), 
			     col, &screen_col, &exact_col)) {
	  /* faudrait-il que je fasse des XFreeColor ?... */
	  BLAHBLAH(2,myprintf(_("Allocation of '%s' OK\n"), col));
	  cur_pixel_color = screen_col.pixel;
	} else {
	  BLAHBLAH(2,myprintf(_("Allocation of '%s' failed\n"), col));
	  cur_pixel_color = ph->default_pixel_color;
	}
#endif
      }
    } else if (strcasecmp(tok, "</font>") == 0) {
      cur_color = ph->default_color;
    } else if (strncasecmp(tok, "<img",4) == 0) {
      BLAHBLAH(1, myprintf(_("we forget '%<YEL %s>'\n"), tok));
      /*} else if (tok[0] == '<') {
      
      BLAHBLAH(1,myprintf("tag html non reconnu: '%<YEL %s>'\n", tok));
      */
    } else {
      flag_item_to_add = 1;
    }

    if (new_parag) {
      x = ph->parag_indent;
      y += parag_skip;
      //xpos_debut_ligne = x;
      if (parag_align != NORMAL) {
	//	justif_ligne(ph, it_debut_ligne, xpos_debut_ligne, width, parag_align);
	justif_ligne(it_debut_ligne, width, parag_align);
      }
      parag_align = next_parag_align;
      new_parag = 0;
      flag_debut_ligne = 1;
      if ( isListe )
        {
          char str[5];
          
          if ( isOrderedListe )
            /* je défie quiconque de poster une news avec une liste de plus de 99 éléments */
            snprintf(str,5,"%2d.", indexListe); 
          else
            strcpy(str,"-");
          
          it_debut_ligne = picohtml_additem(ph,str, strlen( str ), attrib, x, y, cur_fn, cur_color, cur_link, special_attr);
          flag_debut_ligne = 0;
          x+=ph->parag_indent;
        }
    }

    if (flag_item_to_add) {
      int len;
      if (attrib & CATTR_TT) cur_fn = ph->fn_mono;
      else if (attrib & CATTR_BOLD) cur_fn = ph->fn_bd;
      else if (attrib & CATTR_ITAL) cur_fn = ph->fn_it;
      else cur_fn = ph->fn_base;

      if (new_parag) {
	x = ph->parag_indent;
	y += parag_skip;
	//xpos_debut_ligne = x;
	parag_align = next_parag_align;
      }

      /* ca c'est recent (v2.2) et c'est Bien(tm) */
      len = convert_to_ascii(tok, tok, MAX_TOK_LEN);
      
      w = ccfont_text_width8(cur_fn, tok, len);
      
      if (x + w + (space_width)*(1-new_parag) >= width) {
	//justif_ligne(ph, it_debut_ligne, xpos_debut_ligne, width, parag_align);
	justif_ligne(it_debut_ligne, width, parag_align);
	if ( isBlockquote )
		x = ph->parag_indent;
	else
		x = 0;
	y += line_skip; 
	//xpos_debut_ligne = x;
	it_debut_ligne = picohtml_additem(ph, tok, len, attrib, x, y, cur_fn, cur_color, cur_link, special_attr);
	x += w;
      } else {
	PicoHtmlItem *it;
	x += (space_width)*(1-flag_debut_ligne);
	it = picohtml_additem(ph, tok, len, attrib, x, y, cur_fn, cur_color, cur_link, special_attr);
	if (flag_debut_ligne) it_debut_ligne = it;
	x += w;
      }
      flag_debut_ligne = 0;
    }


    p = np;
  }
}

/* renvoie la zone rectqngulqire, en pixel, englobant l'ensemble du
   texte -> utilise pour determiner la hauteur de la fenetre */
void
picohtml_gettxtextent(PicoHtml *ph, int *width, int *height)
{
  PicoHtmlItem *it;

  assert(width && height);
  it = ph->txt;
  *width = *height = 0;
  if (it) {
    *width = it->x + it->w;
    //    *height = it->y + it->h;
    *height = it->y;
    it = it->next;
  }
  while (it) {
    *width  = MAX(*width , it->x + it->w);
    //    *height = MAX(*height, it->y + it->h);
    *height = MAX(*height, it->y);
    it = it->next;
  }

  if (ph->required_width > 0) {
    *width = ph->required_width;
  }
}

/*
  affiche l'html dans le pixmap
*/
void
picohtml_render(PicoHtml *ph, Drawable d, int x, int y)
{
  PicoHtmlItem *it;
  
  it = ph->txt;
  while (it) {
    /*    XSetFont(dock->display, gc, it->fn->fid);
    XSetForeground(dock->display, gc, it->pixel);
    XDrawString(dock->display, d, gc, x + it->x, y + it->y, it->s, strlen(it->s));
    */
    ccfont_draw_string8(it->fn, it->color, d, x + it->x, y + it->y, it->s, -1);
    it = it->next;
  }
}

void
picohtml_freetxt(PicoHtml *ph)
{
  PicoHtmlItem *it, *nit;
  int i;

  it = ph->txt;
  while (it) {
    nit = it->next;
    free(it->s);
    free(it);
    it = nit;
  }
  ph->txt = NULL;
  for (i=0; i < ph->nb_links; i++) free(ph->links_array[i]);
}

int picohtml_isempty(PicoHtml *ph) {
  return (ph->txt == NULL);
}

void picohtml_set_parag_indent(PicoHtml *ph, int parag_indent) {
  ph->parag_indent = parag_indent;
}

/* interligne entre deux paragraphes, 1.0 = hauteur des car, 2.0 = double interligne etc.. */
void picohtml_set_parag_skip(PicoHtml *ph, float parag_skip) {
  ph->parag_fskip = parag_skip;
}

/* interligne, 1.0 = hauteur des car, 2.0 = double interligne etc.. */
void picohtml_set_line_skip(PicoHtml *ph, float line_skip) {
  ph->line_fskip = line_skip;
}

void picohtml_set_tabul_skip(PicoHtml *ph, int tabul_skip) {
  ph->tabul_skip = tabul_skip;
}

CCFontId picohtml_get_fn_base(PicoHtml *ph)
{
  return ph->fn_base;
}

CCFontId picohtml_get_fn_bold(PicoHtml *ph)
{
  return ph->fn_bd;
}

static
int
picohtml_try_loadfonts(PicoHtml *ph, char *fn_family, int fn_size)
{
  char base_name[512];
  char ital_name[512];
  char bold_name[512];
  char tt_name[512];

  snprintf(base_name, 512, "%s:pixelsize=%d", fn_family, fn_size);
  snprintf(ital_name, 512, "%s:pixelsize=%d:slant=italic,oblique", fn_family, fn_size);
  snprintf(bold_name, 512, "%s:pixelsize=%d:bold", fn_family, fn_size);
  snprintf(tt_name, 512, "%s:pixelsize=%d:monospace", fn_family, fn_size);
  ph->fn_base = ccfont_get(base_name);
  ph->fn_it = ccfont_get(ital_name);
  ph->fn_bd = ccfont_get(bold_name);
  ph->fn_mono = ccfont_get(tt_name);
  if (ph->fn_base == (CCFontId)(-1)) return -1;
  if (ph->fn_it == (CCFontId)(-1)) ph->fn_it = ccfont_incref(ph->fn_base);
  if (ph->fn_bd == (CCFontId)(-1)) ph->fn_bd = ccfont_incref(ph->fn_base);
  if (ph->fn_mono == (CCFontId)(-1)) ph->fn_mono = ccfont_incref(ph->fn_base);

#if 0
  /* police de base ... si on ne la trouve pas, c'est une erreur fatale */
  snprintf(base_name, 512, "-*-%s-medium-r-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, encoding);
  ph->fn_base = XLoadQueryFont(display, base_name);
  if (!ph->fn_base) {
    fprintf(stderr, _("XLoadQueryFont: failed loading font '%s'\n"), base_name);
    fprintf(stderr, _("Please choose another font.\n"));
    return -1;
  }

  /* police italique -> on cherche d'abord la police oblique */
  snprintf(ital_name, 512, "-*-%s-medium-o-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, encoding);
  ph->fn_it = XLoadQueryFont(display, ital_name);
  if (!ph->fn_it) {
    /* puis la police italique */
    BLAHBLAH(1, fprintf(stderr, _("Slanted font '%s' not found -> we're looking for the italic font\n"), ital_name));
    snprintf(ital_name, 512, "-*-%s-medium-i-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, encoding);
    ph->fn_it = XLoadQueryFont(display, ital_name);
    if (!ph->fn_it) {
      myfprintf(stderr, _("%<RED WARNING>: error while looking for the italic font: '%s'\n"), ital_name);
      myfprintf(stderr, _("We'll use the base font.\n"));

      /* pas de copie de pointer pour pas poser de pbs dans picohtml_destroy */
      ph->fn_it = XLoadQueryFont(display, base_name); assert(ph->fn_it);
    }
  }

  /* police bold */
  snprintf(bold_name, 512, "-*-%s-bold-r-*-*-%d-*-*-*-*-*-%s", fn_family, fn_size, encoding);
  ph->fn_bd = XLoadQueryFont(display, bold_name);
  if (!ph->fn_bd) {
    myfprintf(stderr, _("%<RED WARNING>: error while looking for the bold font: '%s'\n"), bold_name);
    myfprintf(stderr, _("We'll use the base font.\n"));
    ph->fn_bd = XLoadQueryFont(display, base_name); assert(ph->fn_bd);
  }

  /* police courier */
  snprintf(tt_name, 512, "-*-courier-medium-r-*-*-%d-*-*-*-*-*-%s", fn_size, encoding);
  ph->fn_mono = XLoadQueryFont(display, tt_name);
  if (!ph->fn_mono) {
    myfprintf(stderr, _("%<RED WARNING>: error while looking for the courier font: '%s'\n"), tt_name);
    myfprintf(stderr, _("We'll use the base font.\n"));
    ph->fn_mono = XLoadQueryFont(display, base_name); assert(ph->fn_mono);
  }
#endif
  return 0;
}

static void
picohtml_loadfonts(PicoHtml *ph, char *fn_family, int fn_size) {
  if (picohtml_try_loadfonts(ph,fn_family,fn_size)==-1) {
    myfprintf(stderr, _("Now we try sans/12\n"));
    if (picohtml_try_loadfonts(ph,"sans",12)==-1) {
      myfprintf(stderr, _("Uuuurg, no sans/12 , I prefer to die\n"));
      exit(-1);
    }
  }
}

PicoHtml *picohtml_create(Dock *dock, char *base_family, int base_size, int white_txt)
{
  PicoHtml *ph;

  ALLOC_OBJ(ph,PicoHtml);
  ph->txt = NULL;
  if (white_txt) {
    ph->default_color = cccolor_get(0xffffff); //WhitePixel(dock->display, dock->screennum);
  } else {
    ph->default_color = cccolor_get(0x000000); //BlackPixel(dock->display, dock->screennum);
  }
  ph->url_color = dock->blue_color;
  picohtml_loadfonts(ph, base_family, base_size);
  ph->parag_fskip = 1.3;
  ph->line_fskip = 1.0;
  ph->tabul_skip = 20;
  ph->parag_indent = 20;
  ph->nb_links = 0;
  ph->required_width = -1;
  ph->url_path = NULL;
  return ph;
}
/*
void
picohtml_set_default_pixel_color(PicoHtml *ph, unsigned long pix)
{
  ph->default_pixel_color = pix;
}
*/
void picohtml_destroy(PicoHtml *ph)
{
  ccfont_release(&ph->fn_base);
  ccfont_release(&ph->fn_it);
  ccfont_release(&ph->fn_bd);
  ccfont_release(&ph->fn_mono);
  cccolor_release(&ph->default_color);
  COND_FREE(ph->url_path);
  if (ph->txt) picohtml_freetxt(ph);
  free(ph);
}
