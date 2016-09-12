/*
  rcsid=$Id: coin_util.c,v 1.43 2005/09/25 12:08:55 pouaite Exp $
  ChangeLog:
  $Log: coin_util.c,v $
  Revision 1.43  2005/09/25 12:08:55  pouaite
  ca marche encore ca ?

  Revision 1.42  2004/04/28 22:19:00  pouaite
  bugfixes dae + des trucs que j'ai oublie

  Revision 1.41  2004/04/18 15:37:28  pouaite
  un deux un deux

  Revision 1.40  2004/03/07 13:51:12  pouaite
  commit du dimanche

  Revision 1.39  2004/03/03 23:00:39  pouaite
  commit du soir

  Revision 1.38  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.37  2003/08/26 21:50:48  pouaite
  2.6.4b au mastic

  Revision 1.36  2003/06/29 23:58:38  pouaite
  suppression de l'overrideredirect du palmi et ajout de pinnipede_totoz.c et wmcoincoin-totoz-get etc

  Revision 1.35  2003/06/09 16:42:29  pouaite
  pan pan

  Revision 1.34  2003/03/01 17:31:22  pouaite
  compat ipv6 a tester

  Revision 1.33  2003/02/26 00:03:19  pouaite
  fix bug des urls relatives (pour la caverne)

  Revision 1.32  2002/10/16 20:41:45  pouaite
  killall toto

  Revision 1.31  2002/08/21 20:22:16  pouaite
  fix compil

  Revision 1.30  2002/08/18 19:00:28  pouaite
  plop

  Revision 1.29  2002/08/18 00:29:30  pouaite
  en travaux .. prière de porter le casque

  Revision 1.28  2002/06/23 22:26:01  pouaite
  bugfixes+support à deux francs des visuals pseudocolor

  Revision 1.27  2002/06/23 14:01:36  pouaite
  ouups, j'avais flingué les modifs depuis la v2.3.8b

  Revision 1.26  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.24  2002/06/01 17:54:04  pouaite
  nettoyage

  Revision 1.23  2002/05/12 22:06:27  pouaite
  grosses modifs dans http.c

  Revision 1.22  2002/04/13 11:55:19  pouaite
  fix kde3 + deux trois conneries

  Revision 1.21  2002/04/10 18:13:33  pouaite
  bugfix ppc

  Revision 1.20  2002/04/01 22:56:03  pouaite
  la pseudo-transparence du pinni, bugfixes divers, option tribune.backend_type

  Revision 1.19  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

  Revision 1.18  2002/03/27 23:27:10  pouaite
  tjs des bugfixes (pour gerer des posts qui peuvent atteindre 10ko !), en parallele de la v2.3.6-5

  Revision 1.17  2002/03/27 21:22:29  pouaite
  microbugfix

  Revision 1.16  2002/03/27 20:45:06  pouaite
  deuxième vague de bugfix

  Revision 1.15  2002/03/27 19:02:04  pouaite
  bugfix pour le nouveau format du backend

  Revision 1.14  2002/03/21 22:53:07  pouaite
  ajout d'une icone pour la fenetre du pinnipede et des news

  Revision 1.13  2002/03/03 10:10:04  pouaite
  bugfixes divers et variés

  Revision 1.12  2002/03/01 00:27:40  pouaite
  trois fois rien

  Revision 1.11  2002/02/27 00:32:19  pouaite
  modifs velues

  Revision 1.10  2002/02/24 22:13:56  pouaite
  modifs pour la v2.3.5 (selection, scrollcoin, plopification, bugfixes)

  Revision 1.9  2002/01/16 21:27:35  pouaite
  gros coup de balai dans wmcoincoin.c qui s'est du coup splitté en trois: wmcoincoin.c, dock.c et useragents_file.c

  Revision 1.8  2002/01/16 00:35:26  pouaite
  debut de detection des reponse à nos message avec des couleurs hideuses et certainement plein de bugs moisis

  Revision 1.7  2002/01/14 23:54:06  pouaite
  reconnaissance des posts effectué par l'utilisateur du canard (à suivre...)

  Revision 1.6  2002/01/13 20:02:51  pouaite
  j'ai honte

  Revision 1.5  2002/01/13 15:19:00  pouaite
  double patch: shift -> tribune.post_cmd et lordOric -> tribune.archive

  Revision 1.4  2002/01/12 17:29:08  pouaite
  support de l'iso8859-15 (euro..)

  Revision 1.3  2001/12/21 12:21:23  pouaite
  pico bugfix

  Revision 1.2  2001/12/02 18:29:46  pouaite
  à la demande des décideurs de tous poils, gestion (toute naze...) de l'EURO !

*/
#include "config.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <iconv.h>
#include <errno.h>
#ifdef HAVE_BACKTRACE
# include <execinfo.h>
#endif
#include "coin_util.h"

#include <libintl.h>
#define _(String) gettext(String)
#include "myprintf.h"

/* construit un 'nom' à partir des premiers mots du useragent */

void
make_short_name_from_ua(const unsigned char *ua, unsigned char *name, int name_sz)
{
  if (ua == NULL || strlen(ua) == 0) {
    strncpy(name, _("[empty]"), name_sz); name[name_sz-1] = 0;
    return;
  }
  strncpy(name, ua, name_sz); name[name_sz-1] = 0;
  if ((int)strlen(name) >= name_sz-1) {
    int i;

    // essaye de ramener l'useragent à une longueur correcte sans tronquer de mot 
    i = name_sz-2;
    
    // cas facile: des espaces 
    if (name[i] == ' ') {
      while (name[i] == ' ' && i) i--;
      name[i+1] = 0;
    } else {
      while (i && isalnum((unsigned char)name[i])) {
	i--; 
      }
      while (i>1 && !isalnum((unsigned char)name[i-1])) {
	i--;
      }
      if (i > 5) {
	name[i] = 0;
      }
    }
  }
}


