#ifndef _SITE_H
#define _SITE_H
#include "coincoin.h"

/* fonctions qui agissent sur une liste de sites (sl = site_list) */
void     boards_init_sites(SiteList *slist);

SiteList * sl_create();
void       sl_insert_new_site(SiteList *sl, SitePrefs *sp);
void       sl_delete_site(SiteList *sl, Site *site);
Site *     sl_find_site_by_name(SiteList *sl, char *site_name);
Site *     sl_find_site_id(SiteList *sl, int sid);
Site *     sl_find_board_answer_to_me(SiteList *sl);
void site_save_state(Dock *dock, FILE *f, Site *site);
void site_restore_state(Dock *dock, FILE *f, Site *site);
#endif
