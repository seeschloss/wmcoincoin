
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA



 */
/*
  rcsid=$Id: wmcoincoin.c,v 1.97 2005/09/27 16:59:13 pouaite Exp $
  ChangeLog:
  $Log: wmcoincoin.c,v $
  Revision 1.97  2005/09/27 16:59:13  pouaite
  2.5.1c

  Revision 1.96  2005/09/26 21:40:24  pouaite
  v 2.5.1b

  Revision 1.95  2005/09/25 12:08:55  pouaite
  ca marche encore ca ?

  Revision 1.94  2004/05/16 12:54:30  pouaite
  250c

  Revision 1.93  2004/04/28 22:19:01  pouaite
  bugfixes dae + des trucs que j'ai oublie

  Revision 1.92  2004/04/26 20:32:33  pouaite
  roger demande le commit

  Revision 1.91  2004/04/18 15:37:29  pouaite
  un deux un deux

  Revision 1.90  2004/03/07 13:51:12  pouaite
  commit du dimanche

  Revision 1.89  2004/03/03 23:00:40  pouaite
  commit du soir

  Revision 1.88  2004/02/29 19:01:27  pouaite
  et hop

  Revision 1.87  2004/02/29 15:01:20  pouaite
  May the charles bronson spirit be with you

  Revision 1.86  2003/08/26 21:50:48  pouaite
  2.6.4b au mastic

  Revision 1.85  2003/07/20 22:22:28  pouaite
  ce commit est dedie a Pierre Tramo

  Revision 1.84  2003/06/29 23:58:39  pouaite
  suppression de l'overrideredirect du palmi et ajout de pinnipede_totoz.c et wmcoincoin-totoz-get etc

  Revision 1.83  2003/06/25 20:18:21  pouaite
  support xinerama qui marche

  Revision 1.82  2003/06/24 22:27:57  pouaite
  speciale dedicace a nos amis de l'ile de beaute

  Revision 1.81  2003/06/21 14:48:45  pouaite
  g cho

  Revision 1.80  2003/06/09 16:42:29  pouaite
  pan pan

  Revision 1.79  2003/03/12 21:09:50  pouaite
  mega patch de gle + micro fix du referer

  Revision 1.78  2003/03/02 17:56:26  pouaite
  wmcoincoin-2.4.4a.tar.gz is ready for distribution

  Revision 1.77  2003/03/02 14:41:22  pouaite
  ce commit est dédié à la mémoire de jacques martin

  Revision 1.76  2003/03/01 17:31:22  pouaite
  compat ipv6 a tester

  Revision 1.75  2003/02/28 19:08:44  pouaite
  trucs divers

  Revision 1.74  2003/01/12 18:42:19  pouaite
  et une baguette bien cuite pour le monsieur avec l'ornythorinque

  Revision 1.73  2003/01/11 17:44:10  pouaite
  ajout de stats/coinping sur les sites

  Revision 1.72  2003/01/11 14:10:07  pouaite
  fix du palmi pour xf 4.3

  Revision 1.71  2002/12/20 15:49:53  pouaite
  prout 2.4.2b ?

  Revision 1.70  2002/12/20 11:26:35  pouaite
  deux trois conneries

  Revision 1.69  2002/11/11 15:26:43  pouaite
  fix soulignement et strike avec les span

  Revision 1.68  2002/10/16 20:41:45  pouaite
  killall toto

  Revision 1.67  2002/10/13 23:30:49  pouaite
  plop

  Revision 1.66  2002/10/05 18:08:15  pouaite
  ajout menu contextuel + fix de la coloration des boutons du wmccc

  Revision 1.65  2002/09/21 22:51:01  pouaite
  ajout du patch de shift pour le pdfm

  Revision 1.64  2002/09/08 18:44:08  pouaite
  mouaissssss

  Revision 1.63  2002/09/08 14:28:45  pouaite
  bugfixes salutaires

  Revision 1.62  2002/09/07 16:21:16  pouaite
  ça va releaser en douce

  Revision 1.61  2002/09/05 23:11:58  pouaite
  <blog>ce soir g mangé une omelette</blog>

  Revision 1.60  2002/09/02 23:24:41  pouaite
  bugfixes de la soiree

  Revision 1.59  2002/09/01 23:54:56  pouaite
  completurage du wmc3 et compatibilitation avec new.linuxfr

  Revision 1.58  2002/08/31 21:26:46  pouaite
  ajout du wmccc

  Revision 1.57  2002/08/29 00:15:53  pouaite
  cosmétique et capillotraction

  Revision 1.56  2002/08/28 00:42:32  pouaite
  wmccc aware

  Revision 1.55  2002/08/26 00:52:22  pouaite
  coin coin coin

  Revision 1.54  2002/08/22 00:10:14  pouaite
  prout

  Revision 1.53  2002/08/21 23:20:57  pouaite
  coin

  Revision 1.52  2002/08/21 01:11:49  pouaite
  commit du soir, espoir

  Revision 1.51  2002/08/19 00:21:29  pouaite
  "troll du soir, espoir"

  Revision 1.50  2002/08/18 20:52:15  pouaite
  les locales des sites fonctionnent (c bon pour les news)

  Revision 1.49  2002/08/18 19:00:28  pouaite
  plop

  Revision 1.48  2002/08/17 18:33:40  pouaite
  grosse commition

  Revision 1.47  2002/06/26 22:19:49  pouaite
  ptit fix pour la tribune de f-cpu + patch de lordoric

  Revision 1.46  2002/06/23 22:26:01  pouaite
  bugfixes+support à deux francs des visuals pseudocolor

  Revision 1.45  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.44  2002/06/01 17:54:04  pouaite
  nettoyage

  Revision 1.43  2002/05/27 18:39:14  pouaite
  trucs du week-end + patch de binny

  Revision 1.42  2002/05/13 05:52:18  pouaite
  bugfix anti-segfault

  Revision 1.41  2002/05/12 22:06:27  pouaite
  grosses modifs dans http.c

  Revision 1.40  2002/04/14 23:24:22  pouaite
  re-fix pour kde ..

  Revision 1.39  2002/04/13 11:55:19  pouaite
  fix kde3 + deux trois conneries

  Revision 1.38  2002/04/10 22:53:44  pouaite
  un commit et au lit

  Revision 1.37  2002/04/09 00:28:19  pouaite
  quelques modifs faites dans un état d'hébétude avancé /!\ travaux en cours /!\

  Revision 1.36  2002/04/03 20:15:11  pouaite
  plop

  Revision 1.35  2002/04/01 22:56:03  pouaite
  la pseudo-transparence du pinni, bugfixes divers, option tribune.backend_type

  Revision 1.34  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

  Revision 1.33  2002/03/23 10:37:29  pouaite
  retournement de slip, merci lordoric

  Revision 1.32  2002/03/21 22:53:07  pouaite
  ajout d'une icone pour la fenetre du pinnipede et des news

  Revision 1.31  2002/03/19 09:55:58  pouaite
  bugfixes compilation

  Revision 1.30  2002/03/18 22:46:49  pouaite
  1 ou 2 bugfix mineurs, et surtout suppression de la dependance avec la libXpm .. un premier pas vers wmc² en 8bits

  Revision 1.29  2002/03/10 22:45:36  pouaite
  <mavie>dernier commit avant de passer la nuit dans le train</mavie> , spéciale dédicace à shift et à son patch ;)

  Revision 1.28  2002/03/09 19:45:52  pouaite
  microbugfix du plopifieur et ajout d'une macro PATCH_LEVEL

  Revision 1.27  2002/03/08 23:53:40  pouaite
  derniers bugfixes pour la v2.3.6

  Revision 1.26  2002/03/07 18:54:34  pouaite
  raaa .. fix login_color (jjb) patch plop_words (estian) et bidouille pour le chunk encoding (a tester)

  Revision 1.24  2002/03/05 21:04:28  pouaite
  bugfixes suite à l'upgrade de dlfp [et retour au comportement à l'ancienne du clic sur les horloges pour les moules ronchonnes]

  Revision 1.23  2002/03/03 10:10:04  pouaite
  bugfixes divers et variés

  Revision 1.22  2002/02/27 00:32:19  pouaite
  modifs velues

  Revision 1.21  2002/02/26 22:02:07  pouaite
  bugfix gruikissime pour les pbs de lag sous cygwin

  Revision 1.20  2002/02/26 09:18:23  pouaite
  bugfixes divers

  Revision 1.19  2002/02/24 22:13:57  pouaite
  modifs pour la v2.3.5 (selection, scrollcoin, plopification, bugfixes)

  Revision 1.18  2002/02/06 21:34:17  pouaite
  coin coin

  Revision 1.17  2002/02/02 23:49:17  pouaite
  plop

  Revision 1.16  2002/01/31 23:45:00  pouaite
  plop

  Revision 1.15  2002/01/20 22:49:38  pouaite
  ça va releaser

  Revision 1.14  2002/01/20 02:17:13  pouaite
  modifs d'ordre esthetique (!) sans grand interet

  Revision 1.13  2002/01/16 21:27:35  pouaite
  gros coup de balai dans wmcoincoin.c qui s'est du coup splitté en trois: wmcoincoin.c, dock.c et useragents_file.c

  Revision 1.12  2002/01/16 10:34:21  pouaite
  ptit patch glandium

  Revision 1.11  2002/01/15 15:30:17  pouaite
  rien d'interessant

  Revision 1.10  2002/01/14 23:54:06  pouaite
  reconnaissance des posts effectué par l'utilisateur du canard (à suivre...)

  Revision 1.9  2002/01/12 17:29:08  pouaite
  support de l'iso8859-15 (euro..)

  Revision 1.8  2002/01/10 09:18:23  pouaite
  patch de jjb (ralentissement progressif des updates de la tribune en cas d'inactivité du coincoin)

  Revision 1.7  2002/01/10 09:03:06  pouaite
  integration du patch de glandium (requetes http/1.1 avec header 'If-Modified-Since' --> coincoin plus gentil avec dacode)

  Revision 1.6  2001/12/17 17:58:58  pouaite
  bugfix plutot gruik du flamometre qui partait en nouille

  Revision 1.5  2001/12/16 01:43:33  pouaite
  filtrage des posts, meilleure gestion des posts multiples

  Revision 1.4  2001/12/06 22:41:11  pouaite
  patch de glandium (bugfix du mode asclock)

  Revision 1.3  2001/12/02 18:07:03  pouaite
  amélioration du support de ispell avec un vilain hack + correction(..pas sur..) d'un bug de clignotement du flammometre

*/

/*
  avertissement au lecteur...
  c'est un bordel monstrueux, ce fichier est BEAUCOUP TROP GROS
*/

#define GLOBALS_HERE
//#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
//#include <regex.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <locale.h>
#include <langinfo.h>
#include <libintl.h>
#define _(String) gettext (String)
#include <stdarg.h>
#include "coincoin.h"
#include "spell_coin.h"
#include "scrollcoin.h"
#include "balltrap.h"

#ifdef __CYGWIN__
#include <pthread.h>
#endif

#include <sys/utsname.h> /* pour la fonction uname */

#include "coin_util.h"
#include "coin_xutil.h"
#include "fontcoincoin.h"
#include "http.h"
#include "dock.h"
#include "site.h"
#include "kbcoincoin.h"