/*
  with_bug_amp : le remote.rdf a été deux fois interprete par une moulinette,
   resultat , les 'é' soit devenus des '&eacute;' puis des '&amp;eacute;'

   remarque: on peut avoir dest == src, on utilise alors une chaine temporaire

   renvoie la longueur de la chaine apres conversion

   cette fonction est utilisee par picohtml.c et coincoin_tribune.c
*/
int
convert_to_ascii(char *dest, const char *_src, int dest_sz)
{
  int id, is;

  const char *src;
  static const struct {
    char *sign;
    char *c;
    int num;
  } tab[] = {{"amp;", "&", 0},
	     {"quot;", "\"", 8220},
             {"quot;", "\"", 8221},
             {"apos;","'",8217},
	     {"gt;", ">", 0},
	     {"lt;", "<", 0},
	     {"acute;", "\'", 0},
	     {"nbsp;"  , " ", 0},  // 0xa0 /* ouais bon, du coup il va etre breakable l'espace.. */
	     {"iexcl;" , "¡", 0},
	     {"cent;"  , "¢", 0},
             {"pound;" , "£", 0},
             {"curren;", "¤", 0},  // 0xa4
	     {"yen;"   , "¥", 0},
	     {"brvbar;", "¦", 0},
	     {"sect;"  , "§", 0},
             {"uml;"   , "¨", 0},
	     {"copy;"  , "©", 0},
	     {"ordf;"  , "ª", 0},
	     {"laquo;" , "«", 0},
	     {"not;"   , "¬", 0},
	     {"shy;"   , "­", 0},
	     {"reg;"   , "®", 0},
	     {"macr;"  , "¯", 0},
	     {"deg;"   , "°", 0}, // 0xb0
	     {"plusmn;", "±", 0},
	     {"sup2;"  , "²", 0},
	     {"sup3;"  , "³", 0},
	     {"acute;" , "´", 0},
	     {"micro;" , "µ", 0},
	     {"para;"  , "¶", 0},
	     {"middot;", "·", 0},
             {"cedil;" , "¸", 0},
	     {"sup1;"  , "¹", 0},
	     {"ordm;"  , "º", 0},
	     {"raquo;", "»", 0},

	     {"frac14;", "¼", 0},
	     {"frac12;", "½", 0},
	     {"frac34;", "¾", 0},
	     {"iquest;", "¿", 0},

	     {"Agrave;", "À", 0}, // 0xc0
	     {"Aacute;", "Á", 0},
	     {"Acirc;" , "Â", 0},
	     {"Atilde;", "Ã", 0},
	     {"Auml;"  , "Ä", 0},
	     {"Aring;" , "Å", 0},
	     {"AElig;" , "Æ", 0},
	     {"Ccedil;", "Ç", 0},
	     {"Egrave;", "È", 0},
	     {"Eacute;", "É", 0},
	     {"Ecirc;" , "Ê", 0},
	     {"Euml;"  , "Ë", 0}, 
	     {"Igrave;", "Ì", 0},
	     {"Iacute;", "Í", 0},
	     {"Icirc;" , "Î", 0},
	     {"Iuml;"  , "Ï", 0},
	     {"ETH;"   , "Ð", 0}, // 0xd0
	     {"Ntilde;", "Ñ", 0},
	     {"Ograve;", "Ò", 0},
	     {"Oacute;", "Ó", 0},
	     {"Ocirc;" , "Ô", 0},
	     {"Otilde;", "Õ", 0},
	     {"Ouml;"  , "Ö", 0},
	     {"times;" , "×", 0},
	     {"Oslash;", "Ø", 0},
	     {"Ugrave;", "Ù", 0},
	     {"Uacute;", "Ú", 0},
	     {"Ucirc;" , "Û", 0},
	     {"Uuml;"  , "Ü", 0},
	     {"Yacute;", "Ý", 0},
	     {"THORN;" , "Þ", 0},
	     {"szlig;" , "ß", 0},
	     {"agrave;", "à", 0}, // 0xe0
	     {"aacute;", "á", 0},
	     {"acirc;" , "â", 0},
	     {"atilde;", "ã", 0},
	     {"auml;"  , "ä", 0},
	     {"aring;" , "å", 0},
	     {"aelig;" , "æ", 0},
	     {"ccedil;", "ç", 0},
	     {"egrave;", "è", 0},
	     {"eacute;", "é", 0},
	     {"ecirc;" , "ê", 0},
	     {"euml;"  , "ë", 0},
	     {"igrave;", "ì", 0},
	     {"iacute;", "í", 0},
	     {"icirc;" , "î", 0},
	     {"iuml;"  , "ï", 0},
	     {"eth;"   , "ð", 0}, // 0xf0
	     {"ntilde;", "ñ", 0},
	     {"ograve;", "ò", 0},
	     {"oacute;", "ó", 0},
	     {"ocirc;" , "ô", 0},
	     {"otilde;", "õ", 0},
	     {"ouml;"  , "ö", 0},
	     {"divide;", "÷", 0},
	     {"oslash;", "ø", 0},
	     {"uacute;", "ú", 0},
	     {"ugrave;", "ù", 0},
	     {"ucirc;" , "û", 0},
	     {"uuml;"  , "ü", 0}, 
	     {"yacute;", "ý", 0}, // 0xfd
	     {"thorn;" , "þ", 0}, // 0xfe
	     {"yuml;"  , "ÿ", 0}, // 0xff
             // les horreurs ms
	     //   {"Scaron;",{352,0}},
	     //	     {"scaron;",{353,0}},
	     {"trade;", "(tm)", 153}, // non iso8859-1
	     {"euro;", "¤", 8364},   // il faut iso8859-15 pour que ça fasse le bon char
	     {"OElig;" , "OE", 140},
	     {"oelig;" , "oe", 156},
             {"ldots", "...", 133},
             {"hellip", "...", 133},
             {"bull", "*", 149},
             {"ndash", "--", 150},
             {"endash", "--", 150},
             {"mdash", "--", 151},
             {"emdash", "--", 151},
	     {NULL, "*", 0}};


  /* detection du cas ou les chaines se supperposent */
  if (_src == dest ) {
    src = strdup(_src); assert(src);
  } else {
    src = _src;
  }

  id = 0; is = 0;
  while (id < dest_sz-1 && src[is]) {
    if (src[is] == '&') {
      int i, found;
      i = 0; found = -1;
      if (src[is+1] == '#' && src[is+2] >= '0' && src[is+2] <= '9') {
        long n; char *end;
        n = strtol(src+is+2, &end, 10);
        if (n >= 32) {
          while (tab[i].sign) {
            if (tab[i].num && tab[i].num == n) {
              is += strlen(tab[i].sign)+1;
              found = i;
              break;
            }
            i++;
          }
          if (found == -1 && n < 256) {
            dest[id++] = (unsigned char)n; 
            is += (end - (src+is)); 
            if (src[is] == ';') ++is;
            found = 1;
          } else {
            int j;
            j = 0;
            while (id < dest_sz-1 && tab[i].c[j]) {
              dest[id++] = tab[i].c[j++];
            }
            is += (end - (src+is));if (src[is] == ';') ++is;
          }
        }
      }
      if (found == -1) {
        while (tab[i].sign) {
          if (strncmp(tab[i].sign, src+is+1, strlen(tab[i].sign))==0) {
            is += strlen(tab[i].sign)+1;
            found = i;
            break;
          }
          i++;
        }
        if (found == -1) {
          dest[id++] = '&'; is++;
        } else {
          int j;
          j = 0;
          while (id < dest_sz-1 && tab[i].c[j]) {
            dest[id++] = tab[i].c[j++];
          }
        }
      }
    } else if ((unsigned char)src[is] == 0x80 && id < dest_sz-2) { // cas particulier pour l'odieux EURO (encodage windows) 
      dest[id++] = '¤';
      is++;
    } else {
      dest[id] = src[is];
      id++; is++;
    }
  }
  dest[id] = 0;

  if (_src == dest) {
    free((char*)src);
  }
  return id;
}

