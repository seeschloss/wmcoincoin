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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


*/

/*
  rcsid=$Id: myprintf.c,v 1.3 2003/03/02 14:41:22 pouaite Exp $
  ChangeLog:
  $Log: myprintf.c,v $
  Revision 1.3  2003/03/02 14:41:22  pouaite
  ce commit est dédié à la mémoire de jacques martin

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "myprintf.h"

int use_ansi_codes = 1;
int use_color_codes = 1;

static struct {
  char *code;
  char *esc_sequence;
  int is_color_code;
  char *replacement;
} code_list[] = {{"def","\033[0m", 0, ""},    /* default attributes */
		 {"cls","\033[2J", 0, "\n"},    /* clear screen */
		 {"clb","\033[1K", 0, "\015"},  /* clear begin of line */
		 {"cle","\033[2K", 0, ""},      /* clear end of line */
		 {"cll","\015\033[2K", 0, "\015"},  /* clear line */
		 {"bld","\033[1m", 0, ""},      /* bold */
		 {"blk","\033[5m", 0, ""},      /* blink */
		 {"nblk","\033[5m", 0, ""},     /* disable blink */
		 {"rev","\033[7m", 0, ""},      /* reverse video */
		 {"nrev","\033[27m", 0, ""},    /* disable reverse video */
		 {"brt","\033[22m",0, ""},      /* bright */
		 {"nor","\033[22m",0, ""},      /* normal intensity */ 	 
		 {"blk","\033[30m",1, ""},               /* black foreground*/	       
		 {"red","\033[22m\033[31m",1, ""},               /* red foreground */	       
		 {"RED","\033[1m\033[31m",1, ""},       /* bright red foreground */
		 {"grn","\033[22m\033[32m",1, ""},               /* green foreground */	       
		 {"GRN","\033[1m\033[32m",1, ""},       /* bright green */
		 {"yel","\033[22m\033[33m",1, ""},               /* brown foreground */	       
		 {"YEL","\033[1m\033[33m",1, ""},       /* (bright) yellow */
		 {"blu","\033[22m\033[34m",1, ""},               /* blue foreground */	       
		 {"BLU","\033[1m\033[34m",1, ""},       /* bright blue */
		 {"mag","\033[22m\033[35m",1, ""},               /* magenta foreground */	       
		 {"MAG","\033[1m\033[35m",1, ""},       /* bright magenta */
		 {"cya","\033[22m\033[36m",1, ""},               /* cyan foreground */	       
		 {"CYA","\033[1m\033[36m",1, ""},       /* bright cyan */
		 {"wht","\033[22m\033[37m",1, ""},               /* white foreground */	       
		 {"WHT","\033[1m\033[37m",1, ""},       /* bright white */
		 {"bblk","\033[40m",1, ""},              /* black background*/	       
		 {"bred","\033[41m",1, ""},              /* red background */	       
		 {"bgrn","\033[42m",1, ""},              /* green background */	       
		 {"byel","\033[43m",1, ""},              /* brown background */	       
		 {"bblu","\033[44m",1, ""},              /* blue background */	       
		 {"bmag","\033[45m",1, ""},              /* magenta background */	       
		 {"bcya","\033[46m",1, ""},              /* cyan background */	       
		 {"bwht","\033[47m",1, ""},              /* white background */	       
		 {"bdef","\033[49m",1, ""},              /* set default background color */
		 {"und","\033[4m", 0, ""},              /* underscore, default foreground */
		 {"nund","\033[24m", 0, ""},             /* no underscore */
		 {"", "\n\n[myprintf:MAUVAIS CODE!!!]", 0, "\n\n[myprintf:MAUVAIS CODE]"}};

#define ERROR_CODE ((sizeof(code_list)/sizeof(code_list[0]))-1)

char * reformat(const char *fmt);

	       
void 
myprintf_disable_ansi_codes() 
{
  use_ansi_codes = 0;
  use_color_codes = 0;
}

void 
myprintf_disable_color() 
{
  use_color_codes = 0;
}

void 
myprintf_enable_ansi_codes() 
{
  use_ansi_codes = 1;
}

void 
myprintf_enable_color() 
{
  use_ansi_codes = 1;
  use_color_codes = 1;
}


int
myfprintf(FILE *f, const char *fmt, ...)
{
  va_list ap;
  char *fmt2;
  int nchar;

  if (isatty(fileno(f))) {
    myprintf_enable_color();
  } else {
    myprintf_disable_ansi_codes();
  }
  va_start(ap, fmt);
  fmt2 = reformat(fmt);
  if (fmt2) {
    nchar = vfprintf(f, fmt2, ap);
    free(fmt2);
  } else {  /* il y a eu une erreur */
    vfprintf(f, fmt, ap);
    nchar = -1;
  }
  return nchar;
}

int
myprintf(const char *fmt, ...)
{
  va_list ap;
  char *fmt2;
  int nchar;

  if (isatty(STDOUT_FILENO)) {
    myprintf_enable_color();
  } else {
    myprintf_disable_ansi_codes();
  }
  va_start(ap, fmt);
  fmt2 = reformat(fmt);
  if (fmt2) {
    nchar = vprintf(fmt2, ap);
    free(fmt2);
  } else {  /* il y a eu une erreur */
    vprintf(fmt, ap);
    nchar = -1;
  }
  return nchar;
}