/* quelques images */
#include "../xpms/coin.xpm"
#include "../xpms/weekday.xpm"
#include "../xpms/month.xpm"
#include "../xpms/clock.xpm"
#include "../xpms/date.xpm"
#include "../xpms/led.xpm"

/* des vieilles variables globales qui feraient mieux de rentrer dans une jolie structure */

GeneralPrefs Prefs;

int opened_cnt; /* ça c'est de la bonne vieille variable qui date de la v0.9...*/

Dock *_dock;

Dock *get_dock() { return _dock; } /* et merde j'ai fini par la faire pour la chasse aux canards */
/*
   la fonction de la honte ...

   ouverture d'une url dans un browser externe (avec une commande du type 'mozilla url &', executée par le shell)

   url est une chaine de carateres (terminée par un zero), de longueur
   raisonnable, mais susceptible de contenir absolument n'importe quels caractères.
   Le principe est d'échapper tous ceux qui sont susceptible d'etre interpretes par le shell.



extrait de RATIONALE FOR SECURITY FILTER, d.wheeler


les arguments bidons sont:
  ballon_{x,y} : position d'affichage du ballon d'aide quand aucun browser externe n'a été défini
  browser_num = 1 ou 2 -> utilisation de la commande Prefs.browser_cmd ou de la commande
  alternative Prefs.browser2_cmd
*/
void
open_url(const unsigned char *url, int balloon_x, int balloon_y, int browser_num)
{
  char hex[16] = "0123456789ABCDEF";
#define CMDSZ 4096
  char s[CMDSZ];
#define SECSZ 2048
  char secure_url[SECSZ];
  int i,j;
  char *bcmd;
  static int wmcc_tic_cnt_last = 0;

  if (ABS(wmcc_tic_cnt - wmcc_tic_cnt_last) < 10) {
    myprintf("hé on se calme sur les urls !\n"); /* capote à souris qui cliquent 100 fois de suite */
    return;
  }
  wmcc_tic_cnt_last = wmcc_tic_cnt;

  if (url == NULL) return;

  bcmd = ((browser_num & (~URL_YES_I_KNOW_WHAT_I_DO)) == 1) ? Prefs.browser_cmd : Prefs.browser2_cmd;

  /* maintenant on est facho */
  if (is_url(url) == -1 && (browser_num & URL_YES_I_KNOW_WHAT_I_DO) == 0) {
    snprintf(s, CMDSZ, _("this url STINKS ! <b>%s</b> (neither ftp://, nor http://)<br>"
           "You will have to launch it by hand (copy to the clipboard with a right click, etc.)"), url);
    if (balloon_x != -1 && balloon_y != -1) {
      balloon_show(_dock, balloon_x, balloon_y, 10, 10, s, 300);
    } else {
      fprintf(stderr, "wmcoincoin: %s\n", s);
    }
    return;
  }

  if (bcmd == NULL) {
    snprintf(s, CMDSZ, _("You didn't define the external browser in ~/.wmcoincoin/options (%s), no need to click on the <b>[url]</b>..."),
	     (browser_num == 1) ? "http.browser: thecommand" : "http.browser2: thecommand");
    if (balloon_x > 0 && balloon_y > 0) {
      balloon_show(_dock, balloon_x, balloon_y, 10, 10, s, 300);
    } else {
      fprintf(stderr, "wmcoincoin: %s\n", s);
    }
    return;
  }


      /* d'apres d.wheeler (et la rfc 1738,2396), une url est formée de caractères
       - réservés:                                ; / ? : @ & = + $ ,
       - non réservés:    A-Z, a-z, 0-9, et | | | | | | - _ . ! ~ * ' ( )

       tous les caractères qui ne sont pas 'réservés' peuvent être hexencodés en %XX.

       toujours selon lui, la liste des caractères interprétables par le shell est
       (http://www.dwheeler.com/secure-programs/Secure-Programs-HOWTO/limit-call) :
       & ; ` ' \ " | * ? ~ < > ^ ( ) [ ] { } $ \n \r


       Les virgules et les parenthèses fermantes dans une url sont susceptible d'etre mal interpretées
       par netscape quand on lui passe l'url en ligne de commande avec nestcape -remote openURL(http://blahblah, ddfdjh)
      */

  /*
     FILTRE DEUX-EN-UN:
     on hex-encode tous les caractère qui ne sont pas les suivants:
     ; / ? : @ & = + $ # % A-Z a-z 0-9 - _ . , et tout ce qui est inférieur à 0x20 est giclé

     ceci dit, on pourrait aussi refuser toute url contenant des caractère devant être hexencodés...

     puis, les charactères non alphanumériques qui n'ont pas été hexencodés sont backslashés pour ne surtout pas
     être interpretés par /bin/sh
  */
  for (i=0, j=0; url[i] && j < SECSZ-3; i++) {
    if (url[i] > ' ') { /* tout ce qui est ' ', '\r' '\n' '\t' passe à la trappe */
      int alnum;
      alnum = ((url[i]>='a' && url[i] <= 'z') ||
             (url[i]>='A' && url[i] <= 'Z') ||
             (url[i]>='0' && url[i] <= '9')) ? 1 : 0;
      /* remarque: on hexencode un caractère réservé, la virgule, mais ce n'est apparament pas génant */
      if (alnum || strchr(";/?:@&=+$#%-_.", url[i])) {
      if (alnum == 0 && url[i] != '.' && url[i] != '_' && url[i] != '/' && url[i] != ':') {
        secure_url[j++] = '\\'; // on insere le backslash
      }
      secure_url[j++] = url[i];
      } else {
      secure_url[j++] = '%';
      secure_url[j++] = hex[(url[i]>>4) & 0x0f];
      secure_url[j++] = hex[(url[i]   ) & 0x0f];
      }
    }
  }
  secure_url[j] = 0;

  assert(j>0 && j < SECSZ); //rhoo

  /*
    donc maintenant on a une jolie url toute propre, et qui *marche* contrairement
     à la bouillie que j'avais fait dans coincoin 2.01
  */

  snprintf(s, CMDSZ, bcmd, secure_url, secure_url, secure_url, secure_url); /* on autorise au max 4 repetitions du '%s' dans le browser_cmd */

  myfprintf(stderr, _("Spawning \"%<YEL %s>\"\n"), s);
  /*
    system fait appel à /bin/sh , mais on a backslashé tous les caractères embetants
  */
  if (system(s) == -1)
	  myfprintf(stderr, "%s failed\n", s);
}

int
dock_find_xiscreen_num(Dock *dock, int x, int y) {
  int i;
  for (i=0; i < dock->nb_xiscreen; ++i) {
    if (x >= dock->xiscreen[i].x_org && x < dock->xiscreen[i].x_org + dock->xiscreen[i].width &&
        y >= dock->xiscreen[i].y_org && y < dock->xiscreen[i].y_org + dock->xiscreen[i].height)
      return i;
  }
  return -1;
}

int
dock_get_icon_pos(Dock *dock, int *iconx, int *icony)
{
  Window junkwin;
  XWindowAttributes wa;

  /* lecture de la position de wmcoincoin */
  if (!XGetWindowAttributes(dock->display, DOCK_WIN(dock), &wa))
    fprintf(stderr, _("Can't get window attributes.\n"));
  XTranslateCoordinates (dock->display, DOCK_WIN(dock), wa.root, 
			 -wa.border_width,
			 -wa.border_width,
			 iconx, icony, &junkwin);
  return dock_find_xiscreen_num(dock,*iconx+32,*icony+32);
}


/* déclenche la mise à jour du trolloscope, du pinnipede etc.. */
void
check_if_board_was_updated(Dock *dock)
{
  static int pix_trolloscope_refresh_cnt = 0;
  if (flag_updating_board == 0) {
    if (flag_board_updated) {
      Site *s;
      int i;

      dock->trib_trollo_rate = 0;
      dock->trib_trollo_score = 0;
      for (s = dock->sites->list; s; s = s->next) {
	if (s->board) {
	  float tr, ts;
	  board_get_trollo_rate(s->board, &tr, &ts);
	  i++;
	  dock->trib_trollo_rate  = MAX(dock->trib_trollo_rate, tr);
	  dock->trib_trollo_score = MAX(dock->trib_trollo_score, ts);
	}
      }
      BLAHBLAH(2,myprintf("%<red checkout_tribune>: %<WHT trollo_rate <- %f, trollo_score <- %f>\n", dock->trib_trollo_rate, dock->trib_trollo_score));

      pix_trolloscope_refresh_cnt = 1000;

      pp_set_board_updated(dock);

      flag_board_updated = 0;
    } 
    if (++pix_trolloscope_refresh_cnt > 100) {
      dock_update_pix_trolloscope(dock); pix_trolloscope_refresh_cnt = 0;
    }
  }
}

void
wmcc_init_http_request(HttpRequest *r, SitePrefs *sp, char *url)
{
  http_request_init(r);
  SplittedURL su;
  if (str_startswith(url, "fucking_brocken")) {
    myprintf("%<RED FIX THIS URL>: %<MAG %s>\n", url); exit(1);
  }
  if (split_url(url,&su)) assert(0);
  
  r->type = HTTP_GET;
  r->url = strdup(url);
  if (sp->proxy_name) {
    r->proxy_name = strdup(sp->proxy_name);
    r->proxy_port = sp->proxy_port;
    if (sp->proxy_auth_user) {
      r->proxy_user = strdup(sp->proxy_auth_user);
	}
    if (sp->proxy_auth_pass) {
      r->proxy_pass = strdup(sp->proxy_auth_pass);
    }
  }
  r->pragma_nocache = sp->proxy_nocache;
  r->use_if_modified_since = sp->use_if_modified_since;
  r->user_agent = strdup(app_useragent);
  r->cookie = NULL;
  r->p_last_modified = NULL;
}

void
wmcc_init_http_request_with_cookie(HttpRequest *r, SitePrefs *sp, char *url)
{
  wmcc_init_http_request(r, sp, url);
  if (sp->user_cookie) {
    r->cookie = strdup(sp->user_cookie);
  }
}

void
wmcc_log_http_request(Site *s, HttpRequest *r)
{
  if (r->tic_cnt_tstamp != -1) {
    int i,cnt=0;
    float sum =0.;
    s->http_ping_stat_buf[s->http_ping_stat_i] = 
      (wmcc_tic_cnt - r->tic_cnt_tstamp)*0.001*WMCC_TIMER_DELAY_MS;
    s->http_ping_stat_i = (s->http_ping_stat_i+1)%NB_HTTP_PING_STAT;
    for (i = 0; i < NB_HTTP_PING_STAT; ++i) {
      int ii = (s->http_ping_stat_i+i)%NB_HTTP_PING_STAT;
      if (s->http_ping_stat_buf[ii] >= 0.) {
	sum += (NB_HTTP_PING_STAT-i)*s->http_ping_stat_buf[ii]; 
	cnt += (NB_HTTP_PING_STAT-i);
      }
    }
    if (cnt) {
      s->http_ping_stat = sum/cnt;
    }
  }
}

/* 
   poste le message sur la tribune -- en tant que fonction 'lente' 
   cette fonction est executée par la boucle principale
 */