int
mark_html_tags(char *s, int sz) {
  char *s2 = strdup(s);
  int i,j;

  for (i=0,j=0; i < sz && s2[i]; i++) {
    if ((s2[i] == '<' || s2[i] == '>') && j < sz-2) {
      s[j++] = '\t';
      s[j++] = s2[i];
    } else if (j < sz-1) {
      s[j++] = s2[i];
    }
  }
  assert(j < sz); assert(i <= sz);
  s[j] = 0;
  free(s2);
  return j;
}


/*
  verifie si la chaine est vide (cad si elle ne contient que des caractères non imprimables 
*/
int
str_is_empty(const char *s) {
  int i;
  if (s == NULL) return 1;
  else if (strlen(s) == 0) return 1;
  else {
    i = 0;
    while (s[i] && s[i] <= ' ') i++;
    return (i == (int)strlen(s));
  }
}

int xstrcmp(const char *a, const char *b) {
  if (str_is_empty(a) && str_is_empty(b)) return 0;
  else if (str_is_empty(a)) return -1;
  else if (str_is_empty(b)) return +1;
  else return strcmp(a,b);
}

/* recherche la première occurence d'une des chaines 'keys' dans 'src' et renvoie un pointeur vers
   cette occurence, ainsi que le numéro de la 'keys' trouvée

   bien sûr c'est pas optimal du tout, mais ON S'EN FOUT(tm)

   et oui, effectivement, 'str_multi_str' est un nom à la con
*/
char *
str_multi_str(const char *src, const char **keys, int nb_keys, int *key_idx)
{
  int i;
  const char *res;

  assert(key_idx);
  *key_idx = 0;
  res = NULL;
  for (i=0; i < nb_keys; i++) {
    const char *p;
    p = strstr(src, keys[i]);
    if (p && (res==NULL || p < res)) { res = p; *key_idx = i; }
  }
  return (char*)res;
}

/* renvoie une chaine (allouée correctement) contenant la substitution de toutes les occurences de
   'key' dans 'src' par 'substitution' (key et substition sont des tableaux de chaines de
   caractères, car pour faire plusieurs substitutions, mieux vaut les effectuer simultanement que
   les enchainer pour eviter les effets de bords
*/
char *
str_multi_substitute(const char *src, const char **keys, const char **substitutions, int nkeys)
{
  const char *p, *p_key;
  char *dest, *p_dest;
  int dest_sz, p_len,j;

  if (src == NULL) return NULL;

  /* calcul de la longueur de la destination.. */
  p = src;
  dest_sz = strlen(src)+1;

  while ((p_key=str_multi_str(p, keys, nkeys, &j))) {
    dest_sz += (strlen(substitutions[j]) - strlen(keys[j]));
    p = p_key+strlen(keys[j]);
  }

  dest = malloc(dest_sz);

  /* et là PAF ! */
  p = src;
  p_dest = dest;
  while ((p_key=str_multi_str(p, keys, nkeys, &j))) {
    memcpy(p_dest, p, p_key-p);
    p_dest += p_key-p;
    memcpy(p_dest, substitutions[j], strlen(substitutions[j]));
    p_dest += strlen(substitutions[j]);
    p = p_key + strlen(keys[j]);
  }
  p_len = strlen(p);
  if (p_len) {
    memcpy(p_dest, p, p_len); p_dest += p_len;
  }
  *p_dest = 0;
  assert(p_dest - dest == dest_sz-1); /* capote à bugs */
  return dest;
}

char *
str_substitute(const char *src, const char *key, const char *substitution) {
  return str_multi_substitute(src, &key, &substitution, 1);
}

/* quotage pour les commandes externes.. à priori c'est comme pour open_url
   mais bon.. je me refuse à donner la moindre garantie sur la sécurité 

   be aware
*/
char *
shell_quote(const char *src)
{
  char *quote = "&;`'\\\"|*?~<>^()[]{}$";
  int i,dest_sz;
  const unsigned char *p;
  char *dest;

  if (src == NULL || strlen(src) == 0) return strdup("");

  dest_sz = strlen(src)+1;
  for (p=src; *p; p++) {
    if (strchr(quote, *p) || *p <= ' ') dest_sz+=1;
  }
  dest = malloc(dest_sz);

  for (p=src, i=0; *p; p++) {
    if (strchr(quote, *p) || *p <= ' ') {
      dest[i++] = '\\';
    }
    if (*p <= ' ') {
      dest[i++] = ' ';
    } else {
      dest[i++] = *p;
    }
  }
  dest[i] = 0;
  assert(i == dest_sz-1); /* kapeaute à beugue */
  return dest;
}

