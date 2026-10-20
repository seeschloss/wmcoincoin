#ifndef _DOCK_H
#define _DOCK_H

#include "coincoin.h"

void dock_update_pix_trolloscope(Dock *dock);
int dock_red_button_check(Dock *dock); /* renvoie 1 si le bouton rouge a ete suffisament enfonce */
int dock_find_xiscreen_num(Dock *dock, int x, int y);
int dock_get_icon_pos(Dock *dock, int *iconx, int *icony);
void dock_dispatch_event(Dock *dock, XEvent *event);
void dock_refresh_normal(Dock *dock); /* redessine l'applet (en mode normal, cad pas en mode horloge) */
void dock_refresh_horloge_mode(Dock *dock); /* redessine l'applet en mode horloge */
void dock_refresh_other_win(Dock *dock);
void dock_leds_set_state(Dock *dock); /* active/desactive le clignotement (et la couleur) des leds */
void dock_leds_update(Leds *l); /* decremente les compteurs de clignotement */
void dock_leds_create(Dock *dock, Leds *leds);
void dock_checkout_newstitles(Dock *dock); /* mise à jour du titre défilant de l'applet selon l'arrivage de news */
void dock_set_horloge_mode(Dock *dock);
void dock_unset_horloge_mode(Dock *dock);
char *dock_build_pixmap_porte(Dock *dock);
float http_stats_site_quality(Site *site);
#endif