void
exec_coin_coin(Dock *dock, int sid, const char *ua, const char *msg_)
{
  HttpRequest r;
  char *msg, *urlencod_msg;
  char path[2048];
  Site *site;

  BLAHBLAH(1, myprintf(_("message posted: '%<YEL %s>\n"), msg_));
  BLAHBLAH(1, myprintf(_("    (useragent: '%<CYA %s>\n"), ua));

  site = sl_find_site_id(dock->sites, sid);
  if (site == NULL) {
    myprintf("?? unable to find site id %d (bug?)\n", sid);
    return;
  }
  if (site->board == NULL) {
    myprintf("???? site %s has no board ! (bug!)\n", site->prefs->site_name);
    return;
  }
  if (str_is_empty(site->prefs->post_url)) {
    char *s = str_printf("Hello, this is the wmcoincoin delivery agent.<p>"
                         "I'm sorry to have to inform you that the message returned "
                         "below could not be delivered to one or more destinations.<p>"
                         "X-Reason: %s is read-only", site->prefs->site_name);
    msgbox_show(dock, s); free(s); return;
  }

  pp_set_download_info(site->prefs->site_name, "posting ...");

  msg = strdup(msg_);
  convert_from_iso8859(site->board->encoding, &msg);
  urlencod_msg = http_url_encode(msg,1); assert(urlencod_msg);
  FREE_STRING(msg);
  snprintf(path, 2048, "%s", site->prefs->post_url);

  wmcc_init_http_request_with_cookie(&r, site->prefs, path);
  /* Triton> Euh..., je ne suis pas sur de ce que je dois mettre ici,
             je crois que je vais repousser à plus tard.
             Bon, tonton zorel< a dit text/xml alors je mets text/xml 
             hop, un truc de plus qui est fait ! Patch Accept: fini. \o/
     See> application/xml, c'est bien aussi.
  */
  r.accept = strdup("application/xml, text/xml");
  if (dock->post_anonyme && r.cookie) { free(r.cookie); r.cookie = NULL; }
  r.type = HTTP_POST;
  r.referer = strdup(path); url_au_coiffeur(r.referer, 1);
  url_au_coiffeur(r.referer, 0); 
  if (r.referer[strlen(r.referer)-1] != '/') strcat(r.referer,"/");/*y'a la place pour et comme dlfp impose le slash a la fin.. */

  if (r.user_agent) { free(r.user_agent); r.user_agent = NULL; }
  r.user_agent = strdup(ua);
  r.post = str_printf(site->prefs->post_template, urlencod_msg);  free(urlencod_msg);

  BLAHBLAH(1,myprintf("Post: %s\n", r.post));

  http_request_send(&r);
  BLAHBLAH(1,myprintf("request sent, status=%<YEL %d> (%d)\n", http_error(&r), flag_cancel_task));
  wmcc_log_http_request(site, &r);
  if (http_is_ok(&r)) {
    if (!site->prefs->user_cookie && r.new_cookie) {
        site->prefs->user_cookie = strdup(r.new_cookie);
	char *p;
	for (p = site->prefs->user_cookie; *p; ++p) {
	    if (*p == '\n' || *p == '\r') *p = ' ';
	}
    }
    http_request_close(&r);
    site->http_success_cnt++;
    site->http_recent_error_cnt = 0;
    site->board->last_posted_id = r.post_id;
 } else if (r.response >= 400 && r.response != 406) { /* 406 c'est du bête "Not Acceptable", on s'en fout */
    char *s;
    /* si la reponse n'est pas un 302 Found */
    s = str_printf(_("[%s] Damned ! There has been an error<p>%s"), site->prefs->site_name, http_error(&r));
    msgbox_show(dock, s);
    free(s);
    site->http_error_cnt++;
    site->http_recent_error_cnt++;
  }

  /* pour la reconnaissance des messages de ceux qui sont généralement authentifiés et se lachent en anonyme de temps à autre */
  if (dock->post_anonyme) { site->board->just_posted_anonymous = 1; }

  dock->coin_coin_sent_decnt = 50; /* attention cette valeur est aussi utilisée dans palmipede.c
                                      (intervention de clipouille pour les corses qui s'endorment sur le clavier) */
  pp_set_download_info(NULL,NULL);
}

/*
il parait que ce type de chose est Mal(tm)
int 
flush_expose(Window w) {
  XEvent dummy;
  int i=0;

  while (XCheckTypedWindowEvent(dock->display, w, Expose, &dummy))
    i++;
  return i;
}
*/

int
launch_wmccc(Dock *dock,...)
{
  va_list ap;
  char *spid = str_printf("%u", (unsigned)getpid());
  char *stmpopt = get_wmcc_tmp_options_filename();
  char *args[100]; int nb_args = 0;
  args[0] = "wmccc";
  args[1] = "-wmccpid";
  args[2] = spid;
  args[3] = get_wmcc_options_filename();
  args[4] = stmpopt;  
  nb_args = 5;
  va_start(ap,dock);
  do {
    args[nb_args++] = va_arg(ap, char*);    
  } while (args[nb_args-1]);

  if (dock->wmccc_pid < 1) {
    switch ( dock->wmccc_pid = fork() ) {
    case -1: /* arrrrg */
      {
	fprintf(stderr, _("Fork failed...(%s)..\n you sux\n"), strerror(errno));
	free(stmpopt); free(spid);
	return -1;
      } break;
    case 0: /* fiston (wmccc) */
      {
	int retExec;      
	/*retExec = execlp("wmccc", "wmccc", "-wmccpid", spid, 
			 get_wmcc_options_filename(), stmpopt, 
			 NULL);*/
        //myprintf("\n\n%<YEL CHANGER LE PATH DU%>%<MAG WMCCC>!!!!\n\n");
        retExec = execvp("wmccc", args);
	if( retExec==-1 ) {
	  fprintf(stderr, _("Exec of wmccc failed...(%s)..\n you sux (wmccc not in path?)\n"), strerror(errno));
	}
	exit(retExec);
      } break;
    default: /* pôpa (wmcc) */
      {
        BLAHBLAH(1,printf("forked wmccc, pid=%d\n", dock->wmccc_pid));
      } break;
    }
  } else {
    printf("wmccc is already running .. pid = %d\n", dock->wmccc_pid);
  }
  va_end(ap);
  free(args[3]);
  free(spid);
  free(stmpopt);
  return 0;
}

/* sauvegarde ou restaure l'état du wmcc
   (position du pinnipede/newswin) 
   decalage horaire des boards
*/
void
wmcc_save_or_restore_state(Dock *dock, int do_restore)
{
  char *fname;
  FILE *f;
  Site *site;

  fprintf(stderr, "%s the coincoin state\n", do_restore ? "restoring":"saving");
  fname = str_printf("%s/.wmcoincoin/wmcc_state", getenv("HOME")); assert(fname);
  if (do_restore) {
    f = fopen(fname, "r");
  } else {
    f = open_wfile(fname);
  }
  if (f==NULL) {
    myfprintf(stderr, _("Unable to open '%s' for %s the state of the beast [%s]\n"), fname, do_restore ? _("reading") : _("writing"), strerror(errno));
  } else {
    if (do_restore == 0) {
      fprintf(f, "#do not edit this file (but you are free to erase it)\n");
      pp_save_state(dock,f);
    } else {
      char *bidon = str_fget_line(f); free(bidon);
      pp_restore_state(dock,f);
    }
    fclose(f);
  }
  free(fname);

  for (site=dock->sites->list; site; site = site->next) {
    fname = str_printf("%s/.wmcoincoin/wmcc_state_%s", getenv("HOME"), site->prefs->site_name); 
    assert(fname);
    if ((f=fopen(fname, (do_restore ? "r" : "w")))==NULL) {
      myfprintf(stderr, _("Unable to open '%s' for writing to save the state of the beast\n"), fname);
    } else {
      if (do_restore == 0) {
	fprintf(f, "#do not edit this file\n");
	site_save_state(dock,f, site);
      } else {
	char *bidon = str_fget_line(f); free(bidon);
	site_restore_state(dock,f,site);
      }
      fclose(f);
    }
    free(fname);    
  }
  fname = get_wmcc_tmp_options_filename();
  unlink(fname); free(fname);
}

/* declenchement des ballons d'aide...*/

void check_balloons(Dock *dock) 
{
#ifndef DISABLE_BALLOONS
  if (dock->mouse_win != None && !balloon_ismapped(dock)) {
    if (dock->mouse_cnt >= 2000) {
      int x,y;
      
      x = dock->mouse_x; y = dock->mouse_y;
      if (dock->mouse_win == dock->win || dock->mouse_win == DOCK_WIN(dock)) {
        int iconx, icony;
        dock_get_icon_pos(dock, &iconx, &icony);
        
        if (dock->door_state == CLOSED && dock->horloge_mode == 0) {
          char s[2048];
          snprintf(s,2048,_("If the balltrap is enabled, you can:<br>"
                            "- launch plastic ducks with <font color=blue><tt>Left Click</tt></font><br>"
                            "- quickly kill all ducks with a <font color=blue><tt>Right Click</tt></font><br>"));
          balloon_test(dock,x,y,iconx,icony,0,3,3,57,11,
                       s);
          balloon_test(dock,x,y,iconx,icony,0,TROLLOSCOPE_X,TROLLOSCOPE_Y,TROLLOSCOPE_WIDTH,TROLLOSCOPE_HEIGHT,
                       _("<p align=center><b>This is a professionnal <font color=#a00000><i>Trolloscope</i></font></b></p>"
                         "It displays symbols corresponding to the user-agents of the last messages posted on the board. "
                         "The relationships between useragent and (colour,symbol) are defined in the <tt>~/.wmcoincoin/options</tt> file<br>"
                         "<font color=blue><tt>Left Click</tt></font><tab>: displays the useragent<br>"
                         "<font color=blue><tt>Middle Click</tt></font><tab>: instant refresh of all boards<br>"
                         "Note: if you have given your authentication cookie to wmCoinCoin and if it "
                         "blinks blue, then someone just answered to one of your posts. "));
          balloon_test(dock,x,y,iconx,icony,0,dock->leds.led[0].xpos, dock->leds.led[0].ypos, 9, 4,
                       _("When this led is blue, a <b>http transfer</b> is underway. "
                         "A <b><font color=red>red</font></b> blinking indicates a problem during the last transfer.<br>"
                         "<font color=blue><tt>Left Click</tt></font>: shows the last error message<br>"));
          balloon_test(dock,x,y,iconx,icony,0,dock->leds.led[1].xpos, dock->leds.led[1].ypos, 9, 4,
                       _("This led does no blink anymore.<br>" //blinks when you have just sent a message on the board, and it is waiting for its delivery.<br>"
                         "It is blue when the message is being sent by the palmipede, and green half a second after the effective sending.<br>"
                         "A click on this led allows one to change the scroll speed of the trolloscope:<br>"
                         "<font color=blue><tt>Left Click</tt></font><tab>: slower<br>"
                         "<font color=blue><tt>Right Click</tt></font><tab>: faster<br>"
                         "<font color=blue><tt>Middle Click</tt></font><tab>: change the trolloscope resolution"));
          balloon_test(dock,x,y,iconx,icony,0,dock->leds.led[2].xpos, dock->leds.led[2].ypos, 9, 4,
                       _("When this led blinks green, a new article is available.<br>"
                         "<font color=blue><tt>Left Click</tt></font><tab>: see the article<br>"
                         "<font color=blue><tt>Right Click</tt></font><tab>: mark all articles as read"));
          balloon_test(dock,x,y,iconx,icony,0,dock->leds.led[3].xpos, dock->leds.led[3].ypos, 9, 4,
                       _("When this led blinks green, you have just received a new message.<br>"
                         "<font color=blue><tt>Left Click</tt></font>: see the new message with the external browser 1<br>"
                         "<font color=blue><tt>Middle Click</tt></font>: see the new message with the external browser 2<br>"
                         "<font color=blue><tt>Right Click</tt></font>: cancel all new messages"));
          balloon_test(dock,x,y,iconx,icony,0,dock->leds.led[4].xpos, dock->leds.led[4].ypos, 11, 5,
                       _("When the cursor is above this led, the level of the <font color=#a00000><i>Troll-o-meter</i></font> is displayed.<br>"
                         "<font color=blue><tt>Left Click</tt></font><tab>: INVOKE THE POWER OF <b>C01N C01N</b> !<br>"
                         "<font color=blue><tt>Right Click</tt></font><tab>: see the statistics of the board, and your XP and votes (if you have provided your authentication cookie)<br>"
                         "<font color=blue><tt>Middle Click</tt></font>: re-read the <tt>~/.wmcoincoin/options</tt> file"));
          balloon_test(dock,x,y,iconx,icony,0,3,49,57,12,
                       _("The time of the last message received on the board, and the number of seconds that have past since<br>"
                         "<font color=blue><tt>Left Click</tt></font>: show/hide the <b>palmipede editor</b><br>"
                         "<font color=blue><tt>Right Click</tt></font>: show/hide the <b>pinnipede teletype</b>"));
          
        } else if (dock->door_state == OPENED && dock->horloge_mode == 0) {
          balloon_test(dock,x,y,iconx,icony,2000,31,30,16,16,
                       _("<b><i>DON'T PANIC</i></b>"));
        }
        //} else if (newswin_is_used(dock) && dock->mouse_win == newswin_get_window(dock)) {
      } else if (editw_ismapped(dock->editw) && dock->mouse_win == editw_get_win(dock->editw)) {
        editw_balloon_test(dock, dock->editw, x, y);
      } else if (pp_ismapped(dock) && dock->mouse_win == pp_get_win(dock)) {
        pp_check_balloons(dock, x, y);
      }
    } else if (dock->mouse_cnt > 1000) { /* l'antibloub doit se declencher un poil plus vite quand meme */
      if (editw_ismapped(dock->editw))
        editw_check_bloub(dock);
    }
  }
#endif
}


