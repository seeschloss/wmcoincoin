#ifndef KEYLIST_H
#define KEYLIST_H

/* petite structure pour stocker la liste des mots-clefs qui déclenche la mise en
   valeur du post dans le pinnipede
   (la mise en valeur des messages de l'utilisateur && leurs reponses fonctionne différement) 

   attention à ne pas abuser des appels à tribune_key_list_test_mi avec des HK_THREAD par milliers ! ça *pourrait*
   commencer à faire mouliner coincoin (a voir..)
*/
typedef struct _KeyList KeyList;
typedef enum {HK_UA, HK_LOGIN, HK_WORD, HK_ID, HK_THREAD, HK_UA_NOLOGIN,HK_ALL} KeyListType;
struct _KeyList {
  unsigned char *key;
  short num;          /* indicateur de niveau (optionnel), pour les mots plops, indique le
			 niveau de plopification (0 -> normal, 1->forte, 2->kickette) */
  unsigned char from_prefs; /* non nul si le mot provient d'une liste definie dans le
			       fichier d'options */
  KeyListType type;
  KeyList *next;
};

void        key_list_destroy(KeyList *first);
KeyList*    key_list_add(KeyList *first, const unsigned char *key, 
			 KeyListType type, int num, int from_prefs);
KeyList*    key_list_remove(KeyList *first, const unsigned char *key, KeyListType type);
KeyList*    key_list_clear_from_prefs(KeyList *first);
const char* key_list_type_name(KeyListType t);
KeyList*    key_list_swap(KeyList *first, const char *s, KeyListType t, int num);
unsigned    key_list_get_state(KeyList *first, int num);
KeyList*    key_list_find(KeyList *hk, const char *s, KeyListType t);

#endif
