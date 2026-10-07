/*
  rcsid=$Id: myprintf.h,v 1.4 2003/03/02 14:41:22 pouaite Exp $

  ChangeLog:
  $Log: myprintf.h,v $
  Revision 1.4  2003/03/02 14:41:22  pouaite
  ce commit est dédié à la mémoire de jacques martin

  Revision 1.3  2002/04/04 20:38:10  pouaite
  plop

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

  Revision 1.1.1.1  2001/11/27 19:17:09  pouaite
  vraie initialisation
*/

#ifndef __MY_PRINTF_H
#define __MY_PRINTF_H

#include <stdio.h>

#ifndef __GNUC__
#  define __attribute__(x) /*nothing*/
#endif

int  myfprintf(FILE *f, const char *fmt, ...); /*__attribute__((format(printf,1,2)));*/
int  myprintf(const char *fmt, ...); /*__attribute__((format(printf,1,2)));*/
int  mysprintf(char *s, const char *fmt, ...); /*__attribute__((format(printf,1,2)));*/

#endif