void
wmcoincoin_dispatch_events(Dock *dock)
{
  XEvent event;

  /* grosse boucle */
  while (XPending(dock->display)) {

    XNextEvent(dock->display, &event);
    if (XFilterEvent(&event, None))
      continue;

    /* des que la souris bouge, le ballon disparait */
    balloon_check_event(dock, &event);
    
    /* 
       c'est pas le switch principal des evenements, 
       juste la gestion du declenchement des ballons
       + on ramene le focus dans editw (quand elle est active)
       + on remet à 0 le temps depuis le dernier déplacement */
    switch (event.type) {
    case EnterNotify: 
      {
	dock->mouse_x = event.xcrossing.x;
	dock->mouse_y = event.xcrossing.y;
	dock->mouse_cnt = 0;
	dock->mouse_win = event.xany.window;
        /*	if (editw_ismapped(dock->editw))
                editw_set_kbfocus(dock, dock->editw, 1);*/
      } break;
    case MotionNotify:
      {
	dock->mouse_x = event.xmotion.x;
	dock->mouse_y = event.xmotion.y;
	dock->mouse_cnt = 0;
	dock->mouse_win = event.xany.window;
	temps_depuis_dernier_event = 0;
      } break;
    case LeaveNotify:
      {
	dock->mouse_x = -1;
	dock->mouse_y = -1;
	dock->mouse_cnt = 0;
	dock->mouse_win = None;
      } break;
    case ButtonRelease:
    case KeyPress:
      {
	dock->mouse_cnt = 0;
      } break;
    }


    if (plopup_ismapped(dock)) {
      plopup_dispatch_event(dock, &event);
    } else {
      if (event.type == KeyPress) {
        int ok = 0;
        if (event.xany.window == pp_get_win(dock)) {
          kb_xim_lookup_key(&event.xkey, KB_PINNIPEDE); ok = 1;
        } else if (event.xany.window == editw_get_win(dock->editw)) {
          kb_xim_lookup_key(&event.xkey, KB_PALMIPEDE); ok = 1;
        }
        if (ok) {
          if (!pp_handle_keypress(dock,&event)) {
            editw_handle_keypress(dock,dock->editw,&event);
          }
        }
      } else if (event.type == KeyRelease) {
        if (!pp_handle_keyrelease(dock,&event))
          editw_handle_keyrelease(dock,dock->editw,&event);
      } else if ((event.xany.window == dock->iconwin && dock->iconwin) || event.xany.window == dock->win) {
	dock_dispatch_event(dock, &event);
      } else if (event.xany.window == editw_get_win(dock->editw) && event.xany.window) {
	editw_dispatch_event(dock, dock->editw, &event);
      } else if (msgbox_ismapped(dock) && event.xany.window == msgbox_get_win(dock)) {
	msgbox_dispatch_event(dock, &event);
      } else if (pp_dispatch_event(dock, &event)) {        
	/* plop */
      } else if (balltrap_dispatch_event(dock, &event)) {
        /* pika */
      }
        /*} else if (newswin_is_used(dock)) {
	if (event.xany.window == newswin_get_window(dock)) {
	  newswin_dispatch_event(dock, &event);
          }
      }*/
    }
  }
}

/* appelee ~25 fois par seconde */
void
timer_signal(int signum) {

  if (signum == SIGUSR1) {
    flag_discretion_request = (_dock->horloge_mode ? -1 : 1);
    //printf("sigusr1\n"); 
#   ifdef SIGNAUX_A_LANCIENNE // mais maintenant on utilise sigaction
#   ifdef _XOPEN_SOURCE
    assert(signal(SIGUSR1, timer_signal) != SIG_ERR);
#   endif
#   endif
    return;
  } else if (signum == SIGUSR2) {
    //    flag_discretion_request = -1;

    flag_update_prefs_request = 2; /* c'est wmccc qui nous cause */

    //    printf("sigusr2\n"); 
#   ifdef SIGNAUX_A_LANCIENNE // mais maintenant on utilise sigaction
#   ifdef _XOPEN_SOURCE
    assert(signal(SIGUSR2, timer_signal) != SIG_ERR);
#   endif
#   endif
    return;
  } else if (signum != SIGALRM) {
    return;
  }

#ifdef SIGNAUX_A_LANCIENNE // mais maintenant on utilise sigaction
#ifdef _XOPEN_SOURCE
  assert(signal(SIGALRM, timer_signal) != SIG_ERR);
#endif
#endif

  X_loop_request++; wmcc_tic_cnt++;
}

int x_error_handler(Display *d UNUSED, XErrorEvent *e UNUSED) {
  fprintf(stderr, _("X11 Error. The coincoin is going down for halt NOW!\n"));
  wmcc_save_or_restore_state(_dock, 0);
  exit(0);
}

int x_io_error_handler(Display *d UNUSED) {
  fprintf(stderr, _("X11 IO Error. The coincoin is going down for halt NOW!\n"));
  wmcc_save_or_restore_state(_dock, 0);
  exit(0);
}


/*
  potentiellement causé par ispell quand il y a un problème
   (genre mauvais dictionnaire, malaise d'ispell ..)

   update: chuis pas sûr de moi.. le sigpipe est seulement reçu quand
   on debuggue avec gdb .. bof.. m'en fous
*/
void
sigpipe_signal(int signum UNUSED) {
  static volatile int in_sigpipe = 0;
  if (in_sigpipe == 0) { /* bon finalement je l'ai eu le bug occasionnel de sigpipe infini quand on se delogue..
                            ça devrait un peu aider */
    in_sigpipe++;
    //BLAHBLAH(1,fprintf(stderr, _("Got a SIGPIPE ! Either ispell crashed, or you have just killed\nviolently the coincoin.\n")));
    //wmcc_save_or_restore_state(_dock, 0);
    in_sigpipe--;
  }
}
/* poreil ! */
void
sigchld_signal(int signum UNUSED) {
  //  BLAHBLAH(0,fprintf(stderr, _("Got a SIGCHLD !\n")));
  /* je comprends pas pourquoi un wait() ici n'élimine pas les zombies ... 
     tant pis, ça marche bien comme ça avec le bon gros kill_ispell d'ours */
}
void
sigint_signal(int signum UNUSED) {
  fprintf(stderr, _("pan ! pan ! ... arg\n"));
  wmcc_save_or_restore_state(_dock, 0);
  exit(0);
}


/*
  appelée pour la tribune et les news (delai_base == secondes)
   (pour la tribune, delai_base = Prefs.dlfp_tribune_check_delay,
    pour les news,   delai_base = Prefs.dlfp_news_check_delay)
*/
int
wmcc_eval_delai_rafraichissement(Dock *dock, int delay_base, int recent_err_cnt)
{
  int delay;

  delay = 25*delay_base;

  /* verifie si on a demandé d'éteindre le coincoin au bout d'un certain nb de minutes */
  if (Prefs.switch_off_coincoin_delay != 0) {
    if (temps_depuis_dernier_event/(25*60) > Prefs.switch_off_coincoin_delay) {
      if (dock->horloge_mode == 0) flag_discretion_request = 1; /* passage en horloge :) */
      delay = 10000000; return delay;
    }
  }

  if (Prefs.max_refresh_delay == 0) return delay;

  if (temps_depuis_dernier_event/25 < 300) {
    delay = 25*delay_base;
  } else if (temps_depuis_dernier_event/25 < 1200) {          /* cad entre 5 min et 20mind'inactivité */
    delay = 25*2*delay_base;                                 /* on passe à (par defaut) un refresh/min */
  } else if (temps_depuis_dernier_event/25 < 90*60) {         /* en 20 min et 1h30 */
    delay = 25*10*delay_base;                                /* on passe à un refresh/5min */
  } else {                                                    /* apres 1h30 */
    delay = 25*60*delay_base;                                /* on passe aux delai_max (qui est exprime en minutes) */ 
  }
  delay *= (recent_err_cnt+1);
  delay = MIN(delay, Prefs.max_refresh_delay*25*60);
  return delay;
}


/*
  maj les differents timers et remplit la file d'attente 
*/
void
update_timers(Dock *dock)
{
  Site *site;
  if (dock->coin_coin_sent_decnt >= 1) dock->coin_coin_sent_decnt--; /* pour éteindre la led[1] apres un cours délai */
  dock->red_button_send_cnt++;
  if (flag_update_prefs_request) {
    ccqueue_push_prefs_update(flag_update_prefs_request);
    flag_update_prefs_request = 0;
  }

  /* 
     ces 5 lignes servent à redéclencher le rafraissement lors d'un brusque retour d'activité 
     (si le coincoin rafraichissait 1fois/15min, il va faire très rapidement un refresh) 
     par contre, le delai ne peut être accru que juste après un refresh (c'est mieux pour 
     l'affichage du temps avant le prochain refresh dans le pinni, ça évite des sauts)
  */
  for (site = dock->sites->list; site; site = site->next) {
    Board *b = site->board;
    /*site->news_refresh_delay = 
      MIN(site->news_refresh_delay, 
      wmcc_eval_delai_rafraichissement(dock, site->prefs->news_check_delay, site->http_recent_error_cnt));*/
    if (b) {
      b->board_refresh_delay = 
	MIN(b->board_refresh_delay, 
	    wmcc_eval_delai_rafraichissement(dock, site->prefs->board_check_delay, site->http_recent_error_cnt));
    }
  }

  /* les tribunes */
  for (site = dock->sites->list; site; site = site->next) {
    Board *b = site->board;
    if (site->prefs->check_board && ccqueue_find(Q_BOARD_UPDATE, site->site_id)==NULL) {
      if (b->auto_refresh && b->board_refresh_decnt)
	b->board_refresh_decnt--;
      if (b->board_refresh_decnt == 0) {
	ccqueue_push_board_update(site->site_id);
	b->board_refresh_delay = 
	  wmcc_eval_delai_rafraichissement(dock, site->prefs->board_check_delay, site->http_recent_error_cnt);
        b->board_refresh_decnt = b->board_refresh_delay;
      }
      b->board_refresh_decnt = MIN(b->board_refresh_decnt, b->board_refresh_delay);
    }
  }
}