/* 
   fonction de hachage à la con (vraiment!) 
*/

int
str_hache(const unsigned char *s, int max_len)
{
  unsigned char v[4];
  const unsigned char *p;
  int i, j;

  assert(s);
  if (max_len == -1) max_len = strlen(s);
  v[0] = 0xAB; v[1] = 0x13; v[2] = 0x9A; v[3] = 0x12;
  p = s;
  for (i=0, j=0; i < max_len && s[i]; i++) {
    unsigned char c;
    c = ((p[i])<<j) + ((p[i]) >> (8-j));
    v[j] ^= c;
    j++; if (j == 4) j = 0;
  }
  return CVINT(v[0],v[1],v[2],v[3]);
}

int
str_hache_nocase(const unsigned char *s, int max_len)
{
  unsigned char v[4];
  const unsigned char *p;
  int i, j;

  assert(s);
  v[0] = 0xAB; v[1] = 0x13; v[2] = 0x9A; v[3] = 0x12;
  p = s;
  for (i=0, j=0; i < max_len && s[i]; i++) {
    unsigned char c,d;
    d = tolower(p[i]);
    c = ((d)<<j) + ((d) >> (8-j));
    v[j] ^= c;
    j++; if (j == 4) j = 0;
  }
  return CVINT(v[0],v[1],v[2],v[3]);
}

void
md5_digest(const char *s, md5_byte_t md5[16]) {
  md5_state_t ms;
  md5_init(&ms); md5_append(&ms,s,strlen(s)); md5_finish(&ms,md5);
}

/* en minuscule ou on pete le APOP */
const char *
md5txt(md5_byte_t md5[16]) {         
  static char md5txt[32];
  int i;
  for (i = 0; i < 16; ++i) {
    md5txt[2*i  ] = "0123456789abcdef"[md5[i]/16];
    md5txt[2*i+1] = "0123456789abcdef"[md5[i]%16];
  }
  return md5txt;
}


unsigned char char_trans[256];
static int char_trans_init = 0;

static void 
init_char_trans()
{
  unsigned char *trans_accents  = "éèëêÊËÉÈàâáäÀÂÁÄûüùÙçÇîïíìÏÎÍÌôóòõÔÓÒÕñ";
  unsigned char *trans_accents2 = "eeeeeeeeaaaaaaaauuuucciiiiiiiioooooooon";
  int i;

  for (i=0; i < 256; i++) {
    unsigned char *p;
    if ((p=strchr(trans_accents, i))) {
      char_trans[i] = trans_accents2[(p - trans_accents)];
      } else if (i < (unsigned char)'A' || i > (unsigned char)'Z') {
	char_trans[i] = i;
      } else {
	char_trans[i] = i + 'a' - 'A';
      }
  }
  char_trans_init = 1;
}

unsigned char
chr_noaccent_tolower(unsigned char c)
{
  if (char_trans_init == 0) init_char_trans();
  return char_trans[c];
}

void
str_noaccent_tolower(unsigned char *s)
{
  int i;
  if (s == NULL) return;
  if (char_trans_init == 0) init_char_trans();
  i = 0; while(s[i]) {
    s[i] = char_trans[s[i]]; i++;
  }
}

unsigned char *
str_noaccent_casestr(const unsigned char *meule, const unsigned char *aiguille)
{
  unsigned char *res;
  unsigned char *m = strdup(meule);
  unsigned char *a = strdup(aiguille);
  int pos = -1;
  str_noaccent_tolower(m);
  str_noaccent_tolower(a);
  res = strstr(m, a); if (res) pos = res-m;
  free(a); free(m);
  return ((pos >= 0) ? (unsigned char*)meule+pos : NULL);
}

unsigned char *
str_case_str(const unsigned char *meule, const unsigned char *aiguille) {
  unsigned char c0 = tolower(aiguille[0]);
  assert(meule); assert(aiguille);
  if (aiguille[0] == 0) return (unsigned char*)meule;
  do {
    while (*meule && tolower(*meule) != c0) ++meule;
    if (*meule) {
      int i=1;
      for (i = 1; aiguille[i] && meule[i]; ++i) {
        if (tolower(aiguille[i]) != tolower(meule[i])) break;
      }
      if (aiguille[i] == 0) return (unsigned char*)meule;
      else ++meule;
    }
  } while (*meule);
  return NULL;
}

int str_ncasecmp(const unsigned char *a, const unsigned char *b, unsigned n) {
  assert(a); assert(b); assert(n < 10000000);
  while (n && *a && *b) {
    if (tolower(*a) < tolower(*b)) return -1;
    else if (tolower(*a) > tolower(*b)) return +1;
    --n; ++a; ++b;
  }
  if (*a == 0 && *b) return -1;
  else if (*b && *a == 0) return +1;
  else return 0;
}

int str_case_startswith(const unsigned char *a, const unsigned char *b) {
  return (str_ncasecmp(a,b,strlen(b)) == 0 && strlen(a) >= strlen(b));
}

int str_startswith(const unsigned char *a, const unsigned char *b) {
  return (strncmp(a,b,strlen(b)) == 0 && strlen(a) >= strlen(b));
}

void
str_tolower(unsigned char *s)
{
  if (s == NULL) return;
  for (; *s; s++) if (*s >= 'A' && *s <= 'Z') *s = *s - 'A' + 'a';
}

/* un printf pas très fin, mais avec allocation dynamique..
   c'est pratique ces ptites choses */
