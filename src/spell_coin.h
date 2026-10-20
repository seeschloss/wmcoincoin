/*
  rcsid=$Id: spell_coin.h,v 1.4 2002/04/01 01:39:38 pouaite Exp $
  ChangeLog:
  $Log: spell_coin.h,v $
  Revision 1.4  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

  Revision 1.3  2001/12/02 18:11:45  pouaite
  amélioration du support de ispell (moins de pb de ralentissement du palmipede) au prix d'un vilain hack

*/
#ifndef _spell_coin_H
#define _spell_coin_H

typedef struct spell_err *ErrList;

typedef enum {
  SpellComment = '@',
  SpellOK = '*',        /* Pas de fautes */
  SpellRoot = '+',      /* Indique la racine */
  SpellCompound = '-',  /* Le mot est un concatenation de 2 autres*/
  SpellMiss = '&',      /* Mot non trouve avec des solutions */
  SpellGuess = '?',     /* Mot non trouve avec des solutions  
			   [prefix+] root [-prefix] [-suffix] [+suffix] */
  SpellNone = '#'       /* Mot non trouve sans solution */
} SpellErrType;

struct spell_err {
  char* original;   /* Le fameux mot plain de fautes */
  unsigned offset;  /* Sa position dans la phrase */
  unsigned proposals_size; /* Nombre de propositions */
  char** proposals; /* Contiendra les propositions de correction 
		       Mais pas encore implemente
		     */
  ErrList next;
};


ErrList spellString(const char* str);
/* 
   pseudo-tache de fond ispell (appellee depuis Network_Thread dans wmcoincoin.c,
   mais aussi depuis certaines zones d'attente de http.c pour eviter de temporiser ispell 
   quand un transfert se traine un peu (c'est vraiment du multitache de gros naze :)
*/
void ispell_run_background(const char* spellCmd, const char* spellDict);

/* appele de X_loop de wmcoincoin.c, pour tuer le process quand il est reste inutilisé trop longtemps (10sec) */
int check_if_should_kill_ispell(int now);
#endif