/* la boucle principale (appelée 25 fois par seconde, mais uniquement aux moments 
   propices (cad pas au milieu d'un malloc..)) */
void X_loop()
{
  static int no_reentrant = 0;
  int save_errno;
  static int timer_cnt = 0;
  Dock *dock = _dock;

  if (no_reentrant) {
    // apres reflexion ça peut arriver (en placant allow_x_loop dans des fonctions succeptible d'etre
    // appellees depuis network_thread et X_loop, dans ce cas, hop
    //printf ("REENTRANT !!!!! je vais me mordre les ***i*les\n"); exit(1); /* ça ne doit JAMAIS arriver */
    return;
  }
  no_reentrant = 1;

  X_loop_request = 0;

  save_errno = errno; /* eh oui, si on appelle X_loop dans une periode délicate, il vaut 
			 mieux ne pas trop toucher à cette variable... */
  
  timer_cnt++;
  dock->view_id_timer_cnt++;
  dock->mouse_cnt+=40;

  update_timers(dock);


  /* verifie si il y a des ballons d'aide a afficher */
  if (Prefs.use_balloons) check_balloons(dock); 
  
  editw_action(dock, dock->editw); /* animation du palmipede si necessaire */

  /* lectures des evenements */
  wmcoincoin_dispatch_events(dock);

  timer_cnt++;

  if (timer_cnt % 10 == 0) {
    check_if_should_kill_ispell(0); /* pan ? */
  }
  
  if (timer_cnt % 5 == 0) {
    pp_animate(dock);   /* omg ! il bouge ! */
  }
  if (timer_cnt % 25 == 0) {
    pp_totoz_check_updates(dock); /* met a jour l'affichage et lance les 
                                     telechargements si Prefs.board_auto_dl_pictures */
  }

  if (timer_cnt % 1 == 0) {
    /*
    Window focwin;
    int revert_to;
    XGetInputFocus(dock->display, &focwin, &revert_to);
    printf("focus win=%08lx [%s]\n", focwin, 
           focwin == editw_get_win(dock->editw) ? "palmipede" : 
           (focwin == pp_get_win(dock) ? "pinnipede" : "???"));
    */
    if (dock->horloge_mode == 0) {
      dock_refresh_normal(dock);
    } else {
      dock_refresh_horloge_mode(dock);
    }
    if (timer_cnt < 400) dock_refresh_other_win(dock);

    /* gestion des animations du dock */
    if (dock->door_state == OPENING) {
      dock->door_state_step++;
      /* note pour les generations futures: cette partie du code est une des plus anciennes,
	 elle remonte à la v1.0beta */
      if (dock->door_state_step > 25+13) {
	dock->door_state_step = 25+13;
	dock->door_state = OPENED;
	opened_cnt = 0;
      }
    } else if (dock->door_state == OPENED) {
      if (opened_cnt++ > 200) {
	dock->door_state = CLOSING;
      }
    } else if (dock->door_state == CLOSING) {
      dock->door_state_step --;
      if (dock->door_state_step <= 0) {
	dock->door_state_step = 0;
	dock->door_state = CLOSED;
	dock->red_button_press_flag = -1;
      }
    }
    if (dock->red_button_press_flag > 0) {
      dock->red_button_press_state++;
      if (dock->red_button_press_state >= 6) dock->red_button_press_state = 5;
    } else if (dock->red_button_press_flag < 0) {
      dock->red_button_press_state--;
      if (dock->red_button_press_state <= 0) { 
	dock->red_button_press_state = 0;
	dock->red_button_press_flag = 0;
	if (dock->door_state != CLOSED) dock->door_state = CLOSING;
      }
    }
    
    dock_leds_set_state(dock);
    dock_leds_update(&dock->leds);

    /* suivi du clignotement du flamometre */
    {
      int clign = 0;
      if (dock->flamometre.xp_change_decnt) { dock->flamometre.xp_change_decnt--; clign = 1; }
      if (dock->flamometre.comment_change_decnt) { dock->flamometre.comment_change_decnt--; clign = 1; }
      if (dock->flamometre.board_answer_decnt) { dock->flamometre.board_answer_decnt--; clign = 1; }

      if (clign) {
	dock_update_pix_trolloscope(dock);
      }
    }
    /* déclenchement du clignotement du flamometre */
    if (/*flag_updating_comments == 0 && */flag_updating_board == 0) {
      Site *site;
#if 0
      if ((site = sl_find_xp_change(dock->sites))) {
	site->xp_change_flag = 0;
	printf("debut flamo %s\n", site->prefs->site_name);
	/* on s'assure de rajouter une quantité divisible par FLAMOMETRE_XP_CLIGN_SPEED */
	dock->flamometre.xp_change_decnt += (((FLAMOMETRE_XP_DUREE*(1000/WMCC_TIMER_DELAY_MS))/
					      FLAMOMETRE_XP_CLIGN_SPEED)*FLAMOMETRE_XP_CLIGN_SPEED);
      }
      if ((site = sl_find_comment_change(dock->sites))) {
	site->comment_change_flag = 0;
	if (site_yc_find_modified(site)) { /* les inconsistences sont possibles */
	  dock->flamometre.comment_change_decnt += (((FLAMOMETRE_COMMENT_DUREE*(1000/WMCC_TIMER_DELAY_MS))/
						     FLAMOMETRE_COMMENT_CLIGN_SPEED)*FLAMOMETRE_COMMENT_CLIGN_SPEED);
	}
      }
#endif
      if ((site=sl_find_board_answer_to_me(dock->sites))) {
	site->board->flag_answer_to_me = 0;
	pp_tabs_set_flag_answer_to_me(dock, site);
	dock->flamometre.board_answer_decnt += (((FLAMOMETRE_TRIB_DUREE*(1000/WMCC_TIMER_DELAY_MS))/
						   FLAMOMETRE_TRIB_CLIGN_SPEED)*FLAMOMETRE_TRIB_CLIGN_SPEED);
      }
    }


    //dock_checkout_newstitles(dock);
    check_if_board_was_updated(dock);

    /* 
       affichage effectif du coincoin 

       c'est le genre de petite instruction de rien du tout qu'on met des jours à retrouver
       et paradoxalement dont l'absence se ferait immédiatement remarquer
    */
    XCopyArea(dock->display, dock->coinpix, DOCK_WIN(dock), dock->NormalGC,
	      0,0, 64, 64, 0,0);

    //    if (timer_cnt % 2048 == 0) reread_messages();
    
    /* aaaaaaaaaaaaaarf oula je croyais avoir mis toutes ces merdes dans dock.c */
    if (strlen(dock->newstitles)>10) {
      dock->newstitles_char_dec+=2;
      if (dock->newstitles_char_dec >= 6) {
	dock->newstitles_char_dec = 0;      
	dock->newstitles_pos++; if (dock->newstitles_pos >= (int)strlen(dock->newstitles)) {
	  dock->newstitles_pos = 0;
	}
      }
    }
    
    if (dock->tl_item_clicked && dock->msginfo_defil > 0) {
      dock->msginfo_defil+=3;
    }

    /* faut-il autoscroller le pinnipede teletype ? */
    pp_check_board_updated(dock);

    /* update la fenetre des news si necessaire */
#if 0
    if (newswin_is_used(dock)) {
      if (flag_news_updated && flag_updating_news == 0) {
	newswin_update_content(dock, 0);
	newswin_draw(dock);
	flag_news_updated = 0;
      } else if (flag_updating_news) {
	newswin_update_info(dock, 0, 0); /* juste pour afficher 'maj de news en cours' */
      }
    }
#endif
  }
  if ((timer_cnt % 4) == 0) 
    balltrap_animate(dock);


  /* le chef est-il dans le bureau ? */
  if (flag_discretion_request == +1 && !Prefs.auto_swallow) {
    balltrap_armageddon(dock);
    if ((dock->discretion_saved_state.palmipede_used = editw_ismapped(dock->editw))) {
      editw_unmap(dock, dock->editw);
    }

    if ((dock->discretion_saved_state.pinnipede_used = pp_ismapped(dock))) {
      pp_unmap(dock);
    }
    balloon_hide(dock); msgbox_hide(dock);
    flag_discretion_request = 0;
    dock->discretion_saved_state.last_sig_is_usr1 = 1;
    dock_set_horloge_mode(dock); /* arf arf le gros camouflage ! */
  } else if (flag_discretion_request == -1) { /* ahhh, il vient de sortir alors :) */
    if (dock->discretion_saved_state.last_sig_is_usr1) { /* on vient de tout cacher ? */
      if (dock->discretion_saved_state.palmipede_used &&
	  editw_ismapped(dock->editw) == 0) {
	editw_show(dock, NULL, 0);
      }
      if (dock->discretion_saved_state.pinnipede_used && 
	  pp_ismapped(dock)==0) {
	pp_show(dock);
      }
    } else { /* si on ne vient pas de tout cacher, on considère qu'il faut "raise" les fenetres */
      if (editw_ismapped(dock->editw)) XRaiseWindow(dock->display, editw_get_win(dock->editw));
      if (pp_ismapped(dock)) XRaiseWindow(dock->display, pp_get_win(dock));
    }
    dock_unset_horloge_mode(dock);
    flag_discretion_request = 0;
    dock->discretion_saved_state.last_sig_is_usr1 = 0;
  }

  /* si ispell vient de se finir, on essaye de réafficher le palmipede pour prendre en compte les erreurs ... */
  if (editw_ismapped(dock->editw) && flag_spell_finished) {
    if (flag_spell_finished == 1) {
      editw_refresh(dock, dock->editw);
      flag_spell_finished = 0; /* remise a zero APRES le redraw histoire de ne pas relancer un ispell en plein milieu du redraw */
    } else {
      flag_spell_finished--; /* y'a un petit delai entre la fin de la correction et 
				son affichage, c'est juste pour éviter un clignotement trop
				chiant quand on tape
			     */
    }
  }


  /* verif de la 'visibilité' de l'applet, pour masquage du palmipede si
     necessaire. OUI, c'est gruik et nul de faire comme ça, MAIS ce salaud de
     wmaker fout les applets en substructurenotify ce qui veut dire que wmcc ne
     reçoit pas les unmapnotify lorsque l'applet est cachée (par un changement
     de bureau par ex. ...)  

     Si le palmi n'est pas en mode override_redirect, on laisse le wm choisir
     et on intercepte le unmapnotify dans palmipede.c
  */
  if (Prefs.palmipede_override_redirect && editw_ismapped(dock->editw)) {
    XWindowAttributes wattr;
    XGetWindowAttributes(dock->display, DOCK_WIN(dock), &wattr);
    if (wattr.map_state == IsUnmapped || wattr.map_state == IsUnviewable) {
      editw_unmap(dock, dock->editw);
    }
  }

  no_reentrant =  0;

  errno = save_errno;
}