int
mysprintf(char *s, const char *fmt, ...)
{
  va_list ap;
  char *fmt2;
  int nchar;

  va_start(ap, fmt);
  fmt2 = reformat(fmt);
  if (fmt2) {
    nchar = vsprintf(s, fmt2, ap);
    free(fmt2);
  } else {  /* il y a eu une erreur */
    vsprintf(s, fmt, ap);
    nchar = -1;
  }
  return nchar;
}



#define FMT_MARGE 50      /* nombre d'octets alloues en prevision de l'allongement de 
			     'outformat' au cours du remplacement des code par 
			     les sequences ANSI */

char *
reformat(const char *fmt)
{
  int cnum;
  char *format_out, *format_in;
  int maxsize;
  char *esc, *esc2, *def_pos;
  int j;
  char *p, *p0, *p1;
  int just_replace, need_len, code_len;

  if (fmt == NULL) return 0;

  if (use_ansi_codes) {
    format_in = (char*)malloc(strlen(fmt) + strlen("%<def>") + 1);
    strcpy(format_in, fmt); 
    strcat(format_in, "%<def>"); /* retour aux couleurs par defaut a la fin de l'affichage */
  } else {
    format_in = (char*)malloc(strlen(fmt) + 1);
    strcpy(format_in, fmt); 
  }

  maxsize = strlen(format_in) + FMT_MARGE;
  format_out = (char*)malloc(maxsize+1); 
  if (format_out == NULL) { return NULL; }
  format_out[0]=0;
  j = 0;
  p1 = format_in;
  do {
    /* recherche de la chaine '%<' */
    p0 = p1;
    p1 = strstr(p0, "%<");
    if (p1) {           
      /* recherche du code qui suit '%<' */
      cnum = 0;
      while (code_list[cnum].code[0]) {
	if (strncmp(p1+2, code_list[cnum].code, strlen(code_list[cnum].code)) == 0) {
	  break;
	}
	cnum++;
      }

      /* remplacement du code si necessaire */
      if (use_ansi_codes == 0 || (code_list[cnum].is_color_code == 1 && use_color_codes == 0)) {
	esc = code_list[cnum].replacement;
      } else {
	esc = code_list[cnum].esc_sequence;
      }

      just_replace = 0;
      def_pos = NULL;
      if (strlen(p1+2) <= strlen(code_list[cnum].code)) {
	esc = code_list[ERROR_CODE].code;
      } else {
	char nextc =  p1[2 + strlen(code_list[cnum].code)];
	if (nextc == '>') {
	  just_replace = 1;
	} else if (nextc == ' ') {
	  def_pos = p1 + 2 + strlen(code_list[cnum].code);
	  do {
	    def_pos++;
	  } while (*def_pos != '>' && def_pos < p1 + strlen(p1)-1);
	  if (*def_pos == '>') {
	    just_replace = 0;  /* on va chercher le '>' correspondant et 
				  le remplacer par %<def> */
	  } else {
	    just_replace = 1;
	    esc = code_list[ERROR_CODE].esc_sequence;
	  }
	}
	else {
	  just_replace = 1;
	  esc = code_list[ERROR_CODE].esc_sequence;
	}
      }

      esc2 = NULL;
      if (just_replace == 0) {
	if (use_ansi_codes == 0) {
	  esc2 = code_list[0].replacement;
	} else {
	  esc2 = code_list[0].esc_sequence;
	}
	need_len = strlen(esc) + j + (p1 - p0) + strlen(esc2) - 1;
      } else {
	need_len = strlen(esc) + j + (p1 - p0);
      }

      code_len = strlen(code_list[cnum].code); /* +1 pour le '>' */

      /* verification espace disponible dans 'format_out' */
      if (need_len >= maxsize) {
	maxsize = need_len + FMT_MARGE;
	format_out = (char*)realloc(format_out, maxsize+1);
	if (format_out == NULL) { return NULL; }
      }
      
      /* copie de la partie avant le code */
      for (p=p0; p < p1; p++) {
	format_out[j++] = *p;
      }

      /* copie de la sequence ANSI */
      for (p = esc; *p; p++)
	format_out[j++] = *p;
      format_out[j] = 0;

      p1 += 2+code_len; 
      if (just_replace == 1) {
	p1++;     /* saute le '>' */
      } else {      /* copie jusqu'au '>' et insere le %<def>*/
	for (p = p1+1; p < def_pos; p++) {
	  format_out[j++] = *p;
	}
	for (p = esc2; *p; p++)
	  format_out[j++] = *p;
        format_out[j] = 0;
	p1 = def_pos+1;
      }

    } else {       /* pas de '%<' avant la fin de la chaine */
      /* verif espace disponible */
      if (j + (int)strlen(p0) >= maxsize) {
	maxsize = j+strlen(p0);
	format_out = (char*)realloc(format_out, maxsize+1);
	if (format_out == NULL) { return NULL; }
      }
      /* et paf */
      strcat(format_out, p0);
      p0 += strlen(p0);
    }
  } while (*p0);
  free(format_in);
  return format_out;
}

