/*
    wmCoinCoin, the stupidest WindowMaker dock applet
    Copyright (C) 2001  Julien Pommier
    This is file (C) 2001 Gaël Le Mignot <kilobug@freesurf.fr>

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
  rcsid=$Id: regexp.h,v 1.8 2002/11/20 23:34:40 pouaite Exp $
  ChangeLog:
  $Log: regexp.h,v $
  Revision 1.8  2002/11/20 23:34:40  pouaite
  paf le patch, par lordOric

  Revision 1.7  2002/10/15 23:17:28  pouaite
  rustinage à la truelle

  Revision 1.6  2002/08/18 20:52:15  pouaite
  les locales des sites fonctionnent (c bon pour les news)

  Revision 1.5  2002/03/03 10:10:04  pouaite
  bugfixes divers et variés

  Revision 1.4  2001/12/17 00:18:04  pouaite
  changement du format du backend -> on utilise desormais le /backend.rdf

  Revision 1.3  2001/12/03 13:49:10  pouaite
  fix pour la compil sous cygwin

  Revision 1.2  2001/12/02 18:24:16  pouaite
  modif (virage) des regexp pour le texte des news, trop chiant à maintenir, du coup ça devrait marcher à nouveau sous bsd

*/

#ifndef COINCOIN_REGEXP_H
#define COINCOIN_REGEXP_H

#include <sys/types.h>
#include <regex.h>
#include <stdarg.h>
#include "prefs.h"
typedef enum { pat_xp, pat_votes, pat_xp_votes, pat_news, pat_news_no_sec } pat_type_t;

typedef struct _patterns_t
{
  pat_type_t id;
  const char *pattern;
  regex_t *regexp;
  const char *format;
} patterns_t;

#define NB_MAX_URL 512
typedef struct _news_extract_t {
  char *txt;
  char *date;
  char *auteur;
  char *section;
  int nb_url;
  char *url_tab[NB_MAX_URL];
  char *url_descr[NB_MAX_URL];
} news_extract_t;

/*
** Format can be:
**  d => decimal number (need int *)
**  s => string (need char **)
*/

extern patterns_t patterns[];
const char *site_locale_str(SitePrefs *sp, const char *s);
int regexp_extract(const char *str, pat_type_t pattern, ...);

/* et une fonction speciale qui n'utilise pas les regex.. */
void extract_news_txt_dacode14(SitePrefs *sp, const char *s, news_extract_t *extr);
void extract_news_txt_dacode2(SitePrefs *sp, const char *s, news_extract_t *extr);

/* et deux fonctions à la con qui peuvent etre utiles */
char *mystrndup(const char *s, int n);
char *after_substr(const char *s, const char *substr);

#endif