/* je l'ai pompe honteuseument dans le classique 'wmgeneral.c' ... 
 */
void createXBMfromXPM(char *xbm, char **xpm, int sx, int sy) {

  int i,j,k;
  int width, height, numcol, depth;
  int zero=0;
  unsigned char	bwrite;
  int bcount;
  int curpixel;

  sscanf(*xpm, "%d %d %d %d", &width, &height, &numcol, &depth);

  for (k=0; k!=depth; k++) {
    zero <<=8;
    zero |= xpm[1][k];
  }

  for (i=numcol+1; i < numcol+sy+1; i++) {
    bcount = 0;
    bwrite = 0;
    for (j=0; j<sx*depth; j+=depth) {
      bwrite >>= 1;

      curpixel=0;
      for (k=0; k!=depth; k++) {
	curpixel <<=8;
	curpixel |= xpm[i][j+k];
      }

      if ( curpixel != zero ) {
	bwrite += 128;
      }
      bcount++;
      if (bcount == 8) {
	*xbm = bwrite;
	xbm++;
	bcount = 0;
	bwrite = 0;
      }
    }
  }
}

/*
  ben ça fait l'icone, quoi
*/
  
void
wmcc_set_wm_icon(Dock *dock) {
  XIconSize *isz;
  int nbsz;
  int w,h,z,i,j,c;
  RGBAImage *in_img, *out_img;
#include "../xpms/icon.xpm"

  /* 
     ça a l'air super tordu, mais en fait y'a tout une partie qui sert à rien
  */
  w = 0; h = 0;
  BLAHBLAH(2,fprintf(stderr,_("Creating the windows icon. Just tell me if it makes your WM crash.\n")));
  if (XGetIconSizes(dock->display, dock->win, &isz, &nbsz) != 0) {
    /* aucun wm ne propose de taille d'icone, ce code n'a donc jamais ete teste */
    int i;
    printf("nbsz=%d\n", nbsz);
    for (i=0; i < nbsz; i++) {
      printf("icon sz %d: [%d..%d]x[%d..%d], step=%d,%d\n", i, isz[i].min_width, isz[i].max_width, isz[i].min_height, isz[i].max_height, isz[i].width_inc, isz[i].height_inc);      
      if (i == 0) {
	w = (isz[i].max_width/16) * 16; h = (isz[i].max_height/16)*16;
	w = MAX(w, isz[i].min_width); h = MAX(h, isz[i].min_height);
      }
    }
    XFree(isz);
  } else {
    /* pas la peine de raler, *tous* les wm suxent des ours */
    //    printf("pas de taille d'icone par défaut, voila un wmanager qui suce des ours\n");
    w = 48; h = 48;
  }
  assert(w>0 && h>0);

  in_img = RGBACreateRImgFromXpmData(dock->rgba_context, icon_xpm); assert(in_img);
  assert(in_img->w == 16 && in_img->w == 16);
  out_img = RGBACreateImage(w, h); assert(out_img);
  for (i=0; i < w; i++) {
    for (j=0; j < h; j++) {
      for (c=0; c < 4; c++) {
	out_img->data[j][i].rgba[c] = 0;
      }
    }
  }
  z = MAX(MIN(w/16, h/16),1);
  /* ça c'est un zoom */
  for (i=0; i < 16; i++) {
    for (j=0; j < 16; j++) {
      int ii, jj;
      for (ii = 0; ii < z; ii++) {
	for (jj = 0; jj < z; jj++) {
	  int di, dj;
	  
	  di = MIN(MAX(w/2 + ((i-8)*z+ii),0),w);
	  dj = MIN(MAX(h/2 + ((j-8)*z+jj),0),h);
	  for (c = 0; c < 4; c++) {
	    out_img->data[dj][di].rgba[c] = in_img->data[j][i].rgba[c];
	  }
	}
      }
    }
  }
  dock->wm_icon_pix = RGBAImage2Pixmap(dock->rgba_context, out_img); assert(dock->wm_icon_pix);
  dock->wm_icon_mask = alpha2pixmap_mask(dock->display, DOCK_WIN(dock), w, h, 
					 (unsigned char *)out_img->data[0], 255);
  RGBADestroyImage(in_img);
  RGBADestroyImage(out_img);
}

/* x initialization crap , not nice */
void initx(Dock *dock, int argc, char **argv) {
  int i;

  /* some x structs */
  XSizeHints xsh;
  XWMHints *xwmh;
  XClassHint xch;
  XGCValues xgcv;
  XTextProperty xtp;
  char coin_mask[64*64/8];
  char clock_mask[64*64/8];
  int use_iconwin = Prefs.use_iconwin && !Prefs.auto_swallow;
	
  /* connect to default display */
  dock->display = XOpenDisplay(getenv("DISPLAY"));
  if(!dock->display) {
    fprintf(stderr, "Couldn't connect to display\n");
    exit(1);
  }
  
  /* la magie des locales */
//  XSetLocaleModifiers("@im=none"); /* si quelqu'un sait ce que ça veut dire, je suis interessé */
  dock->input_method = (dock->fuck_utf8 ? NULL : XOpenIM(dock->display, NULL, NULL, NULL));
  if (!dock->input_method) {
 //   printf("echec de XOpenIM() [locale=%s], ca pue ! -- switching to C locale\n",setlocale (LC_ALL, ""));
 //   setlocale (LC_ALL, "C");
    dock->input_method = XOpenIM(dock->display, NULL, NULL, NULL);
    if (!dock->input_method) {
   //   printf("Erreur ! echec de XOpenIM() [locale=%s], ca pue encore plus !! -- \n",setlocale (LC_ALL, ""));
    }
  }
  kb_build();


  /* get screen and root window */
  dock->screennum = DefaultScreen(dock->display);
  dock->rootwin = RootWindow(dock->display, dock->screennum);

  dock->rgba_context = RGBACreateContext(dock->display, dock->screennum);    

  ccfont_initialize(dock->display, dock->screennum, dock->rgba_context->visual , dock->rgba_context->cmap, dock->rootwin);

  /* detect xinerama */
  dock->nb_xiscreen = 0;
#ifdef XINERAMA
  {
    int event_base, error_base;
    if (XineramaQueryExtension(dock->display, &event_base, &error_base)) {
      XineramaScreenInfo *xsi;
      int xv1=0, xv2=0,i;
      XineramaQueryVersion(dock->display,&xv1, &xv2);
      myprintf("Xinerama extension %<YEL supported> by X server (version %d.%d)\n", xv1, xv2);
      xsi = XineramaQueryScreens(dock->display, &dock->nb_xiscreen);
      if (dock->nb_xiscreen>0) {        
        dock->xiscreen = calloc(dock->nb_xiscreen, sizeof *(dock->xiscreen));
        for (i = 0; i < dock->nb_xiscreen; ++i) {
          dock->xiscreen[i].screen_number = xsi[i].screen_number;
          dock->xiscreen[i].x_org = xsi[i].x_org;
          dock->xiscreen[i].y_org = xsi[i].y_org;
          dock->xiscreen[i].width = xsi[i].width;
          dock->xiscreen[i].height= xsi[i].height;
          myprintf("screen %d: [%<yel %d>-%<yel %d>]x[%<yel %d>-%<yel %d>]\n", 
                   dock->xiscreen[i].screen_number,
                   dock->xiscreen[i].x_org,
                   dock->xiscreen[i].y_org,
                   dock->xiscreen[i].x_org+dock->xiscreen[i].width-1,
                   dock->xiscreen[i].y_org+dock->xiscreen[i].height-1);
        }
      }
    } else myprintf("no Xinerama for this server\n");
  }
#endif
  if (dock->nb_xiscreen==0) {
    myprintf("Xinerama support disabled\n");
    dock->nb_xiscreen = 1;
    dock->xiscreen = calloc(1, sizeof *(dock->xiscreen));
    dock->xiscreen[0].screen_number = 0;
    dock->xiscreen[0].x_org = 0;
    dock->xiscreen[0].y_org = 0;
    dock->xiscreen[0].width = WidthOfScreen(XScreenOfDisplay(dock->display, dock->screennum));
    dock->xiscreen[0].height= HeightOfScreen(XScreenOfDisplay(dock->display, dock->screennum));
  }

  if (Prefs.auto_swallow) {
    /* create the pinnipede window in advance since the swallower may use it */
    dock->pp_win = XCreateSimpleWindow (dock->display, dock->rootwin, 
                                        0, 0, 100, 100, 0, //pp->win_width,pp->win_height, 0,
                                        BlackPixel(dock->display, dock->screennum),
                                        WhitePixel(dock->display, dock->screennum));
    kb_create_input_context_for(dock, dock->pp_win, KB_PINNIPEDE);
  } else dock->pp_win = None;


  /* set size hints 64 x 64 */
  xsh.flags = USSize | USPosition;
  xsh.width = 64;
  xsh.height = 64;
  if (Prefs.draw_border == 0) {
    XWMGeometry(dock->display, dock->screennum, "64x64+0+0", NULL, 0,
		&xsh, &xsh.x, &xsh.y, &xsh.width, &xsh.height, &i);
  } else {
    xsh.x = Prefs.dock_xpos;
    xsh.y = Prefs.dock_ypos;
  }

  /* create the application window */
  dock->win = XCreateSimpleWindow(dock->display, Prefs.auto_swallow ? dock->pp_win : dock->rootwin,
				  xsh.x, xsh.y, xsh.width, xsh.height, 0,
				  BlackPixel(dock->display, dock->screennum),
				  WhitePixel(dock->display, dock->screennum));
  
  if(!dock->win) {
    fprintf(stderr, _("Couldn't create window\n"));
    exit(1);
  }
  
  if (use_iconwin) {
    /* create icon window */
    dock->iconwin = XCreateSimpleWindow(dock->display, dock->rootwin,
					xsh.x, xsh.y, xsh.width, xsh.height, 0,
					BlackPixel(dock->display, dock->screennum),
					WhitePixel(dock->display, dock->screennum));
  
    if(!dock->iconwin) {
      fprintf(stderr, _("Couldn't create icon window\n"));
      exit(1);
    }
  } else dock->iconwin = None;
    


  /* load interface pixmap */

  if (Prefs.dock_skin_pixmap) {
    int w, h;
    dock->coinpix = RGBACreatePixmapFromXpmFile(dock->rgba_context, Prefs.dock_skin_pixmap, &w, &h);
    if (dock->coinpix != None && (h != 64 || w < 320)) {
      printf(_("Bad dimensions for the pixmap skin (expected 64x320, found %dx%d)\n"),w,h);
      exit(1);
    } else if (dock->coinpix == None) {
      myprintf(_("Unable to load the pixmap '%<grn %s>'\n"), Prefs.dock_skin_pixmap);
      exit(1);
    }
  } else {
    dock->coinpix = RGBACreatePixmapFromXpmData(dock->rgba_context, coin_xpm);
  }
  assert(dock->coinpix != None);
  
  createXBMfromXPM(coin_mask, coin_xpm, 64, 64);
  
  dock->coin_pixmask = XCreateBitmapFromData(dock->display, dock->win, 
					     coin_mask, 64, 64);

  dock->clockpix = RGBACreatePixmapFromXpmData(dock->rgba_context, clock_xpm);
  
  createXBMfromXPM(clock_mask, clock_xpm, 64, 64);
  
  dock->clock_pixmask = XCreateBitmapFromData(dock->display, dock->win, 
					     clock_mask, 64, 64);
  
  dock->led = RGBACreatePixmapFromXpmData(dock->rgba_context, led_xpm); assert(dock->led != None);
  
  dock->month = RGBACreatePixmapFromXpmData(dock->rgba_context, month_xpm); assert(dock->month != None);
  
  dock->date = RGBACreatePixmapFromXpmData(dock->rgba_context, date_xpm); assert(dock->date != None);
  
  dock->weekday = RGBACreatePixmapFromXpmData(dock->rgba_context, weekday_xpm); assert(dock->weekday != None);
  
  if (Prefs.draw_border == 0) {
    /* setup shaped window */
    XShapeCombineMask(dock->display, dock->win, ShapeBounding, 
		      0, 0, dock->coin_pixmask, ShapeSet);
    if (use_iconwin) {
      XShapeCombineMask(dock->display, dock->iconwin, ShapeBounding,
			0, 0, dock->coin_pixmask, ShapeSet);
    }

    /* set window manager hints */
    xwmh = XAllocWMHints();
    xwmh->flags = WindowGroupHint | IconWindowHint | StateHint;
    xwmh->icon_window = dock->iconwin;
    xwmh->window_group = dock->win;
    xwmh->initial_state = (use_iconwin ? WithdrawnState : NormalState);

    XSetWMHints(dock->display, dock->win, xwmh);
    
    XFree(xwmh); xwmh = NULL;
  } else {
    /*    XSetWindowAttributes wattr;
    wattr.override_redirect = True;
  
    XChangeWindowAttributes(dock->display, dock->win, CWOverrideRedirect, &wattr);
    */
    set_borderless_window_hints(dock->display, dock->win);
  }


  /* set class hints */


  xch.res_name = argv[0]; //Prefs.app_name; 
  xch.res_class = argv[0]; //Prefs.app_name; 
  /* ca chie avec KDE:
     quand KDE sauve la session, il sauve ce champ et execute ce nom
     a la session suivante, donc: 
     * rec_class doit etre le nom de l'executable
     * et la ligne de commande, je la mets ou ?
     
     --> utiliser l'option "dock.iconwin: false"
  */


  XSetClassHint(dock->display, dock->win, &xch);
  
  /* set size hints */
  XSetWMNormalHints(dock->display, dock->win, &xsh);
  
  /* tell window manager app name */
  if(!XStringListToTextProperty(&argv[0], 1, &xtp)) {
    fprintf(stderr, _("Couldn't create text property\n"));
    exit(1);
  }
  XSetWMName(dock->display, dock->win, &xtp);

  XFree(xtp.value); /* fait le faire à la main .. */

  /* create a graphics context */
  xgcv.foreground = RGB2PIXEL(0,0,0);
  xgcv.background = RGB2PIXEL(255,255,255);

  dock->NormalGC = XCreateGC(dock->display, dock->win, GCForeground | GCBackground, &xgcv);

  if(!dock->NormalGC) {
    fprintf(stderr, _("Couldn't create graphics context\n"));
    exit(1);
  }
  
  /* select events to catch */
  XSelectInput(dock->display, dock->win,
	       ExposureMask |
	       ButtonPressMask |
	       ButtonReleaseMask |
	       PointerMotionMask |
	       EnterWindowMask | 
	       LeaveWindowMask |
	       StructureNotifyMask);
  if (dock->iconwin) {
    XSelectInput(dock->display, dock->iconwin,
		 ExposureMask |
		 ButtonPressMask |
		 ButtonReleaseMask |
		 PointerMotionMask |
		 EnterWindowMask | 
		 LeaveWindowMask |
		 StructureNotifyMask);
  }

  /* set the command line for restarting */
  XSetCommand(dock->display, dock->win, argv, argc);
  
  /* map the main window */
  XMapWindow(dock->display, dock->win);
  /* affiche l'image d'initialisation (oh comme c'est userfriendly ! ) */
  XSync(dock->display, True);
  XCopyArea(dock->display, dock->coinpix, DOCK_WIN(dock), dock->NormalGC, 0, 0, 64, 64, 0, 0);
  XFlush(dock->display);

  wmcc_set_wm_icon(dock);

  swallower_init(dock);
}