char *
str_printf(const char *fmt, ...)
{
  va_list ap;
  char *s;
  int s_sz;

  s_sz = 100;
  s = malloc(s_sz); assert(s);
  while (1) {
    int ret;
    va_start(ap, fmt);
    ret = vsnprintf(s, s_sz, fmt, ap);
    va_end(ap);
    if (ret == -1 || ret >= s_sz-1) {
      s_sz *= 2;
      assert(s_sz < 100000);
      s = realloc(s, s_sz); assert(s);
    } else 
      break;
  }
  s = realloc(s, strlen(s)+1); assert(s);
  return s;
}

char *
str_cat_printf(char *in_s, const char *fmt, ...)
{
  va_list ap;
  char *s, *out_s;
  int s_sz;

  s_sz = 100;
  s = malloc(s_sz); assert(s);
  while (1) {
    int ret;
    va_start(ap, fmt);
    ret = vsnprintf(s, s_sz, fmt, ap);
    va_end(ap);
    if (ret == -1 || ret >= s_sz-1) {
      s_sz *= 2;
      assert(s_sz < 100000);
      s = realloc(s, s_sz); assert(s);
    } else 
      break;
  }
  
  out_s = malloc( (in_s ? strlen(in_s) : 0) + strlen(s) + 1); 
  assert(out_s); out_s[0] = 0;
  if (in_s) { strcpy(out_s, in_s); free(in_s); }
  strcat(out_s, s);
  free(s);
  return out_s;
}


/* lecture d'une ligne d'un fichier, avec allocation dynamique */
char *
str_fget_line(FILE *f)
{
  int i,c;
  char *s;
  int s_sz;

  s_sz = 100; s = malloc(s_sz); assert(s);
  i = 0;
  while ((c = fgetc(f)) > 0) {
    if (c >= ' ' || c == '\t') {
      s[i++] = c;
      if (i == s_sz) { 
	s_sz *= 2; assert(s_sz < 100000);
	s = realloc(s, s_sz); assert(s);
      }
    }
    if (c == '\n') break;
  }
  s[i] = 0; assert(i < s_sz);
  s = realloc(s, strlen(s)+1); assert(s);
  return s;
}

/* enleve les espaces au début et à la fin */
void
str_trim(unsigned char *s) {
  int i,j;

  if (s == NULL) return;
  j = strlen(s)-1;
  while (j>=0 && s[j] <= ' ') s[j--] = 0;

  i = 0; 
  while (s[i] && s[i] <= ' ') i++;
  if (i<=j) {
    memmove(s, s+i, j+2-i);
  }
}

void
str_rtrim_lf(unsigned char *s) {
  int j;
  if (s == NULL) return;
  j = strlen(s)-1;
  while (j>=0 && s[j] < ' ') s[j--] = 0;
}

char *
str_preencode_for_http(const char *in)
{
  const char *keys[] = {" ", "+"  , ":"  ,"@"  };
  const char *subs[] = {"+", "%2B", "%3A","%40"};
  char *s;
  
  if (in == NULL) return NULL;
  s = str_multi_substitute(in, keys, subs, 2);
  return s;
}

char *
str_ndup(const char *in, int n)
{
  int l;
  char *s; 
  assert(in); assert(n>=0);
  l = MIN(strlen(in), (unsigned)n);
  s = malloc(l+1); assert(s);
  strncpy(s, in, l);
  s[l] = 0;
  return s;
}

char *str_dup_unquoted(const char *s) {
  int len;
  if (s == NULL) return NULL;
  len = strlen(s);
  if (len < 2 || (s[0] != '"' && s[0] != '\'')) return strdup(s);
  if (s[0] == '"' && s[len-1] == '"') return str_ndup(s+1,len-2);
  if (s[0] == '\'' && s[len-1] == '\'') return str_ndup(s+1,len-2);
  return strdup(s);
}

char*
str_cat(char *s1, const char *s2)
{
  int l1;
  int l2;
  char *s; 
  if (s2 == NULL) return s1;
  if (s1 == NULL) return strdup(s2);
    
  l1 = strlen(s1);
  l2 = strlen(s2);
  s = malloc(l1+l2+1); assert(s);
  strcpy(s, s1);
  strcpy(s+l1,s2);
  free(s1);
  return s;
}

char*
str_ncat(char *s1, const char *s2, int n)
{
  int l1;
  int l2;
  char *s;
  if (s2 == NULL) return s1;
  if (s1 == NULL) return str_ndup(s2,n);
  assert(n>=0);
  l1 = strlen(s1);
  l2 = MIN((int)strlen(s2),n);
  s = malloc(l1+l2+1); assert(s);
  strcpy(s, s1);
  strncpy(s+l1,s2,l2); s[l1+l2]=0;
  free(s1);
  return s;
}

void
str_trunc_nice(char *s, int approx_max_len) {
  if (s && (int)strlen(s) > approx_max_len - 20) {
    int i=approx_max_len - 20; 
    while (s[i] && !isspace(s[i]) && i < approx_max_len+30) ++i;
    if (s[i] && i < (int)(strlen(s) - 10)) { s[i] = 0; strcat(s, "(...)"); }
  }
}

/* la feinte: extern timezone n'est:
     - pas portable sur bsd
     - ne prend pas en compte le daylight saving, i.e. en été en france, ça chie
   -> pour chopper le vrai decalage par rapport au gmt, il faut utiliser localtime, 
   mettre tm_isdst à zero et reappeller mktime
*/ 
time_t get_timezone() {
  static int isinit = 0;
  static time_t t;
  if (!isinit) {
    isinit = 1;
    time_t t0 = time(NULL);
    struct tm *tt;
    tt = gmtime(&t0); tt->tm_isdst = 0;
    t = mktime(tt);
    tt = localtime(&t0); tt->tm_isdst = 0;
    t -= mktime(tt);
  }
  return t;
}

