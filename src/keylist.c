#include <stdlib.h>
#include <string.h>
//#include "global.h"
#include "coin_util.h"
#include "myprintf.h"
#include "keylist.h"

#ifndef WMCCC
#include <libintl.h>
#define _(String) gettext (String)
#else
#define _(String) (String)
#endif

void
key_list_destroy(KeyList *first)
{
  KeyList *hk, *n;
  hk = first;
  while (hk) {
    n = hk->next;
    free(hk->key); free(hk); hk = n;
  }
}
 
KeyList *
key_list_add(KeyList *first, const unsigned char *key, KeyListType type, int num, int from_prefs)
{
  KeyList *hk, *last;

  ALLOC_OBJ(hk, KeyList);
  hk->key = strdup(key);
  hk->type = type;
  hk->num = num;
  hk->from_prefs = from_prefs;
  hk->next = NULL;
  
  //BLAHBLAH(1, myprintf(_("Adding keyword: '%<CYA %s>'\n"), key));
  last = first;
  if (last == NULL) {
    first = hk;
  } else {
    while (last->next != NULL && last->next->num > hk->num) last = last->next;
    hk->next = last->next;
    last->next = hk;
  }
  return first;
}

KeyList *
key_list_remove(KeyList *first, const unsigned char *key, KeyListType type)
{
  KeyList *hk, *prev;

  prev = NULL;
  hk = first;
  while (hk) {
    if (strcasecmp(key, hk->key)==0 && (hk->type == type || type == HK_ALL)) {
      //BLAHBLAH(1, myprintf(_("Deleting key: '%<CYA %s>'\n"), key));
      if (prev) {
	/* supprime les refs des postvisuals vers cette clef.. */
	prev->next = hk->next;
      } else {
	first = hk->next;
      }
      free(hk->key);
      free(hk);
      break;
    }
    prev = hk;
    hk = hk->next;
  }
  return first;
}

KeyList *
key_list_clear_from_prefs(KeyList *first)
{
  KeyList *hk;
  hk = first;
  while (hk) {
    if (hk->from_prefs) {
      first = key_list_remove(first, hk->key, hk->type);
      hk = first;
    }
    if (hk) hk = hk->next;
  }
  return first;
}
const char*
key_list_type_name(KeyListType t)
{
  switch (t) {
  case HK_UA: return _("useragent");
  case HK_UA_NOLOGIN: return _("useragent w/o login");
  case HK_LOGIN: return _("login");
  case HK_WORD: return _("word");
  case HK_ID: return _("message id");
  case HK_THREAD: return _("thread from id");
  case HK_ALL: break;
  }
  return NULL;
}


KeyList *
key_list_swap(KeyList *first, const char *s, KeyListType t, int num)
{
  /* verifie si le mot est déjà dans la liste */
  if (key_list_find(first, s, t) == NULL) {
    return key_list_add(first, s, t, num, 0);
  } else {
    return key_list_remove(first, s, t);
  }
}

/* renvoie un hash_code identifiant l'état de la boitakon (pour savoir si elle a été modifiée ou pas */
unsigned
key_list_get_state(KeyList *first, int num) {
  unsigned hash = 0x98651030;
  static unsigned bloup[4] = {0xf0e84bb1,0x8124e841,0xd1ccc871,0x31415976};
  KeyList *hk;
  int cnt = 0;

  hk = first;
  while (hk) {
    if (hk->num >= num) {
      hash ^= (((int)hk->type) ^ bloup[cnt % 4]);
      hash ^= str_hache(hk->key, 100);
      cnt++;
    }
    hk = hk->next;
  }
  return hash;
}


KeyList *
key_list_find(KeyList *hk, const char *s, KeyListType t)
{
  while (hk) {
    if ((hk->type != HK_WORD && strcmp(hk->key, s)==0) ||
	(hk->type == HK_WORD && strcasecmp(hk->key, s)==0)) {
      if (t == HK_ALL || t == hk->type) {
	return hk;
      }
    }
    hk = hk->next;
  }
  return NULL;
}