#ifdef __CYGWIN__
void *Timer_Thread(void *arg UNUSED)
{
  while (1) {

    /* very very ugly hack ... 
       le but est que le coincoin ne se bloque plus sous cygwin dans
       l'appel à 'select' de la fonction 'http_select_fd'

       pour ça on l'autorise exceptionnellement à être appelée depuis
       cette thread

       oui c'est laidissime mais j'ai vraiment pas envie de me pencher
       sur les sockets non bloquants de windows
    */
    if (flag_cygwin_x_loop_in_thread && X_loop_request) {
      X_loop_request = 0;
      X_loop();
    }
    usleep (40000);
    X_loop_request++; wmcc_tic_cnt++;
  }
}
#endif 


static void
install_sighandlers()
{
  /* installation du gestionnaire de signal ALARM
     c'est lui qui permet à coincoin de continuer à clignoter pendant
     que des download sont en cours... ça n'est pas très orthodoxe comme fonctionnement,
     mais, avec quelques protections basiques, ça marche très bien, au moins sous linux
  */
#ifdef SIGNAUX_A_LANCIENNE
  assert(signal(SIGALRM, timer_signal) != SIG_ERR); /* le comportement de certains OS (solaris par ex. differe
					  differe de linux (si _XOPEN_SOURCE est défini,
					  le gestionnaire de signal n'est pas automatiquement réinstallé
					  après le déclenchement du signal ... y'a un bugfix dans X_loop,
					  mais c'est mieux d'utiliser les signaux posix (sigaction..)
				       */
  assert(signal(SIGUSR1, timer_signal) != SIG_ERR);
  assert(signal(SIGUSR2, timer_signal) != SIG_ERR);
  assert(signal(SIGPIPE, sigpipe_signal) != SIG_ERR);
  assert(signal(SIGCHLD, sigchld_signal) != SIG_ERR);
  assert(signal(SIGINT, sigint_signal) != SIG_ERR);
#else
  {
    struct sigaction action;
    action.sa_handler = timer_signal;
    sigemptyset(&(action.sa_mask));
    action.sa_flags = 0;
    if (sigaction(SIGALRM, &action, NULL) != 0) {
     fprintf(stderr,_("sigaction: erreur %d (%s)\n essayez de recompiler en faisant un #define SIGNAUX_A_LANCIENNE...\n"),
         errno, strerror(errno));
     exit(1);
    }
    if (sigaction(SIGUSR1, &action, NULL) != 0) {
     fprintf(stderr,_("sigaction: erreur %d (%s)\n essayez de recompiler en faisant un #define SIGNAUX_A_LANCIENNE...\n"),
         errno, strerror(errno));
     exit(1);
    }
    if (sigaction(SIGUSR2, &action, NULL) != 0) {
     fprintf(stderr,_("sigaction: erreur %d (%s)\n essayez de recompiler en faisant un #define SIGNAUX_A_LANCIENNE...\n"),
         errno, strerror(errno));
     exit(1);
    }
    action.sa_handler = sigpipe_signal;
    if (sigaction(SIGPIPE, &action, NULL) != 0) {
     fprintf(stderr,_("sigaction: erreur %d (%s)\n essayez de recompiler en faisant un #define SIGNAUX_A_LANCIENNE...\n"),
         errno, strerror(errno));
     exit(1);
    }

    action.sa_handler = sigchld_signal;
    if (sigaction(SIGCHLD, &action, NULL) != 0) {
     fprintf(stderr,_("sigaction: erreur %d (%s)\n essayez de recompiler en faisant un #define SIGNAUX_A_LANCIENNE...\n"),
         errno, strerror(errno));
     exit(1);
    }
    
    action.sa_handler = sigint_signal;
    if (sigaction(SIGINT, &action, NULL) != 0) {
     fprintf(stderr,_("sigaction: erreur %d (%s)\n essayez de recompiler en faisant un #define SIGNAUX_A_LANCIENNE...\n"),
         errno, strerror(errno));
     exit(1);
    }
  }
#endif
}


/* fonction non utilisee */
void
block_sigalrm(int bloque)
{
#ifdef SIGNAUX_A_LANCIENNE
#else
  {
    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGALRM);
    
    sigprocmask(bloque ? SIG_BLOCK : SIG_UNBLOCK, &sset, NULL);
  }
#endif
}

#ifdef TESTTIME
void test_time_functions() {
  time_t time1, time2, time3;
 char *theLocalTime;
 char *theUTCTime;
 char *theTime;

 extern char *tzname[2];
 extern time_t timezone;

 tzset();
 printf("Local time zone name is %s %s, ", tzname[0], tzname[1]);
 printf("%ld seconds from UTC time. daylight=%d\n", (long int)timezone, daylight);

 time1 = time( NULL );
 theLocalTime = asctime( localtime( &time1 ));
 printf("Local time is %s", theLocalTime);

 theUTCTime = asctime( gmtime( &time1 ));
 printf("UTC time is %s", theUTCTime);

 time2 = mktime( gmtime( &time1 ));
 theTime = asctime( localtime( &time2 ));
 printf("The time is %s", theTime);  
  

 str_to_time_t("2004-02-22T03:02-08:00", &time3);
 printf("convert to locatime time: %s\n", asctime( localtime( &time3 )));
 
 str_to_time_t("Sun, 22 Feb 2004 01:35:06 GMT", &time3);
 printf("convert to locatime time: %s\n", asctime( localtime( &time3 )));

 str_to_time_t("20040222132310", &time3);
 printf("convert to locatime time: %s\n", asctime( localtime( &time3 )));

 str_to_time_t("Sat, 21 Feb 2004 15:26:02 PDT", &time3);
 printf("convert to locatime time: %s\n", asctime( localtime( &time3 )));

 str_to_time_t("Sun, 7 Mar 2004 14:27:52 -0800", &time3);
 printf("convert to locatime time: %s\n", asctime( localtime( &time3 )));
 
 str_to_time_t("Tue, 09 Mar 2004 00:34:39 +0100", &time3); // heure locale == 00:34:39
 printf("convert to locatime time: %s\n", asctime( localtime( &time3 )));
 char tstamp[15];
 time_t_to_tstamp(time3, tstamp);
 printf("tstamp: %s\n", tstamp);
 exit(0);
}
#endif
int x_error_handler_debug(Display *dpy UNUSED, XErrorEvent *err UNUSED) {
  fprintf(stderr, "X11 error\n");
  dump_backtrace();
  abort();
}