/* une fonction qui n'en veut */
int
str_to_time_t(const char *s, time_t *tt) {
  char sday[10], smon[10], stz[20], *stzdg = NULL;
  float fracsec = 0;
  int apply_tzshift = 1;
  int tzshift_h = 0, tzshift_m = 0, tz_sgn = +1;
  int confidence = 0; /* pour les dates qui ne contiennent pas les hh:mm:ss, on ajoute systematiquement 24 h */
  struct tm t;
  int ok = 0;

  sday[0] = smon[0] = stz[0] = 0;
  memset(&t, 0, sizeof(t));
  
  if (sscanf(s, "%4d%2d%2d%2d%2d%2d", &t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec) == 6) {
    ok = 1; apply_tzshift = 0;
  }
  /* test format RFC-822 */
  if (!ok && (sscanf(s,"%10s %d %10s %d %d:%d:%d %s", 
                     sday, &t.tm_mday, smon, &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec, stz) >= 7 ||
              sscanf(s,"%10s %d %10s %d %d:%d %s", 
                     sday, &t.tm_mday, smon, &t.tm_year, &t.tm_hour, &t.tm_min, stz) >= 6 ||
              sscanf(s,"%d %10s %d %d:%d:%d %s", /* format trouvé dans certains mails */
                     &t.tm_mday, smon, &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec, stz) >= 6)) {
    /*
      date-time   =  [ day "," ] date time        ; dd mm yy
                                                 ;  hh:mm:ss zzz

     day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"
                 /  "Fri"  / "Sat" /  "Sun"

     date        =  1*2DIGIT month 2DIGIT        ; day month year
                                                 ;  e.g. 20 Jun 82

     month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"
                 /  "May"  /  "Jun" /  "Jul"  /  "Aug"
                 /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"

     time        =  hour zone                    ; ANSI and Military

     hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
                                                 ; 00:00:00 - 23:59:59

     zone        =  "UT"  / "GMT"                ; Universal Time
                                                 ; North American : UT
                 /  "EST" / "EDT"                ;  Eastern:  - 5/ - 4
                 /  "CST" / "CDT"                ;  Central:  - 6/ - 5
                 /  "MST" / "MDT"                ;  Mountain: - 7/ - 6
                 /  "PST" / "PDT"                ;  Pacific:  - 8/ - 7
                 /  1ALPHA                       ; Military: Z = UT;
                                                 ;  A:-1; (J not used)
                                                 ;  M:-12; N:+1; Y:+12
                 / ( ("+" / "-") 4DIGIT )        ; Local differential
                                                 ;  hours+min. (HHMM)
    */
    static char *monthnames[12] = {"Jan",  "Feb",  "Mar",  "Apr",  "May", "Jun", 
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    if (strlen(smon) && smon[strlen(smon)-1] == ',') smon[strlen(smon)-1] = 0;
    int i; for (i = 0; i < 12; ++i) 
      if (str_ncasecmp(smon, monthnames[i], 3)==0) {
        t.tm_mon = i+1;
        ok = 1; break;
      }
    
    str_trim(stz); stzdg = stz; tz_sgn = +1;
    if (stz[0] == '+') { stzdg++; }
    else if (stz[0] == '-') { stzdg++; tz_sgn = -1; }    
    if (sscanf(stzdg, "%2d%2d", &tzshift_h, &tzshift_m) == 2) { /* plop */ 
      //printf("stz = '%s', tzshift_h=%d, tzshift_m=%d\n", stz, tzshift_h, tzshift_m);
    } 
    else if (strcasecmp(stz, "EST")==0) { tzshift_h = -5; }
    else if (strcasecmp(stz, "EDT")==0) { tzshift_h = -4; }
    else if (strcasecmp(stz, "CST")==0) { tzshift_h = -6; }
    else if (strcasecmp(stz, "CDT")==0) { tzshift_h = -5; }
    else if (strcasecmp(stz, "MST")==0) { tzshift_h = -7; }
    else if (strcasecmp(stz, "MDT")==0) { tzshift_h = -6; }
    else if (strcasecmp(stz, "PST")==0) { tzshift_h = -8; }
    else if (strcasecmp(stz, "PDT")==0) { tzshift_h = -7; }
  } 
  /* test format ISO 8601 http://www.w3.org/TR/NOTE-datetime -- 6 possiblités
      YYYY (eg 1997)
      YYYY-MM (eg 1997-07)
      YYYY-MM-DD (eg 1997-07-16)
      YYYY-MM-DDThh:mmTZD (eg 1997-07-16T19:20+01:00)
      YYYY-MM-DDThh:mm:ssTZD (eg 1997-07-16T19:20:30+01:00)
      YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+01:00)
   */
  if (!ok && sscanf(s, "%4d-%2d-%2dT%2d:%2d:%f%d:%d", &t.tm_year, &t.tm_mon, &t.tm_mday, 
                    &t.tm_hour, &t.tm_min, &fracsec, &tzshift_h, &tzshift_m) == 8) {
    t.tm_sec = (int)fracsec;
    ok = 1;
  }
  if (!ok && sscanf(s, "%4d-%2d-%2dT%2d:%2d%d:%d", &t.tm_year, &t.tm_mon, &t.tm_mday, 
                    &t.tm_hour, &t.tm_min, &tzshift_h, &tzshift_m) == 7) {
    t.tm_sec = (int)fracsec;
    ok = 1;
  }
  if (!ok && sscanf(s, "%4d-%2d-%2dT%2d%d:%d", &t.tm_year, &t.tm_mon, &t.tm_mday, 
                    &t.tm_hour, &tzshift_h, &tzshift_m) == 6) {
    t.tm_sec = (int)fracsec;
    ok = 1;
  }
  if (!ok && sscanf(s, "%4d-%2d-%2d", &t.tm_year, &t.tm_mon, &t.tm_mday) == 3) {
    confidence = 24*60*60; t.tm_hour = t.tm_min = t.tm_sec = 0;
    ok = 1;
    apply_tzshift = 0;
  }
  if (ok && t.tm_year >= 0 && t.tm_year < 3000 && t.tm_mon > 0 && t.tm_mon <= 12 && t.tm_mday > 0 && t.tm_mday < 32 && 
      t.tm_hour >= 0 && t.tm_hour < 24 && t.tm_min >= 0 && t.tm_min < 60 && t.tm_sec >= 0 && t.tm_sec < 60) {
    if (t.tm_year < 50) t.tm_year += 100; else if (t.tm_year > 1900) t.tm_year -= 1900;
    t.tm_mon--; 
    t.tm_isdst = -1;
    /*printf("str_to_time_t(%s): %04d %02d %02d %02d:%02d:%02d%+02d:%02d\n", s, 
      t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, tzshift_h,tzshift_m);*/
    *tt = mktime(&t);
    if (apply_tzshift) {
      if (tzshift_h < 0) { tzshift_h = -tzshift_h; tz_sgn = -tz_sgn; }
      *tt -= (tzshift_h*60+tzshift_m)*60*tz_sgn; 
#if 0
      /* A CORRIGER 
       update: maintenant ça doit etre ok, je le garde sous le coude a tout hasard*/
#ifdef FREEBSD
      int timezone_h = -(tm_now->tm_gmtoff/3600);
      int timezone_m = abs(tm_now->tm_gmtoff/60)%60;
#else
      int timezone_h = -(timezone/3600);
      int timezone_m = abs(timezone/60)%60;
#endif
#endif
      *tt -= get_timezone(); //(timezone_h*60+timezone_m)*60;
    }
    *tt += confidence;
    /*if (confidence) {
      printf("s=%s time = %ld %s\n", s, *tt, asctime(localtime(tt)));
      }*/
  }
  return ok;
}

void time_t_to_tstamp(time_t tim, char tstamp[15]) {
  struct tm *t = localtime(&tim);
  snprintf(tstamp, 15, "%04d%02d%02d%02d%02d%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

FILE*
open_wfile(const char *fname) {
  int fd;
  FILE *f;
  fd = open (fname, O_CREAT | O_TRUNC | O_RDWR, 0600);
  if (fd != -1) f = fdopen(fd, "w"); else return NULL;
  return f;
}

int
is_url(const char *s) {
  int i=0;
  while (s[i] && isalpha(s[i])) ++i;
  if (i && s[i] == ':' && s[i+1] == '/' && s[i+2] == '/') return i+3;
  else return -1;
}

/* rend une url présentable, et lui degage la nuque si nécessaire */
void url_au_coiffeur(unsigned char *url, int coupe) {
  int i=0,j, j_path=0;
  enum {IN_HOST,IN_PORT,IN_PATH} where = IN_HOST;
  //printf("ENTREE url_au_coiffeur(%s,%d)\n", url, coupe);
  i = is_url(url);
  if (i == -1) {
    fprintf(stderr,"url_au_coiffeur(%s): vous avez des pous\n", url);
    url[0] = 0; return;
  }
  j = i;
  while (url[i]) {
    if (where==IN_HOST && url[i] == ':') where=IN_PORT;    
    if (url[i] == '/') { where=IN_PATH; if (j_path ==0) j_path = j; }

    if (where == IN_PORT && strncmp(url+i, ":80",3)==0) {
      i+= 3;
    } else if (url[i] >= ' ' && (url[i] != '/' || url[i-1] != '/')) {
      url[j++] = url[i++];
    } else ++i;
  }
  assert(j);
  j--;
  if (url[j] == '/') url[j--] = 0;
  while (coupe && j > j_path) {
    while (j > j_path) { if (url[j--] == '/') { coupe--; break; } };
  }
  url[j+1] = 0;
  //printf("SORTIE url_au_coiffeur(%s,%d)\n", url, coupe);
}

void dump_backtrace() {
#ifdef HAVE_BACKTRACE
  static int cnt = 0;
  int i,n;
  void* trace[256];
  char** strings;
  if (cnt++ == 0) {
    n = backtrace(trace, 256);
    strings = backtrace_symbols (trace, n);
    if (strings == NULL) {
      myfprintf(stderr, "backtrace unavailable ... no more memory ?\n"); return;
    }
    myfprintf(stderr,"Backtrace dump follows:\n");
    for (i = 0; i < n; ++i)
      myfprintf(stderr,"%<grn %2d> : %<GRN %s>\n",
                i, strings[i]);
    free (strings);
    cnt--;
  } else { /* on n'est jamais trop prudent */
    fprintf(stderr, "yaisse, a recursive bug in backtrace\n"); 
  }
#endif
}

void assertion_failed(const char *fun, const char *ass) {
  myfprintf(stderr,"assertion failed in function %<MAG %s>: %<YEL %s>\n", fun, ass); 
  dump_backtrace(); 
  abort();
}

static char*
wmcc_iconv(const char *src_encoding, const char *dest_encoding, char *src) {
  size_t inbytesleft, outbytesleft, outsz = 16384;
  size_t cnt, len;
  char       *srce;
  char *out, *oute;
  /* pour eviter d'ouvrir/fermer un million d'iconv.. */
  static iconv_t cv;
  static char *old_src_encoding = 0, *old_dest_encoding = 0, *dest_encoding_trans = 0;
  if (src_encoding == NULL || dest_encoding == NULL) return NULL;
  assert(src_encoding); assert(dest_encoding);
  if (!src || strlen(src) == 0 || src_encoding == NULL) return NULL;

  if (old_src_encoding == NULL || old_dest_encoding == NULL || 
      strcmp(old_src_encoding,src_encoding) ||
      strcmp(old_dest_encoding,dest_encoding)) {
    if (old_src_encoding) {
      /* Bouh! La mémoire est pas libérée a la fin du programme !! */
      free(old_src_encoding); free(old_dest_encoding); free(dest_encoding_trans); iconv_close(cv); 
    }
    old_src_encoding = strdup(src_encoding);
    old_dest_encoding = strdup(dest_encoding);
    dest_encoding_trans = malloc(strlen(old_dest_encoding) + 11); /* //TRANSLIT = 10; \0 = 1 */
    strcpy(dest_encoding_trans, old_dest_encoding);
    strcat(dest_encoding_trans, "//TRANSLIT");
    cv = iconv_open(dest_encoding_trans, old_src_encoding);
    if (cv == (iconv_t)(-1)) {
      fprintf(stderr, "iconv_open(%s,%s) failed : %s\n", dest_encoding, src_encoding, strerror(errno));
      return NULL;
    }
  } else iconv(cv, NULL, NULL, NULL, NULL); /* reinitialisation au cas où */
  out = malloc(outsz); assert(out);
  do {
    errno = 0;
    inbytesleft = strlen(src);
    outbytesleft = outsz;
    srce = src; oute = out;
    cnt = iconv(cv, &srce, &inbytesleft, &oute, &outbytesleft);
    if (cnt == (size_t)(-1)) {
      if (errno == E2BIG) {
        outsz *= 2; out = realloc(out, outsz); assert(out);
      } else {
        int i;
        myprintf("wmcc_iconv('%<YEL %s>' -> '%<YEL %s>'): invalid %s sequence here: %<RED %.30s> [", 
                 src_encoding, dest_encoding, src_encoding, srce);
        for (i=0; i < 16 && srce[i]; ++i) printf("%02x ", srce[i]); printf("]\n");
        iconv(cv, NULL, NULL, NULL, NULL);
        free(out); return NULL;
      }
    }
  } while (cnt == (size_t)(-1));
  len = oute - out;
  out = realloc(out, len + 1); assert(out);
  out[len] = 0;
  return out;
}

void
convert_to_iso8859(const char *src_encoding, char **psrc) {
  char *out = wmcc_iconv(src_encoding, "ISO8859-15", *psrc);
  if (out) { free(*psrc); *psrc = out; }
}

void
convert_from_iso8859(const char *dest_encoding, char **psrc) {
  char *out = wmcc_iconv("ISO8859-15", dest_encoding, *psrc);
  if (out) { free(*psrc); *psrc = out; }
}

void
convert_to_utf8(const char *src_encoding, char **psrc) {
  char *out = wmcc_iconv(src_encoding, "UTF-8", *psrc);
  if (out) { free(*psrc); *psrc = out; }
}

void
convert_from_utf8(const char *dest_encoding, char **psrc) {
  char *out = wmcc_iconv("UTF-8", dest_encoding, *psrc);
  if (out) { free(*psrc); *psrc = out; }
}

void
strbuf_init(strbuf *sb, const char *s) {
  sb->str = NULL;
  sb->max_sz = 0;
  if (s == NULL) s = "";
  strbuf_reserve_len(sb, strlen(s)); 
  strcpy(sb->str, s);
  sb->len = strlen(s);
}

void
strbuf_reserve_len(strbuf *sb, size_t len) {
  assert(len < 1000000); /* sinon y'a un truc qui chie dans la colle à 99.9% */
  if (len+1 > sb->max_sz) {
    if (sb->max_sz == 0) sb->max_sz = 1;
    while (len+1 > sb->max_sz) sb->max_sz *= 2;
    sb->str = realloc(sb->str, sb->max_sz); assert(sb->str);
  }
}

void
strbuf_putc(strbuf *sb, char c) {
  strbuf_reserve_len(sb, sb->len+1);
  sb->str[sb->len++] = c; 
  sb->str[sb->len] = 0;
}

void strbuf_cat(strbuf *sb, const char *s) {
  if (s == NULL) return;
  strbuf_reserve_len(sb, sb->len+strlen(s));
  strcpy(sb->str+sb->len, s);
  sb->len += strlen(s);
  sb->str[sb->len] = 0;
}

void strbuf_ncat(strbuf *sb, const char *s, size_t n) {
  if (s == NULL) return;
  n = MIN(n, strlen(s));
  strbuf_reserve_len(sb, sb->len+n);
  strncpy(sb->str+sb->len, s, n);
  sb->len += strlen(s);
  sb->str[sb->len] = 0;
}

void strbuf_free(strbuf *sb) {
  if (sb->str) free(sb->str); sb->str = NULL;
  sb->len = sb->max_sz = 0;
}

/* ben voila.. fionalement je l'ai faite, du coup faudrait updater url au coiffeur */
int split_url(const char *url, SplittedURL *d) {
  char *p, *p2, *start;
  strncpy(d->data, url, 1000); d->data[999] = 0;
  str_trim(d->data);
  p = d->data;
  if (strlen(d->data) == 0) return -1;
  if (p[0] == '"' && p[strlen(d->data)-1] == '"') { p++; d->data[strlen(d->data)-1] = 0; }
  start = p;
  p = strstr(p, "://"); if (!p) return -1;
  *p = 0;
  if (strcasecmp(start, "http") == 0) {
    d->type = HTTP_URL; d->port = 80;
  } else if (strcasecmp(start, "https") == 0) {
    d->type = HTTPS_URL; d->port = 80;
  } else if (strcasecmp(start, "file") == 0) {
    d->type = FILE_URL; d->port = -1;
  } else return -1;
  p+=3;
  
  /* fait pointer d->host sur le debut */
  d->host = p;
  if (p[0] == '[' && (p2 = strchr(p, ']')) && p2 > p+1) { /* ipv6 */
    d->host = p+1;
    *p2 = 0;
    p = p2+1;
  } else {
    while (*p && *p != ':' && *p != '/') ++p;
  }
  if (*p == ':') {
    *p++ = 0; 
    d->port = atoi(p);
    while (*p && isdigit(*p)) ++p;
  }
  if (*p && *p != '/') return -1;
  d->path_len = 0;
  {int i; for (i=0; i < 50; ++i) d->path[i] = NULL; }
  if (*p == '/') {
    *p++ = 0; /* fin du host */
    while (*p) {
      while (*p == '/') ++p;
      if (*p) {
        d->path[d->path_len] = p;
      }
      while (*p && *p != '/') ++p;
      if (p != d->path[d->path_len]) d->path_len++;
      if (*p) { *p++ = 0; }
      if (d->path_len == 100) return 0;
    }
  }
  return 0;
}

char *shorten_path(const char *s_) {
  char *home = getenv("HOME");
  char *s = strdup(s_);
  if (home && strlen(home) > 1) {
    if (str_startswith(s, home)) {
      s[0] = '~';
      strcpy(s+1, s_ + strlen(home));
    }
  }
  return s;
}
