/*
  rcsid=$Id: picohtml.h,v 1.6 2004/02/29 19:01:27 pouaite Exp $
  ChangeLog:
  $Log: picohtml.h,v $
  Revision 1.6  2004/02/29 19:01:27  pouaite
  et hop

  Revision 1.5  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.4  2002/10/15 23:17:28  pouaite
  rustinage à la truelle

  Revision 1.3  2002/01/18 00:28:42  pouaite
  le ménage continue + grosses modifs (experimentales pour l'instant)

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

*/

#ifndef PICOHTML_H__
#define PICOHTML_H__

#include "coincoin.h"
#include "fontcoincoin.h"

#define PH_MAX_LINKS 100

#define CATTR_LNK    (1<<1)
#define CATTR_BOLD   (1<<2)
#define CATTR_ITAL   (1<<3)
#define CATTR_TT     (1<<4)

typedef unsigned char c_attr;

struct _PicoHtmlItem {
  unsigned char *s;
  short x, y, w, h;
  c_attr attr;
  PicoHtmlItem *next;
  CCFontId fn;
  char *link_str; /* pointe (si CATTR_LNK positionné) vers une entree de links_array */
  int special_attr; /* un attribut remplit avec le tag personnel: <!special=xxx> (utilisé pour reconnaitre les news_id dans la liste de titres de newswin) */
  CCColorId color;
};

struct _PicoHtml {
  PicoHtmlItem *txt;
  CCFontId fn_base, fn_it, fn_bd, fn_mono;
  //XFontStruct *fn_base, *fn_it, *fn_bd, *fn_mono;
  CCColorId default_color;
  CCColorId url_color;
  float parag_fskip, line_fskip;
  int parag_indent, tabul_skip;

  int required_width; /* si > 0, on ne doit pas reduire la largeur
			 c'est le cas quand certains elements sont centrés,
		        dans ce cas, required width == la width demandee */

  char *url_path; /* url de base (pour les liens relatifs) */
  char *links_array[PH_MAX_LINKS]; /* liste des urls citées */
  int nb_links;
};

#endif