int main(int argc, char **argv)
{
  Dock *dock;
#ifdef __CYGWIN__
  pthread_t timer_thread;
#endif
  //setlocale (LC_ALL, "");
  umask(077); /* allez hop */
  /* on peut forcer la locale sans faire d'export LC_MESSAGES=blahblah
     avec l'option -l */
  {
    const char *locale_dir = LOCALEDIR;
    int i;
    for (i=1; i < argc-1; i++) {
      if (strcmp(argv[i],"-l")==0) {
	setlocale(LC_MESSAGES, argv[i+1]);
	//	printf("locale changed to '%s' (%s)\n",argv[i+1],setlocale (LC_MESSAGES, NULL));
      }
      if (strcmp(argv[i],"-L")==0) {
	locale_dir = argv[i+1];
	//	printf("locale changed to '%s' (%s)\n",argv[i+1],setlocale (LC_MESSAGES, NULL));
      }
    }
    bindtextdomain (PACKAGE, locale_dir);
  }

  textdomain (PACKAGE);

  tzset();


#ifdef TESTTIME
  test_time_functions();
#endif

  /* la structure de base */
  ALLOC_OBJ(dock, Dock); dock->sites = NULL;

  _dock = dock; /* la vilaine variable globale (pour les sighandlers) */

  myprintf(_("%<GRN wmc2> v.%<WHT %s>\n"),VERSION);

  /* contruit un useragent qui ignore la partie terminale du numero de version
     ( '2.3.6f' --> '2.3.6') */
  {
    int i;
    app_useragent = strdup("wmcoincoin/" VERSION);
    i = strlen(app_useragent)-1;
    while (app_useragent[i] < '0' || app_useragent[i] > '9') app_useragent[i--] = 0;
  }
    
  printf(_("locale used: %s\n"), setlocale (LC_MESSAGES, NULL));
  
  dock->fuck_utf8 = 0;
  {
#ifdef CODESET
    char *codeset = nl_langinfo(CODESET);
    printf("codeset : %s\n", codeset);
    if (codeset && str_case_str(codeset, "utf")) {
      printf("utf8 puxor!\n");
      dock->fuck_utf8 = 1;
    }
#endif
  }

  memset(&Prefs, 0, sizeof(Prefs));
  wmcc_prefs_initialize(argc, argv, &Prefs);
  
  dock->sites = sl_create();

  ccqueue_build();

  if (Prefs.debug & 1) {
    XSetErrorHandler(x_error_handler_debug);
    _Xdebug = 1; /* oblige la synchronisation */
  } else {
    XSetErrorHandler(x_error_handler);
    XSetIOErrorHandler(x_io_error_handler);
  }


  {
    Site *s;
    myprintf("Site         Locale     Backend type       Pants\n");
    for (s = dock->sites->list; s; s = s->next) {
      myprintf("%<YEL %10s>   \t", s->prefs->site_name);
      switch (s->prefs->locale) {
      case locEN: myprintf("EN\t"); break;
      case locFR: myprintf("FR\t"); break;
      default: myprintf("BUG!!!!"); break;
      }
      if (s->prefs->check_board) {
	switch (s->prefs->backend_type) {
	case BACKEND_TYPE_BOARD:
          myprintf(_("%<YEL BOARD      >")); break;
        case BACKEND_TYPE_RSS:  
          myprintf(_("%<YEL RSS        >")); break;
        case BACKEND_TYPE_POP:
          myprintf(_("%<YEL POP3       >")); break;          
	default: myprintf(_("%<YEL prrrrrrt    >")); break;
	}
        switch (s->prefs->backend_flavour) {
        case BACKEND_FLAVOUR_UNENCODED:
          myprintf(_("%<YEL tags not encoded  >")); break;
	case BACKEND_FLAVOUR_ENCODED:
          myprintf(_("%<YEL tags encoded      >")); break;
	case BACKEND_FLAVOUR_NO_PANTS:  
          myprintf(_("%<YEL without underpants>")); break;
        default: myprintf(_("%<YEL prrrrrrt    >")); break;
	}
      } else {
	myprintf("none              ");
      }
      //      myprintf("    %3s     ", s->prefs->check_news ? "Yes" : "No");
      //myprintf("    %3s     ", s->prefs->check_comments ? "Yes" : "No");
      //myprintf("    %3s     ", s->prefs->check_messages ? "Yes" : "No");
      myprintf("\n");
    }
  }

  ALLOC_VEC(dock->newstitles,MAX_NEWSTITLES_LEN, unsigned char);
  #if 0
  ALLOC_VEC(dock->newstitles_id,MAX_NEWSTITLES_LEN, id_type);
  {
    int i;
    for (i=0; i < MAX_NEWSTITLES_LEN; i++) dock->newstitles_id[i]= id_type_invalid_id();
  }
#endif
  ALLOC_VEC(dock->msginfo,MAX_MSGINFO_LEN, unsigned char);
  /* le trolloscope */
  {
    int i,j;
    ALLOC_ARR(dock->trolloscope, TROLLOSCOPE_HEIGHT, TROLLOSCOPE_WIDTH, TL_item);
    for (i=0; i < TROLLOSCOPE_HEIGHT; i++) {
      for (j=0; j < TROLLOSCOPE_WIDTH; j++) {
	dock->trolloscope[i][j].id = id_type_invalid_id();
      }
    }
  }

  /* recopie du message et du useragent des preferences */
  strncpy(dock->coin_coin_message, Prefs.coin_coin_message, MESSAGE_MAXMAX_LEN); 
  dock->coin_coin_message[MESSAGE_MAXMAX_LEN] = 0;
  free(Prefs.coin_coin_message); Prefs.coin_coin_message = NULL; //dock->coin_coin_message; /* pas beau */

  dock->coin_coin_site_id = -1;

  /* pour les http_get, on utilisera Prefs.user_agent, qui est non modifiable
     pour les http_post, on utilisera dock->coin_coin_useragent */

  dock->tl_item_survol = NULL;
  dock->trolloscope_resolution = 5;

  dock->flamometre.xp_change_decnt = 0;
  dock->flamometre.comment_change_decnt = 0;
  dock->flamometre.board_answer_decnt = 0;

  /*  dock->trolloscope_bgr = dock->trolloscope_bgb = dock->trolloscope_bgg = 0;
      dock->trolloscope_clign_step = -1;*/

  dock->view_id_in_newstitles = id_type_invalid_id();
  dock->flag_survol_trollo = 0;
  dock->flag_survol_led1 = 0;
  dock->flag_trib_load_cursor = 0;
  dock->mouse_x = 0; 
  dock->mouse_y = 0;
  dock->mouse_cnt = 0;
  dock->mouse_win = None;
  dock->trolloscope_speed = Prefs.default_trollo_speed;
  dock->discretion_saved_state.palmipede_used = 0;
  dock->discretion_saved_state.pinnipede_used = 0;
  dock->discretion_saved_state.last_sig_is_usr1 = 0;
  dock->horloge_mode = 0;

  dock->mask_porte_haut = None;
  dock->mask_porte_bas = None;

  dock->trib_trollo_rate = 0; dock->trib_trollo_score = 0;

  {
    char *errmsg;
    char mask[64*64];
    
    XGCValues xgcv;
    
    createXBMfromXPM(mask, coin_xpm, 64, 64);
    
    initx(dock,argc, argv);


    if ((errmsg=dock_build_pixmap_porte(dock))) {
      fprintf(stderr, "%s", errmsg);
    }


    dock_leds_create(dock, &dock->leds);

    {
      char fn[512];
      snprintf(fn, 512, "%s-%s", DOCK_FIXED_FONT, Prefs.font_encoding);
      dock->fixed_font = XLoadQueryFont(dock->display, fn);
      if (!dock->fixed_font) {
	myfprintf(stderr, _("Unable to load font %s , trying iso8859-1\n"), fn);
	snprintf(fn, 512, "%s-iso8859-1", DOCK_FIXED_FONT);
	dock->fixed_font = XLoadQueryFont(dock->display, fn);
	if (!dock->fixed_font) {
	  myfprintf(stderr, _("Unable to load font %s, aborting\n"), fn);
	  exit(-1);
	}
      }
    }

    
    dock->pix_trolloscope = XCreatePixmap(dock->display, dock->coinpix, TROLLOSCOPE_WIDTH, TROLLOSCOPE_HEIGHT, DefaultDepth(dock->display,dock->screennum));
    XFillRectangle(dock->display, dock->pix_trolloscope, dock->NormalGC, 0, 0, TROLLOSCOPE_WIDTH, TROLLOSCOPE_HEIGHT);

    xgcv.function = GXor;
    xgcv.graphics_exposures = 0;
    dock->or_GC = XCreateGC(dock->display, dock->coinpix, GCFunction| GCGraphicsExposures, &xgcv);

    xgcv.function = GXand;
    xgcv.graphics_exposures = 0;
    dock->and_GC = XCreateGC(dock->display, dock->coinpix, GCFunction| GCGraphicsExposures, &xgcv);
  }
  /* selection du curseur affiche qd on survole la trib_load */
  //  dock->trib_load_cursor = XCreateFontCursor(dock->display, XC_target);
  //dock->trib_load_cursor = XCreateFontCursor(dock->display, XC_plus);
  dock->trib_load_cursor = XCreateFontCursor(dock->display, XC_tcross);

  dock->atom_WM_DELETE_WINDOW = XInternAtom(dock->display, "WM_DELETE_WINDOW", False);
  dock->atom_WM_SAVE_YOURSELF = XInternAtom(dock->display, "WM_SAVE_YOURSELF", False);
  dock->atom_WM_TAKE_FOCUS = XInternAtom(dock->display, "WM_TAKE_FOCUS", False);
  dock->atom_WM_PROTOCOLS = XInternAtom(dock->display, "WM_PROTOCOLS", False);
  
  scrollcoin_build(dock->rgba_context);
  totoz_bookmark_load(dock);
  balloon_build(dock);
  msgbox_build(dock);
  editw_build(dock);
  plopup_build(dock);
  pp_build(dock);
  balltrap_build(dock);
  if (Prefs.auto_swallow)
    swallower_autoswallow(dock);
  //sw_swallow_win_id(dock,0x3200001);

  dock->door_state = CLOSED; dock->door_state_step = 0;
  dock->red_button_press_flag = 0; dock->red_button_press_state = 0;
  dock->post_anonyme = 0;

  dock->newstitles_pos = 0; 
  dock->newstitles_char_dec = 0;
  dock->newstitles[0] = 0;

  if (Prefs.start_in_boss_mode == 0) {
    dock_refresh_normal(dock);
  } else {
    dock_set_horloge_mode(dock);
    dock_refresh_horloge_mode(dock);
  }

  //kb_create_input_context_for(dock, DOCK_WIN(dock));

  dock->wmccc_pid = -1;

  /* essaye de restorer la taille / position du pinnipede / newswin */
  wmcc_save_or_restore_state(dock, 1);

  if (Prefs.pinnipede_open_on_start == 1 || Prefs.auto_swallow) {
    pp_show (dock);
  }

#ifndef NOSIGNALS
  install_sighandlers();
#endif


#ifdef __CYGWIN__ 
  /* signals sux under cygwin */
  pthread_create (&timer_thread, NULL, Timer_Thread, NULL);
#else
#  ifndef NOSIGNALS
  {
    struct itimerval the_timer;
    the_timer.it_interval.tv_sec = 0;
    the_timer.it_interval.tv_usec = WMCC_TIMER_DELAY_MS * 1000; /* microsecondes */
    the_timer.it_value = the_timer.it_interval;

    setitimer(ITIMER_REAL, &the_timer, NULL);
  }
#  endif
#endif  
  /* launching the network update thread */

  ccqueue_loop(dock);

  return 0;
}
