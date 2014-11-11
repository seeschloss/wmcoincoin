/*

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
  rcsid=$Id: palmipede.c,v 1.30 2005/09/27 16:59:13 pouaite Exp $
  ChangeLog:
  $Log: palmipede.c,v $
  Revision 1.30  2005/09/27 16:59:13  pouaite
  2.5.1c

  Revision 1.29  2005/09/26 21:40:24  pouaite
  v 2.5.1b

  Revision 1.28  2005/09/25 12:08:55  pouaite
  ca marche encore ca ?

  Revision 1.27  2005/06/11 22:47:41  pouaite
  prout?

  Revision 1.26  2005/02/22 18:45:32  pouaite
  *** empty log message ***

  Revision 1.25  2004/05/16 12:54:29  pouaite
  250c

  Revision 1.24  2004/04/28 22:19:00  pouaite
  bugfixes dae + des trucs que j'ai oublie

  Revision 1.23  2004/04/26 20:32:31  pouaite
  roger demande le commit

  Revision 1.22  2004/04/18 15:37:28  pouaite
  un deux un deux

  Revision 1.21  2004/03/07 13:51:12  pouaite
  commit du dimanche

  Revision 1.20  2004/02/29 19:01:27  pouaite
  et hop

  Revision 1.19  2004/02/29 15:01:19  pouaite
  May the charles bronson spirit be with you

  Revision 1.18  2003/08/26 21:50:48  pouaite
  2.6.4b au mastic

  Revision 1.17  2003/07/20 22:22:28  pouaite
  ce commit est dedie a Pierre Tramo

  Revision 1.16  2003/06/29 23:58:39  pouaite
  suppression de l'overrideredirect du palmi et ajout de pinnipede_totoz.c et wmcoincoin-totoz-get etc

  Revision 1.15  2003/06/25 20:18:21  pouaite
  support xinerama qui marche

  Revision 1.14  2003/06/24 22:27:57  pouaite
  speciale dedicace a nos amis de l'ile de beaute

  Revision 1.13  2003/06/23 22:43:47  pouaite
  shift-enter pour le post anonyme + fix ouinouin

  Revision 1.12  2003/06/22 12:17:19  pouaite
  2.4.5a a la piscine

  Revision 1.11  2003/06/21 14:48:45  pouaite
  g cho

  Revision 1.10  2003/06/09 16:42:29  pouaite
  pan pan

  Revision 1.9  2003/03/01 17:31:22  pouaite
  compat ipv6 a tester

  Revision 1.8  2003/01/19 18:52:23  pouaite
  patch gle (couleur de fond du palmi)

  Revision 1.7  2003/01/11 14:10:07  pouaite
  fix du palmi pour xf 4.3

  Revision 1.6  2002/12/20 11:26:35  pouaite
  deux trois conneries

  Revision 1.5  2002/10/05 18:08:14  pouaite
  ajout menu contextuel + fix de la coloration des boutons du wmccc

  Revision 1.4  2002/09/07 16:21:15  pouaite
  ?a va releaser en douce

  Revision 1.3  2002/08/21 01:11:49  pouaite
  commit du soir, espoir

  Revision 1.2  2002/08/18 00:29:30  pouaite
  en travaux .. prière de porter le casque

  Revision 1.1  2002/08/17 18:54:15  pouaite
  ajout des fichiers manquants

  Revision 1.21  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.20  2002/05/20 16:01:25  pouaite
  nouveau raccourci alt-F du palmipede

  Revision 1.19  2002/04/01 22:56:03  pouaite
  la pseudo-transparence du pinni, bugfixes divers, option tribune.backend_type

  Revision 1.18  2002/04/01 01:39:38  pouaite
  grosse grosse commition (cf changelog)

  Revision 1.17  2002/03/28 00:06:15  pouaite
  le clic sur un login ouvre le palmipede en remplissant '/msg lelogin '

  Revision 1.16  2002/03/19 09:55:58  pouaite
  bugfixes compilation

  Revision 1.15  2002/03/18 22:46:49  pouaite
  1 ou 2 bugfix mineurs, et surtout suppression de la dependance avec la libXpm .. un premier pas vers wmc² en 8bits

  Revision 1.14  2002/03/09 00:25:12  pouaite
  coin coin

  Revision 1.13  2002/03/08 23:53:40  pouaite
  derniers bugfixes pour la v2.3.6

  Revision 1.12  2002/03/03 10:10:04  pouaite
  bugfixes divers et variés

  Revision 1.11  2002/03/01 00:27:40  pouaite
  trois fois rien

  Revision 1.10  2002/02/24 22:13:57  pouaite
  modifs pour la v2.3.5 (selection, scrollcoin, plopification, bugfixes)

  Revision 1.9  2002/01/19 19:56:09  pouaite
  petits crochets pour la mise en valeur de certains messages (cf changelog)

  Revision 1.8  2002/01/13 18:19:20  pouaite
  nouvelles suggestions pour clipouille (merci jjb!)

  Revision 1.7  2002/01/12 19:03:54  pouaite
  bugfix de picohtml et raccourci altgr-e pour le symbole euro (gruik)

  Revision 1.6  2002/01/12 17:29:08  pouaite
  support de l'iso8859-15 (euro..)

  Revision 1.5  2001/12/18 12:43:37  pouaite
  ajout de l'option de la fonte des ballons d'aide (pour mr. imr !) + bugfix d'une connerie assez naze dans la gestion du nom du fichier d'options (merci glandium de me l'avoir signalé)

  Revision 1.4  2001/12/16 20:28:45  pouaite
  bugfixes divers

  Revision 1.3  2001/12/16 16:46:12  pouaite
  Clippouille joins C0IN C0IN

  Revision 1.2  2001/12/02 18:34:54  pouaite
  ajout de tags cvs Id et Log un peu partout...

*/

/*
  ceci est le palmipede editor
*/


#include <libintl.h>
#define _(String) gettext (String)

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xmu/Atoms.h>

#include <X11/Xmd.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include "coin_util.h"
#include "coin_xutil.h"
#include "coincoin.h"
#include "fontcoincoin.h"
#include "kbcoincoin.h"
#include "../xpms/editwin_minib.xpm"
#include "../xpms/clippy.xpm"
#include "spell_coin.h" 
#include "dock.h"
#include "site.h"
#include "board_util.h" // pour le boards_find_id de l'antibloub

#define FN_W 6
#define FN_H 11
#define FN_BASE_H 9
//#define FN_W (ew->fn_w)
//#define FN_H (ccfont_height(ew->fn))
//#define FN_BASE_H (ccfont_ascent(ew->fn))
//#define EW_FONT "-*-fixed-*--10-*"
#define EW_FONT "monospace:pixelsize=10"
#define EW_NCOL 64
#define EW_NROW 4

#define EW_TXT_XBORD_WIDTH 8
#define EW_TXT_Y0 2
#define EW_TXT_WIDTH  (EW_NCOL*FN_W)
#define EW_TXT_HEIGHT (EW_NROW*FN_H)

#define EW_SHAPE_SPACEX 14
#define EW_SHAPE_SPACEY 8
#define EW_WIDTH (EW_TXT_WIDTH + 2*EW_TXT_XBORD_WIDTH)
#define EW_HEIGHT 59
#define EW_SHAPED_WIDTH (EW_WIDTH + EW_SHAPE_SPACEX)
#define EW_SHAPED_HEIGHT (EW_HEIGHT + EW_SHAPE_SPACEY)
#define EW_XBORD (ew->dock_side==RIGHT ? 0 : EW_SHAPE_SPACEX)
#define EW_TXT_X0 (EW_XBORD+EW_TXT_XBORD_WIDTH)

#define EW_MAX_TOTAL_NROW 20

enum { BT_CLOSE=0, BT_CHANGE, BT_ITAL, BT_BOLD, BT_STRIKE, BT_UNDERLINE,
       BT_UNDO, BT_CLEAR, BT_DEFAULTUA, BT_TT, BT_TOTOZBOOKMARK, NB_MINIBT };

#define EWC_NORMAL 0
#define EWC_LONGWORD 1
#define EWC_URL 2
#define EWC_KNOWN_URL 3 // todo un jour.. UPDATE: done!!
#define EWC_BALISE 4
#define EWC_SPELLWORD 5
#define EWC_NBATTR 6

#ifdef SWAP
#undef SWAP
#endif

#define SWAP(a,b,t) { t tmp; tmp = (t)a; a = (t)b; b = (t)tmp; }

/* utilise pour le copier coller */
typedef CARD32 Atom32;

typedef struct _MiniBouton {
  int pix_x, pix_y;
  int x,y,w,h;
  int visible;
  int enfonce;
} MiniBouton;

typedef enum {CUTPASTE, INSERT_CHAR, DELETE_CHAR, OTHER} EditWCommandClass;

struct _EditW {
  Window win;
  Pixmap pix;
  CCFontId fn; //XFontStruct *fn;
  unsigned char *buff;

  int  buff_sz;

  int  mapped;

  int buff_len;

  int sel_anchor, sel_head; /* premier et [dernier+1] numero de char de la selection ( a classer dans l'ordre)*/
  
  int y_scroll;
  int curs_x, curs_y;

  struct {
    unsigned char *buff;
    int y_scroll;
    int curs_x, curs_y;
  } undo;

  /* pour le word-wrap */
  int row_firstchar[EW_MAX_TOTAL_NROW+1]; 
  int nrow_used;

  CCColorId win_bgcolor, dark_color, light_color;
  CCColorId txt_fgcolor[EWC_NBATTR], txt_bgcolor;
  CCColorId cur_fgcolor, cur_bgcolor;
  CCColorId sel_fgcolor, sel_bgcolor;
  CCColorId fill_bgcolor;
  MiniBouton mini[NB_MINIBT];
  Pixmap minipix;

  enum {NOACTION, SORTIR, RENTRER} action;
  int action_step;

  int dock_x, dock_y;
  enum {LEFT,RIGHT} dock_side;

  int win_xpos, win_ypos;

  int buff_num; /* 0 -> dock->coin_coin_message,
		   1 -> dock->coin_coin_ueragent */

  /* utilise pour le undo */
  EditWCommandClass last_command;

  Pixmap clippy_pixmap;
  int clippy_w, clippy_h;

  id_type antibloub_id; /* id du message ayant poste l'url bloubesque */
  int antibloub_cnt;    /* nb de fois ou l'url bloublesque a ete postee */
  int antibloub_need_clipouille; /* pas mal comme nom de variable :) */
  SitePrefs *prefs;
};

#define IS_SEP(c,nc) (((c) <=' ') || ((c)=='.') || ((c)==',')  || \
                      ((c) ==';') || ((c)==':') || ((c)=='!')  || ((c)=='?') || \
                      ((nc)=='(') || ((c)==')') || \
                      ((nc)=='{') || ((c)=='}') || \
                      ((nc)=='<') || ((c)=='>'))

#define IS_SEP2(c)   (((c) <=' ') || ((c)=='.') || ((c)==',')  || \
                      ((c) ==';') || ((c)==':') || ((c)=='!')  || ((c)=='?') || \
                      ((c)=='(') || ((c)==')') || \
                      ((c)=='{') || ((c)=='}') || \
                      ((c)=='<') || ((c)=='>'))


static unsigned char *cb_buffer = NULL;

int *head,*anch;


/* (x,y) = position dans le tableau EW_NCOL x EW_NROW */
static int
editw_xy2strpos(EditW *ew, int x, int y)
{
  int i;

  if (x < 0 || x >= EW_NCOL) {
    myprintf(_("%<MAG> FUCK THAT BUG! x=%d y=%d curs_x=%d curs_y=%d,y_scroll=%d,sel_anchor=%d,sel_head=%d\n"),
	     x,y,ew->curs_x,ew->curs_y,ew->y_scroll,ew->sel_anchor,ew->sel_head);
  }
    //  assert(x >= 0 && x < EW_NCOL);
  assert(y >= 0 && (y-ew->y_scroll) < EW_NROW);
  
  i = ew->row_firstchar[y] + x;
  if (i > ew->row_firstchar[y+1]) i =  ew->row_firstchar[y+1];
  
  return i;
}

static void
editw_strpos2xy(EditW *ew, int pos, int *x, int *y)
{
  *y = 0;
  assert(pos <= ew->buff_len);

  while (ew->row_firstchar[*y+1] < pos) (*y)++;
  (*x) = pos - ew->row_firstchar[*y];

  /* cas particulier: la ligne est de longueur EW_NCOL */
  if ((*x) == EW_NCOL) {
    *x = 0;
    (*y)++;
  }
  assert(*x < EW_NCOL);
}

static void editw_adjust_yscroll(EditW *ew) {
  if (ew->curs_y >= ew->y_scroll + EW_NROW) {
    ew->y_scroll =  ew->curs_y - (EW_NROW - 1);
  } else if (ew->curs_y < ew->y_scroll) {
    ew->y_scroll = ew->curs_y;
  }
  //  printf("yscroll=%d\n",ew->y_scroll);
}

static void editw_strpos2curs(EditW *ew, int pos)
{
  editw_strpos2xy(ew, pos, &ew->curs_x, &ew->curs_y);
  editw_adjust_yscroll(ew);
}

/* effectue le word-wrap en s'assurant que le curseur désigne toujours le bon caractère */
static void
editw_wordwrap(EditW *ew)
{
  int l, i, j, curs_pos;

  ew->buff_len = strlen(ew->buff);

  /* il faudra recalculer la pos du curseur si ca wrappe */
  curs_pos = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
  curs_pos = MIN(curs_pos, ew->buff_len);
  
  i = 0;
  ew->row_firstchar[0] = i;

  i = IS_SEP('e','g');
  ew->nrow_used = 0;
  l = 1;
  for (l=1; l < EW_MAX_TOTAL_NROW+1; l++) {
    if (i < ew->buff_len || i == 0) {
      /* nb de ligne utilisees pour l'affichage (avec cas pasticulier quand
	 la chaine est vide) */
      ew->nrow_used++; 
    }
    i += EW_NCOL;
    if (i >= ew->buff_len) {
      ew->row_firstchar[l] = ew->buff_len;
    } else {
      j = i-1;
      /* on essaye de couper la ligne precedente au bon endroit,
       en cherchant un separateur parmi les 15 dernier caracteres*/
      while (!IS_SEP(ew->buff[j], ew->buff[j+1])) {
	j--;
	if (j < 0) { j = 0; break; }
	if (j < i-15) break;
      }

      ew->row_firstchar[l] = ew->buff_len;
      if (ew->buff[j]) {
	if (IS_SEP(ew->buff[j], ew->buff[j+1])) {
	  i = ew->row_firstchar[l] = j+1;
	} else {
	  ew->row_firstchar[l] = i;
	}
      }
    }
    //    printf("row_firstchar[%d] = %d\n", l, ew->row_firstchar[l]);
  }

  /* cas particulier: la derniere ligne fait exactement EW_NCOL char
     donc on rajoute une ligne vide en dessous */
  if (ew->row_firstchar[ew->nrow_used] - ew->row_firstchar[ew->nrow_used-1] == EW_NCOL &&
      ew->nrow_used < EW_MAX_TOTAL_NROW) {
    assert(ew->buff[ew->row_firstchar[ew->nrow_used]] == 0);
    ew->nrow_used++;
    //    ew->row_firstchar[ew->nrow_used] = ew->row_firstchar[ew->nrow_used-1];
  }

  /* cas particulier, EW_MAX_TOTAL_NROW trop faible,
     ca ne devrait jamais arriver. au cas on, on tronque comme une bete */
  if (i < ew->buff_len) ew->buff[i] = 0;

  editw_strpos2curs(ew, curs_pos);
}

static
void editw_get_sel_bounds(EditW *ew, int *sel_start, int *sel_end)
{
  if (ew->sel_anchor <= ew->sel_head) {
    *sel_start = ew->sel_anchor;
    *sel_end = ew->sel_head;
  } else {
    *sel_end = ew->sel_anchor;//-1;
    *sel_start = ew->sel_head;
  }
}


int
editw_has_selection(EditW *ew) 
{
  int sel_start;
  int sel_end;

  editw_get_sel_bounds(ew, &sel_start, &sel_end);
  return (sel_end != sel_start);
}


static void
editw_unselect(EditW *ew)
{
  ew->sel_head = ew->sel_anchor = -1;
}

/* efface le cut-buffer */
static void
editw_cb_clear()
{
  if (cb_buffer) {
    free(cb_buffer); cb_buffer = NULL;
  }
}

/*
static char *cmd_str(EditWCommandClass cmd) {
  switch (cmd) {
  case CUTPASTE: return "CUTPASTE";
  case INSERT_CHAR: return "INSERT_CHAR";
  case DELETE_CHAR: return "DELETE_CHAR";
  case OTHER: return "OTHER";
  }
  return NULL;
  }*/
/* pour pouvoir faire un undo */
static void
editw_save_state(EditW *ew, EditWCommandClass cmd) {
  if ((cmd == CUTPASTE) ||
      (cmd == INSERT_CHAR && ew->last_command != INSERT_CHAR) ||
      (cmd == DELETE_CHAR && ew->last_command != DELETE_CHAR)) {
    //    printf("save STATE %s -> %s\n", cmd_str(ew->last_command), cmd_str(cmd));
    strcpy(ew->undo.buff, ew->buff);
    ew->undo.y_scroll = ew->y_scroll;
    ew->undo.curs_x = ew->curs_x;
    ew->undo.curs_y = ew->curs_y;
  }
  ew->last_command = cmd;
}

static void
editw_remove_txt(EditW *ew, int i0, int i1)
{
  if (i0 < i1) {
    assert(i1 <= (int)strlen(ew->buff));

    if (i1-i0 > 1) {
      editw_save_state(ew, CUTPASTE);
    } else {
      editw_save_state(ew, DELETE_CHAR);
    }

    //    printf("delete %d %d\n",i0,i1);
    memmove(ew->buff + i0, ew->buff+i1, strlen(ew->buff+i1)+1);
    editw_wordwrap(ew);
  }
}

static void
editw_erase_selection(EditW *ew)
{
  int i0, i1;
  
  if (editw_has_selection(ew)) {
    editw_get_sel_bounds(ew, &i0, &i1);
    editw_unselect(ew);
    editw_remove_txt(ew, i0, i1);
    editw_strpos2curs(ew, i0);
  }
}

/* efface tout le buffer si il n'y a pas de selection active */
void
editw_erase(EditW *ew)
{
  if (editw_has_selection(ew)) {
    editw_erase_selection(ew);
  } else {
    editw_unselect(ew);
    editw_remove_txt(ew, 0, strlen(ew->buff));
    editw_strpos2curs(ew, 0);
  }
  { int x = ew->curs_x, y = ew->curs_y;
  if (x < 0 || x >= EW_NCOL) {
    myprintf(_("%<MAG> RAH ENCORE CE BUG! x=%d y=%d curs_x=%d curs_y=%d,y_scroll=%d,sel_anchor=%d,sel_head=%d\n"),
	     x,y,ew->curs_x,ew->curs_y,ew->y_scroll,ew->sel_anchor,ew->sel_head);
  }}
}


/* renvoie 0 si pas de pb, longueur tronquee si tronquage */
int
editw_insert_string(EditW *ew, const unsigned char *str)
{
  char *s = strdup(str);
  convert_to_iso8859("UTF-8", &s);
  int ls,lb,lrest,ldec;
  int tronq;

  if (strlen(s)>1) {
    editw_save_state(ew, CUTPASTE);
  } else {
    editw_save_state(ew, INSERT_CHAR);
  }

  editw_erase_selection(ew);

  lb = strlen(ew->buff);
  ls = strlen(s);
  lrest = ew->buff_sz-1-ls-lb;

  tronq = 0;

  if (lrest < 0) {
    tronq = -lrest;
    ls += lrest;
    lrest = 0;
  }
  assert(ls >= 0);
  //  printf("ls=%d",ls);
  if (ls > 0) {
    int i,j;
    i = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
    ldec = MIN(lb - i + 1, ew->buff_sz-1-i-ls); assert(ldec>=0);
    //    printf(" ldec=%d\n",ldec);
    if (ldec) {
      memmove(ew->buff + i + ls, ew->buff+i, ldec);
    }
    assert(ew->buff[ew->buff_sz-1] == 0);
    for (j=0; j < ls; j++) 
      ew->buff[i+j] = (s[j] >= ' ' ? s[j] : ' ');
    //    memmove(ew->buff + i, s, ls);
    editw_wordwrap(ew);
    editw_strpos2curs(ew, i+ls);
  }

  return tronq;
  /*
  l = MIN(strlen(s), ew->buff_sz-1-strlen(s)-strlen(ew->buff));
  if (l > 0) {
    int i;
    int l2;
    i = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
    l2 = MIN(strlen(ew->buff+i)+1, ew->buff_sz-1-l-i); 
    memmove(ew->buff + i + l, ew->buff+i, l2);
    assert(ew->buff[ew->buff_sz-1] == 0);
    memmove(ew->buff + i, s, l);
    //    printf("'%s' [%s]\n", ew->buff, s);
    editw_wordwrap(ew);
    editw_strpos2curs(ew, i+l);
    }*/
}



static void
editw_insert_char(EditW *ew, unsigned char c)
{
  char s[2];
  s[0] = c; s[1] = 0;
  editw_insert_string(ew, s);
}

/* X clipboard function -- stolen from wmpinboard  Copyright (C) 1998-2000 by Marco G"otze. */



/*
 * copies <text> to the X clipboard
 */
void
editw_cb_copy(Dock *dock, Window win, const char *text, int len)
{
  int l;

  if (!text) return;

  if (cb_buffer) free(cb_buffer);
  l = len < 0 ? (int)strlen(text) : len;
  cb_buffer = malloc(l+1); assert(cb_buffer);
  strncpy(cb_buffer, text, l);
  cb_buffer[l] = 0;

  XSetSelectionOwner(dock->display, XA_PRIMARY, win, CurrentTime);
  if (XGetSelectionOwner(dock->display, XA_PRIMARY) != win) {
    fprintf(stderr, _("wmcoincoin: Failed to set XA_PRIMARY ownership."));
    XChangeProperty(dock->display, dock->rootwin, XA_CUT_BUFFER0,
		    XA_UTF8_STRING(dock->display), 8, PropModeReplace, cb_buffer, l);
  }
}


/*
 * responds to a SelectionRequest event
 * [once again, thanks to the RXVT source]
 */
void
editw_cb_handle_selectionrequest(Dock *dock, XSelectionRequestEvent *rq)
{
  XEvent ev;
  Atom32 target_list[2];
  static Atom xa_targets = None;

  if (xa_targets == None) xa_targets = XInternAtom(dock->display, "TARGETS", 0);

  ev.xselection.type = SelectionNotify;
  ev.xselection.property = None;
  ev.xselection.display = rq->display;
  ev.xselection.requestor = rq->requestor;
  ev.xselection.selection = rq->selection;
  ev.xselection.target = rq->target;
  ev.xselection.time = rq->time;

  if (rq->target == xa_targets) {
    target_list[0] = (Atom32) xa_targets;
    target_list[1] = (Atom32) XA_UTF8_STRING(dock->display);
    XChangeProperty(dock->display, rq->requestor, rq->property, rq->target,
      8*sizeof(target_list[0]), PropModeReplace,
      (unsigned char*) target_list,
      sizeof(target_list)/sizeof(target_list[0]));
    ev.xselection.property = rq->property;
  } else if (rq->target == XA_UTF8_STRING(dock->display)) {
    XChangeProperty(dock->display, rq->requestor, rq->property, rq->target,
      8, PropModeReplace, cb_buffer, strlen((char*) cb_buffer));
      ev.xselection.property = rq->property;
  }
  XSendEvent(dock->display, rq->requestor, 0, 0, &ev);
}

/*
 * pastes the current contents of the clipboard into <note> at <pos>, inserting
 * or overwriting depending on <ins>, trying to word-wrap unless <raw>; moves
 * the cursor
 */
static void
editw_cb_paste_external(Dock *dock, EditW *ew, Window window, Atom prop, int Delete)
{
  unsigned long bytes_after, nitems;
  unsigned char *data;
  Atom actual_type;
  int actual_fmt;

  data = NULL;
  if (prop == None) return;
  /* 256 -> longueur max qu'on copy est 256*4 */
  if ((XGetWindowProperty(dock->display, window, prop, 0, 256, Delete,
    AnyPropertyType, &actual_type, &actual_fmt, &nitems, &bytes_after,
    &data) != Success))
  {
    XFree(data);
    return;
  }
  if (nitems && data) {
    editw_unselect(ew);
    editw_insert_string(ew,data);
  }
  if (data)  XFree(data);
}

/*
 * handle's the user's request to paste text into a note
 */
static void
editw_cb_paste(Dock *dock, EditW *ew, int external_only)
{
  Atom prop;

  if (cb_buffer && external_only == 0) {
    editw_unselect(ew);
    editw_insert_string(ew, cb_buffer); 
  } else if (XGetSelectionOwner(dock->display, XA_PRIMARY) == None) {
    editw_cb_paste_external(dock, ew, dock->rootwin, XA_CUT_BUFFER0, 0);
  } else {
    prop = XInternAtom(dock->display, "VT_SELECTION", 0);
    XConvertSelection(dock->display, XA_PRIMARY, XA_UTF8_STRING(dock->display), prop, ew->win, CurrentTime);
  }
}


/* end of stolen function from wmpinboard */



static void
editw_cut_selection(Dock *dock, EditW *ew)
{
  int i0, i1;
  
  if (editw_has_selection(ew)) {
    editw_get_sel_bounds(ew, &i0, &i1);
    editw_cb_copy(dock, ew->win, ew->buff+i0, i1-i0);
    editw_remove_txt(ew, i0, i1);
    editw_strpos2curs(ew, i0);
    editw_unselect(ew);
  }
}

/* renvoie le numero du minibouton contenant le pixel (x,y)
   -1 sinon
*/
static int
editw_xy2minibutton(EditW *ew, int x, int y)
{
  int i;
  for (i=0; i < NB_MINIBT; i++) {
    if (ew->mini[i].visible) {
      if (IS_INSIDE(x,y,ew->mini[i].x, ew->mini[i].y, 
		    ew->mini[i].x + ew->mini[i].w - 1, ew->mini[i].y + ew->mini[i].h - 1)) {
	return i;
      }
    }
  }
  return -1;
}

static int
editw_button_pressed(EditW *ew) {
  int i;
  for (i=0; i < NB_MINIBT; i++) {
    if (ew->mini[i].visible && ew->mini[i].enfonce) {
      return i;
    }
  }
  return -1;
}

#define IS_BLANK(i) (((unsigned char)i)<=' ')

/*
  defini les couleurs a afficher pour chaque caractere de ew->buff
   -> detecte les mots trops longs, les urls et les balises html
*/
static void
editw_colorize(EditW *ew, unsigned char *ctab)
{
  int word_start, word_end,word_len, i, j;
  ErrList spelled_faults;

  int old_antibloub_cnt = ew->antibloub_cnt;
  ew->antibloub_cnt = 0; 

  if( ew->buff_num!=0 ) { /* ainsi on evite de se taper 2 fois la boucle 
			     ( comme pour les versions precedentes ) */
    for( i=0; ew->buff[i]; i++)
      ctab[i] = EWC_NORMAL;
  } else {
    if( Prefs.ew_do_spell )
      spelled_faults = spellString(ew->buff);
    else
      spelled_faults = NULL;
    i = 0;
    do {
      /* isolation du premier/prochain mot de buf */
      for(; ew->buff[i] && IS_BLANK(ew->buff[i]); ++i)
	ctab[i] = EWC_NORMAL;
      if( ew->buff[i]==0 )
	break;
      word_start = i;
      for(; ew->buff[i] && !IS_BLANK(ew->buff[i]); ++i);
      word_end = i;
      word_len = word_end-word_start+1; assert(word_len>0);
      /* detection des URLs */
      if( is_url(ew->buff+word_start) != -1) {
        unsigned char *url = str_ndup(ew->buff+word_start,
                                      word_end - word_start);
        unsigned char c = EWC_URL;
        if ((ew->antibloub_cnt = logged_urls_find_url(url, &ew->antibloub_id))) 
          c = EWC_KNOWN_URL;
	for( j=word_start; j<word_end; ++j) 
	  ctab[j] = c;
        
	/* detection des mots trops longs */
      } else if( word_len>31 ) {
	for( j=word_start; j<word_end; ++j)
	  ctab[j] = EWC_LONGWORD;
      } else {
	j = word_start;
	while( j<word_end ) {
	  if( !isalpha(ew->buff[j]) ) {
	    ctab[j] = EWC_NORMAL;
	    if( ew->buff[j]!='<' ) 
	      ;
	    /* detection des balises connues */
	    else if ( strncmp(ew->buff+j, "<i>", 3)==0 ||
		      strncmp(ew->buff+j, "</i>", 4)==0 ||
		      strncmp(ew->buff+j, "<b>", 3)==0 ||
		      strncmp(ew->buff+j, "</b>", 4)==0 ||
		      strncmp(ew->buff+j, "<u>", 3)==0 ||
		      strncmp(ew->buff+j, "</u>", 4)==0 ||
		      strncmp(ew->buff+j, "<s>", 3)==0 ||
		      strncmp(ew->buff+j, "</s>", 4)==0 ) {
	      for(; ew->buff[j]!='>'; ++j)
		ctab[j] = EWC_BALISE;
	      ctab[j] = EWC_BALISE;
	    }
	    ++j;
	    /* Detection des mots pleins de fautes */
	  } else if( spelled_faults!=NULL && (unsigned)j==spelled_faults->offset ) {
	    for(; isalpha(ew->buff[j]) || ew->buff[j]=='\''; ++j)
	      ctab[j] = EWC_SPELLWORD;
	    spelled_faults = spelled_faults->next;
	  } else {
	    /* Cas ou on a un caratere alpha qui est dans un mot san fautes (?) */
	    ctab[j] = EWC_NORMAL;
	    ++j;
	  }
	}
      }
    } while(ew->buff[i]);
  }
  if (ew->antibloub_cnt != old_antibloub_cnt) {
    ew->antibloub_need_clipouille = 0;
  }
}

static void
editw_draw_frame(Dock *dock, EditW *ew, Drawable d, GC *gc, int mono)
{  
#define EWDF_COLOR(p) { if (!mono) {XSetForeground(dock->display, *gc, (p)); } \
                        else XSetForeground(dock->display, *gc, 1); } 

#define GROUIK_NPTS 19
  static XPoint grouik[GROUIK_NPTS] = 
    {{216                        , EW_SHAPED_HEIGHT-1},
     {216                        , EW_HEIGHT-3},
     {216+7                      , EW_HEIGHT-3-7},
     {EW_WIDTH - 8               , EW_HEIGHT-3-7},
     {EW_WIDTH - 4               , EW_HEIGHT-3-7-4},
     {EW_WIDTH - 4               , 15},
     {EW_WIDTH - 4+4             , 11},
     {EW_WIDTH + 4               , 11},
     {EW_WIDTH + 4               , 28},
     {EW_SHAPED_WIDTH-3          , 28},

     {EW_SHAPED_WIDTH-3          , 2},
     {EW_SHAPED_WIDTH-1          , 2},
     {EW_SHAPED_WIDTH-1          , 61},
     {EW_SHAPED_WIDTH-3          , 61},


     {EW_SHAPED_WIDTH-3          , 36},
     {EW_WIDTH + 4               , 36},
     {EW_WIDTH + 4               , EW_SHAPED_HEIGHT-1-7},
     {EW_WIDTH + 4 - 7           , EW_SHAPED_HEIGHT-1},
     {216                        , EW_SHAPED_HEIGHT-1}};
  
  XPoint grouik2[GROUIK_NPTS];

  // 1 -> couleur claire, -1 ->sombre, 0 ->normal
  static char grouik_seg_side_left[GROUIK_NPTS-1] = 
    {-1,0,1,0,-1,0,1,1,1,-1,0,0,0,-1, -1,1,0,-1};
  static char grouik_seg_side_right[GROUIK_NPTS-1] = 
    {1,1,1,1,1,1,1,-1,1, 1,0,0,0,1, -1,-1,-1,-1};


  int i;


  /* cadres */
  EWDF_COLOR(cccolor_pixel(ew->win_bgcolor));
  XFillRectangle(dock->display, d, *gc, 
		 EW_XBORD, 0, EW_XBORD+EW_WIDTH, EW_HEIGHT);

  /* partie claire */
  EWDF_COLOR(cccolor_pixel(ew->light_color));
  XDrawLine(dock->display, d, *gc, EW_XBORD, 0, EW_XBORD+EW_WIDTH-1, 0);
  XDrawLine(dock->display, d, *gc, EW_XBORD, 0, EW_XBORD, EW_HEIGHT-1);

  XDrawLine(dock->display, d, *gc, 
  	    EW_TXT_X0, EW_TXT_Y0+EW_TXT_HEIGHT, EW_TXT_X0 + EW_TXT_WIDTH, EW_TXT_Y0+EW_TXT_HEIGHT);
  XDrawLine(dock->display, d, *gc, 
  	    EW_TXT_X0 + EW_TXT_WIDTH, EW_TXT_Y0, EW_TXT_X0 + EW_TXT_WIDTH, EW_TXT_Y0+EW_TXT_HEIGHT);

  XDrawLine(dock->display, d, *gc, EW_XBORD, 0, EW_XBORD, EW_HEIGHT);

  /* partie sombre */
  EWDF_COLOR(cccolor_pixel(ew->dark_color));
  XDrawLine(dock->display, d, *gc, EW_XBORD, EW_HEIGHT-1, EW_XBORD+EW_WIDTH-1, EW_HEIGHT-1);
  XDrawLine(dock->display, d, *gc, EW_XBORD+EW_WIDTH-1, 0, EW_XBORD+EW_WIDTH-1, EW_HEIGHT-1);
  XDrawLine(dock->display, d, *gc, 
	    EW_TXT_X0-1, EW_TXT_Y0-1, EW_TXT_X0 + EW_TXT_WIDTH, EW_TXT_Y0-1);
  XDrawLine(dock->display, d, *gc, 
	    EW_TXT_X0-1, EW_TXT_Y0-1, EW_TXT_X0-1, EW_TXT_Y0+EW_TXT_HEIGHT);

  /* partie coloree normale */
  EWDF_COLOR(dock->bg_pixel);

  for (i=0; i < GROUIK_NPTS; i++) {
    if (ew->dock_side == RIGHT) {
      grouik2[i] = grouik[i];
    } else {
      grouik2[i].x = EW_SHAPED_WIDTH-1 - grouik[i].x;
      grouik2[i].y = grouik[i].y;
    }
  }
    
  XFillPolygon(dock->display, d, *gc, grouik2, GROUIK_NPTS, Complex, CoordModeOrigin);

  for (i=0; i < GROUIK_NPTS-1; i++) {
    int x0, y0, x1, y1;
    int v;
    x0 = grouik2[i].x; y0 = grouik2[i].y;
    x1 = grouik2[i+1].x; y1 = grouik2[i+1].y;

    v = (ew->dock_side == LEFT ? grouik_seg_side_left[i] : grouik_seg_side_right[i]);
    if (v == 1) { EWDF_COLOR(dock->light_pixel); }
    else if (v == -1) { EWDF_COLOR(dock->dark_pixel); }
    else { EWDF_COLOR(dock->bg_pixel); }
    XDrawLine(dock->display, d, *gc, x0, y0, x1, y1);
  }
}


/* s'assurer que le wordwrap a ete fait avec d'appeler cette fonction */
static void
editw_draw(Dock *dock, EditW *ew, Drawable d)
{
  int i;
  unsigned char *ctab;
  int charcnt;

  if (strlen(ew->buff)) {
    ctab = malloc(strlen(ew->buff));
  } else ctab = NULL;

  editw_colorize(ew, ctab);
  
  /* le texte */
  //XSetFont(dock->display, dock->NormalGC, ew->fn->fid);
  if (!Prefs.pp_use_colored_tabs) {
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(ew->txt_bgcolor));
    XFillRectangle(dock->display, d, dock->NormalGC, 
                   EW_TXT_X0, EW_TXT_Y0, EW_TXT_WIDTH, EW_TXT_HEIGHT);
  } else {
    XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(ew->prefs->pp_bgcolor));
    XFillRectangle(dock->display, d, dock->NormalGC, 
                   EW_TXT_X0, EW_TXT_Y0, EW_TXT_WIDTH, EW_TXT_HEIGHT);
    cccolor_reset(&ew->txt_fgcolor[EWC_NORMAL], ew->prefs->pp_fgcolor.opaque);
  }



  //  XSetForeground(dock->display, dock->NormalGC, ew->txt_fgpixel[EWC_NORMAL]);
  
  charcnt = ew->row_firstchar[ew->y_scroll];
  for (i = ew->y_scroll; i < EW_NROW+ew->y_scroll; i++) {
    int j, cnt;
    for (j=ew->row_firstchar[i], cnt=0; j < ew->row_firstchar[i+1]; j++, cnt++) {
      /*if (ctab[j] != 0 || Prefs.pp_use_colored_tabs==0)
	XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(ew->txt_fgcolor[ctab[j]]));
        else XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(ew->prefs->pp_fgcolor.opaque));*/
      ccfont_draw_string8(ew->fn, ew->txt_fgcolor[ctab[j]], d, EW_TXT_X0+cnt*FN_W, 
                          EW_TXT_Y0 + (i-ew->y_scroll)*FN_H + FN_BASE_H, ew->buff+j, 1);
      /*XDrawString(dock->display, d, dock->NormalGC, 
        EW_TXT_X0+cnt*FN_W, EW_TXT_Y0 + (i-ew->y_scroll)*FN_H + FN_BASE_H, ew->buff+j, 1);*/
    }

    if (i >= ew->nrow_used-1) charcnt += EW_NCOL; else charcnt += cnt;
    

    //    llast = ew->nrow_used + (ew->buff_sz - ew->row_firstchar[ew->nrow_used])/EW_NCOL
    if ((i < ew->nrow_used - 1 && cnt < EW_NCOL)
	|| charcnt > ew->buff_sz) {
      if (charcnt > ew->buff_sz) cnt = MAX(EW_NCOL - (charcnt - ew->buff_sz+1), 0);
      XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(ew->fill_bgcolor));
      XFillRectangle(dock->display, d, dock->NormalGC, 
		     EW_TXT_X0+FN_W*cnt, EW_TXT_Y0 + (i-ew->y_scroll)*FN_H, 
		     EW_TXT_WIDTH-(FN_W*cnt), FN_H);
    }
  }


  /* pas de selection active -> on affiche le curseur */
  if (!editw_has_selection(ew)) {
    int curs_idx;

    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(ew->cur_bgcolor));
    XFillRectangle(dock->display, d, dock->NormalGC, 
		   EW_TXT_X0 + FN_W * ew->curs_x, 
		   EW_TXT_Y0 + FN_H * (ew->curs_y-ew->y_scroll), FN_W, FN_H);
    curs_idx = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
    //XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(ew->cur_fgcolor));
    
    if (curs_idx < ew->row_firstchar[ew->curs_y+1]) {
      ccfont_draw_string8(ew->fn, ew->cur_fgcolor, d, EW_TXT_X0 + FN_W * ew->curs_x, 
                          EW_TXT_Y0 + FN_H * (ew->curs_y-ew->y_scroll) + FN_BASE_H, 
                          &ew->buff[curs_idx], 1);
      /*XDrawString(dock->display, d, dock->NormalGC, 
		   EW_TXT_X0 + FN_W * ew->curs_x, 
		  EW_TXT_Y0 + FN_H * (ew->curs_y-ew->y_scroll) + FN_BASE_H, 
		  &ew->buff[curs_idx], 1);*/
    }
  } else {
    /* affichage de la selection */
    int sx0, sy0, sx1, sy1, y, x0, x1;
    int sel_start, sel_end;


    editw_get_sel_bounds(ew, &sel_start, &sel_end);
    assert(sel_start < sel_end);
    editw_strpos2xy(ew, sel_start, &sx0, &sy0);
    editw_strpos2xy(ew, sel_end-1, &sx1, &sy1);

    /* printf("sel: (%d,%d) - (%d,%d), ew->y_scroll=%d\n", sx0,sy0,sx1,sy1, ew->y_scroll); */

    if (sy0 < ew->y_scroll) { sy0 = ew->y_scroll; sx0 = 0; }
    if (sy1 > ew->y_scroll + EW_NROW - 1) { sy1 = ew->y_scroll + EW_NROW - 1; sx1 = EW_NCOL; }
    for (y = sy0; y <= sy1; y++) {
      if (y == sy0) x0 = sx0; else x0 = 0;
      if (y == sy1) {
	x1 = sx1;
	x1 = MIN(x1, ew->row_firstchar[y+1]-ew->row_firstchar[y] - 1);
      } else x1 = ew->row_firstchar[y+1]-ew->row_firstchar[y] - 1;

      /* printf("y=%d, x0=%d, x1=%d, row first=%d row last=%d\n", y, x0, x1, ew->row_firstchar[y], ew->row_firstchar[y+1]); */
      XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(ew->sel_bgcolor));
      XFillRectangle(dock->display, d, dock->NormalGC, 
		     EW_TXT_X0 + FN_W * x0, EW_TXT_Y0 + FN_H * (y-ew->y_scroll), FN_W*(x1+1-x0), FN_H);

      ccfont_draw_string8(ew->fn, ew->sel_fgcolor, d, EW_TXT_X0 + FN_W * x0, 
                          EW_TXT_Y0 + FN_H * (y-ew->y_scroll) + FN_BASE_H, 
                          &ew->buff[ew->row_firstchar[y]+x0], x1 - x0 + 1);
      /*XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(ew->sel_fgcolor));
      XDrawString(dock->display, d, dock->NormalGC, 
		   EW_TXT_X0 + FN_W * x0, EW_TXT_Y0 + FN_H * (y-ew->y_scroll) + FN_BASE_H, 
		  &ew->buff[ew->row_firstchar[y]+x0], x1 - x0 + 1);
      */
    }
  }


  /* titre */
  {
    int x_title, x_name;
    char *title;
    char *site_name;
    assert(ew->prefs);
    title = str_printf("palmipede editor: %s",
		       ew->buff_num == 0 ? "MSG" : "UA");
    site_name = str_printf("%.8s", ew->prefs->site_name);
    
    x_title = (ew->dock_side == RIGHT ? EW_TXT_X0 + 8 : 216);
    x_name = x_title + strlen(title)*FN_W + 8;

    
    ccfont_draw_string8(ew->fn, ew->light_color, d, 
                        x_title - 1, EW_HEIGHT - 3 -1, 
                        title, strlen(title));
    /*ccfont_draw_string8(ew->fn, ew->light_color, d, 
                        x_name - 1, EW_HEIGHT - 3 -1, 
                        site_name, strlen(site_name));*/
    ccfont_draw_string8(ew->fn, ew->dark_color, d, 
                        x_title, EW_HEIGHT - 3, 
                        title, strlen(title));
    ccfont_draw_string8(ew->fn, dock->blue_color, d,
                        x_name, EW_HEIGHT - 3, 
                        site_name, strlen(site_name));
    /*
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(ew->light_color));
    XDrawString(dock->display, d, dock->NormalGC, 
		x_title - 1, EW_HEIGHT - 3 -1, 
		title, strlen(title));
    XDrawString(dock->display, d, dock->NormalGC, 
		x_name - 1, EW_HEIGHT - 3 -1, 
		site_name, strlen(site_name));
    XSetForeground(dock->display, dock->NormalGC, cccolor_pixel(ew->dark_color));
    XDrawString(dock->display, d, dock->NormalGC, 
		x_title, EW_HEIGHT - 3, 
		title, strlen(title));
    XSetForeground(dock->display, dock->NormalGC, IRGB2PIXEL(0x0000ff));
    XDrawString(dock->display, d, dock->NormalGC, 
		x_name, EW_HEIGHT - 3, 
		site_name, strlen(site_name));
    */
    free(title);
    free(site_name);
  }
  /* miniboutons */
  for (i=0; i < NB_MINIBT; i++) {
    int x,y,w,h;
    x =  ew->mini[i].x; y = ew->mini[i].y;
    w =  ew->mini[i].w; h = ew->mini[i].h;
    if (ew->mini[i].visible) {
      int dx;
      dx = (i == BT_TOTOZBOOKMARK) ? 1:0;
      XCopyArea(dock->display, ew->minipix, d, dock->NormalGC, ew->mini[i].pix_x, ew->mini[i].pix_y,
		w,h,x+ew->mini[i].enfonce+dx,y+ew->mini[i].enfonce+dx);
      
      if (ew->mini[i].enfonce) {
	XSetForeground(dock->display, dock->NormalGC, dock->dark_pixel);
	
	XDrawLine(dock->display, d, dock->NormalGC, x, y, x+w, y);
	XDrawLine(dock->display, d, dock->NormalGC, x, y, x, y+h);
      } else {
	XSetForeground(dock->display, dock->NormalGC, dock->dark_pixel);
	
	XDrawLine(dock->display, d, dock->NormalGC, x+w, y+1, x+w, y+h);
	XDrawLine(dock->display, d, dock->NormalGC, x+1, y+h, x+w, y+h);
      }
    }
  }
  if (ctab) free(ctab);
}

void
editw_show_pix(Dock *dock, EditW *ew)
{
  XCopyArea(dock->display, ew->pix, ew->win, dock->NormalGC, 0, 0, EW_SHAPED_WIDTH, EW_SHAPED_HEIGHT, 0, 0);
}

void
editw_refresh(Dock *dock, EditW *ew)
{
  editw_draw(dock, ew, ew->pix);
  editw_show_pix(dock, ew);
}

/* un seul niveau d'undo... */
void
editw_undo(Dock *dock, EditW *ew) {
  if (ew->undo.buff) {
    int i;
    for (i=0; i < ew->buff_sz; i++) {
      SWAP(ew->buff[i], ew->undo.buff[i], char);
    }
    SWAP(ew->y_scroll, ew->undo.y_scroll, int);
    SWAP(ew->curs_x, ew->undo.curs_x, int);
    SWAP(ew->curs_y, ew->undo.curs_y, int);


    editw_unselect(ew);
    editw_wordwrap(ew);
    editw_refresh(dock, ew);
  }
}

int
editw_get_site_id(Dock *dock) {
  int i;
  for (i=0; i < MAX_SITES; i++) {
    if (dock->editw->prefs == Prefs.site[i]) return i;
  }
  return -1;
}

/* changement de buffer edite: message <-> useragent */
static void
editw_select_buff(Dock *dock, EditW *ew, int user_agent_mode)
{
  int j,x,sgn;

  Site *s = sl_find_site_by_name(dock->sites, ew->prefs->site_name);
  assert(s);
  assert(s->board);
  if (user_agent_mode == 1) {
    ew->buff_num = 1;
    ew->buff_sz = s->prefs->palmi_ua_max_len+1; //USERAGENT_MAX_LEN+1;
    ew->buff = s->board->coin_coin_useragent;
  } else if (user_agent_mode == 0) {
    ew->buff_num = 0;
    ew->buff_sz = s->prefs->palmi_msg_max_len+1; //MESSAGE_MAX_LEN+1;
    ew->buff = dock->coin_coin_message;
  } else {
    assert(0);
  }

  ew->buff[ew->buff_sz-1] = 0; // securite

  ew->curs_x = 0; ew->curs_y = 0; ew->y_scroll = 0;
  ew->sel_anchor = 0;
  ew->sel_head = strlen(ew->buff);

  editw_wordwrap(ew);

  if (ew->dock_side == RIGHT) {
    x = EW_WIDTH-8;
    sgn = -1;
  } else {
    x = EW_XBORD+8-15;
    sgn = +1;
  }
  
  for (j=0; j < NB_MINIBT; j++) {
    static const int miniorder[NB_MINIBT] = {
      BT_CLOSE, BT_CHANGE, BT_TOTOZBOOKMARK, BT_UNDO, BT_CLEAR, 
      BT_STRIKE, BT_UNDERLINE, BT_ITAL, BT_BOLD, BT_TT, BT_DEFAULTUA};
    int i;
    
    i = miniorder[j];

    ew->mini[i].visible = 1;
    if ((user_agent_mode && i != BT_CLOSE && i != BT_CHANGE && i != BT_UNDO && i != BT_CLEAR && i != BT_DEFAULTUA) ||
	(user_agent_mode==0 && i == BT_DEFAULTUA)) {
      ew->mini[i].visible = 0;
    }
    ew->mini[i].enfonce = 0;

    if (ew->mini[i].visible)
      x += sgn*(ew->mini[i].w+1);
    ew->mini[i].x = x;
    ew->mini[i].y = EW_SHAPED_HEIGHT - 15;
    if (j == 2) x += sgn*10;
    if (j == 4) x += sgn*10;
  }

  if (ew->undo.buff) free(ew->undo.buff);
  ew->undo.buff = malloc(ew->buff_sz); 
  memset(ew->undo.buff, 0, ew->buff_sz);

  ew->last_command = OTHER;
  editw_save_state(ew, CUTPASTE);

  //editw_refresh(dock, ew);
  editw_draw_frame(dock, ew, ew->pix, &dock->NormalGC, 0);
  editw_draw(dock, ew, ew->pix);
  //  XMoveWindow(dock->display, ew->win, 1200, 70) ;
  editw_action(dock, ew);
}

/* appelée par editw_show */
static void
editw_select_site(Dock *dock, int site_id) {
  Site *s;
  s = sl_find_site_id(dock->sites, site_id); assert(s);
  assert(s->prefs->check_board);
  dock->coin_coin_site_id = site_id;
  dock->editw->prefs = s->prefs;
}

static void
editw_select_default_site(Dock *dock) {
  Site *s;
  dock->editw->prefs = NULL;
  for (s = dock->sites->list; s; s = s->next) {
    if (s->prefs->check_board) {
      dock->editw->prefs = s->prefs; break;
    }
  }
}

/* declenche l'affichage de la fenetre */
void
editw_show(Dock *dock, SitePrefs *sp, int user_agent_mode)
{
  EditW *ew = dock->editw;
  XSetWindowAttributes wa;
  Pixmap shape_mask;
  GC monoGC;
  int xiscr;
  if (sp) ew->prefs = sp;
  if (ew->prefs == NULL) {
    editw_select_default_site(dock);
  }
  if (ew->prefs == NULL) { /* raaaah y'a aucune tribune */
    msgbox_show(dock, _("looks like you fucked your options file, no board "
			"has be defined: no noard, no palmipede"));
    return;
  } else {
    Site *s = sl_find_site_by_name(dock->sites, ew->prefs->site_name);
    if (s->prefs->check_board) {
      editw_select_site(dock, s->site_id);
    } else editw_select_default_site(dock);
  }

  assert(ew->mapped == 0);
  ew->mapped = 1;

  xiscr = MAX(dock_get_icon_pos(dock, &ew->dock_x, &ew->dock_y),0);
  if (ew->dock_x - dock->xiscreen[xiscr].x_org < EW_SHAPED_WIDTH) ew->dock_side = LEFT; else ew->dock_side = RIGHT;
  
  ew->win = XCreateSimpleWindow (
        dock->display, dock->rootwin, 0, 0, EW_SHAPED_WIDTH, EW_SHAPED_HEIGHT, 0,
        WhitePixel(dock->display, dock->screennum),
        cccolor_pixel(ew->win_bgcolor));
  
  //wa.background_pixmap = ButtonBarImage ;
  wa.event_mask =
    ButtonPressMask | 
    ButtonReleaseMask | 
    ExposureMask | 
    PointerMotionMask | 
    StructureNotifyMask |
    KeyPressMask | 
    KeyReleaseMask | 
    EnterWindowMask | 
    LeaveWindowMask | 
    FocusChangeMask; // raaaaah necessaire pour que les Input Method fonctionnent!
  
  //  if (! ClickOnly) wa.event_mask |= EnterWindowMask | LeaveWindowMask ;
  wa.override_redirect = (Prefs.palmipede_override_redirect ? True : False) ;
  XChangeWindowAttributes (dock->display, ew->win,
			   //CWBackPixmap | 
			   CWEventMask | CWOverrideRedirect, &wa);
  XSetTransientForHint(dock->display, ew->win, DOCK_WIN(dock));
  XSetWindowBorderWidth(dock->display, ew->win, 0);
  /* passage en borderless à grands coups de massue sur la tête du wmanager */
  XSetWMProtocols(dock->display, ew->win, &dock->atom_WM_TAKE_FOCUS, 1);
  set_borderless_window_hints(dock->display, ew->win);
  /*  set_window_sizepos_hints(dock->display, ew->win, -10000, -10000, 
                           EW_SHAPED_WIDTH,EW_SHAPED_WIDTH,EW_SHAPED_WIDTH,
                           EW_SHAPED_HEIGHT,EW_SHAPED_HEIGHT,EW_SHAPED_HEIGHT);
  */
  set_window_class_hint(dock->display, ew->win, "wmcoincoin", "palmipede");
  //#ifdef OLD_KBCOINCOIN
  kb_create_input_context_for(dock, ew->win, KB_PALMIPEDE);
  //#endif
  /* cree le pixmap et le rempli de noir */
  ew->pix = XCreatePixmap(dock->display, ew->win, EW_SHAPED_WIDTH, EW_SHAPED_HEIGHT, 
			  DefaultDepth(dock->display,dock->screennum));
  XSetForeground(dock->display, dock->NormalGC, BlackPixel(dock->display,dock->screennum));
  XFillRectangle(dock->display, ew->pix, dock->NormalGC, 0, 0, EW_SHAPED_WIDTH, EW_SHAPED_HEIGHT);

  /* cree le mask, avec le GC-qui-va-bien et le met a 0 */
  shape_mask = XCreatePixmap(dock->display, dock->rootwin, EW_SHAPED_WIDTH, EW_SHAPED_HEIGHT, 1);
  monoGC = XCreateGC(dock->display, shape_mask, 0, NULL);
  XSetForeground(dock->display, monoGC, 0); 
  XFillRectangle(dock->display, shape_mask, monoGC, 0, 0, EW_SHAPED_WIDTH, EW_SHAPED_HEIGHT);

  editw_draw_frame(dock, ew, ew->pix, &dock->NormalGC, 0);
  editw_draw_frame(dock, ew, shape_mask, &monoGC, 1);
  XShapeCombineMask(dock->display, ew->win, ShapeBounding, 0, 0, shape_mask,
		    ShapeSet);
  XFreePixmap(dock->display, shape_mask);
  XFreeGC(dock->display, monoGC);

  /*
  {
    XRectangle xr[2];
    
    xr[0].x = 0; xr[0].y = 0; xr[0].width = EW_WIDTH; xr[0].height = EW_HEIGHT-10;
    xr[1].x = 100; xr[1].y = EW_HEIGHT-9; xr[1].width = EW_WIDTH-100; xr[1].height = 10;
    XShapeCombineRectangles(dock->display, ew->win, ShapeBounding, 0, 0, xr, 2, ShapeSet, Unsorted);
    }*/

  ew->action = SORTIR; ew->action_step = 0;

  /* user_agent_mode = -1 -> on ne change pas le buff_num,
     sinon on lui assigne user_agent_mode */
  editw_select_buff(dock, ew, (user_agent_mode == -1 ? ew->buff_num : user_agent_mode));

}


void
editw_unmap(Dock *dock, EditW *ew)
{
  ew->mapped = 0;
  //#ifdef OLD_KBCOINCOIN
  kb_release_input_context(KB_PALMIPEDE);
  //#endif
  //  fprintf(stderr, "destroy! %lx\n", (unsigned long)ew->win);
  XDestroyWindow(dock->display, ew->win);
  //  fprintf(stderr, "destroy2!\n");

  XFreePixmap(dock->display, ew->pix);
  ew->win = None;
  ew->action = NOACTION; /* au cas ou on fait un unmap en pleine sortie */
  ew->buff = NULL; ew->buff_sz = 0;
  if (ew->undo.buff) { free(ew->undo.buff); ew->undo.buff = NULL; }
}

int editw_ismapped(EditW *ew) {
  return ew->mapped;
}

#define ACTION_NB_STEPS 15

double exp(double x);
/* animation de la fenetre quand elle rentre et sort .. */
void
editw_action(Dock *dock, EditW *ew)
{
  //  printf("%d %d\n", ew->action, ew->action_step);
  if (ew->action == SORTIR) {
    float f;
    int l;
    static int save_prev_l = 0;
    Pixmap tpix = Prefs.palmipede_override_redirect ? 
      None : XCreatePixmap(dock->display, ew->win, EW_SHAPED_WIDTH, EW_SHAPED_HEIGHT, 
                           DefaultDepth(dock->display,dock->screennum));
    f = (ew->action_step+1) / ((float)ACTION_NB_STEPS);
    l = (int)(EW_SHAPED_WIDTH * f * f + .5);
    //    l = (EW_SHAPED_WIDTH * (ew->action_step+1)) / (ACTION_NB_STEPS);
    
    //printf("SORTIR l=%d\n",l);
    if (l == 0) {
      l = 2;
    }
    if (ew->action_step == 0) {
      /* traitement particulier quand le dock est a gauche de editw,
	 a cause des XShape...
      */
      if (ew->dock_side == LEFT) {
	XShapeOffsetShape(dock->display, ew->win, ShapeBounding, -EW_SHAPED_WIDTH+l, 0);
	save_prev_l = l;
      }
    }
    if (l > EW_SHAPED_WIDTH) l = EW_SHAPED_WIDTH;
    if (ew->dock_side == LEFT) {
      ew->win_xpos = ew->dock_x+64-2;
      ew->win_ypos = ew->dock_y;
      XShapeOffsetShape(dock->display, ew->win, ShapeBounding, l-save_prev_l, 0);
      save_prev_l = l;
      XMoveResizeWindow(dock->display, ew->win, ew->win_xpos, ew->win_ypos, l, EW_SHAPED_HEIGHT);
      if (Prefs.palmipede_override_redirect)
        XCopyArea(dock->display, ew->pix, ew->win, dock->NormalGC, EW_SHAPED_WIDTH-l, 0, l, EW_SHAPED_HEIGHT, 0, 0);
      else
        XCopyArea(dock->display, ew->pix, tpix, dock->NormalGC, EW_SHAPED_WIDTH-l, 0, l, EW_SHAPED_HEIGHT, 0, 0);
    } else {
      ew->win_xpos = ew->dock_x-l +2;
      ew->win_ypos = ew->dock_y;
      XMoveResizeWindow(dock->display, ew->win, ew->win_xpos, ew->win_ypos, l, EW_SHAPED_HEIGHT);
      if (Prefs.palmipede_override_redirect)
        XCopyArea(dock->display, ew->pix, ew->win, dock->NormalGC, 0, 0, l, EW_SHAPED_HEIGHT, 0, 0);
      else XCopyArea(dock->display, ew->pix, tpix, dock->NormalGC, 0, 0, l, EW_SHAPED_HEIGHT, 0, 0);
    }
    
    if (!Prefs.palmipede_override_redirect) {
      XSetWindowBackgroundPixmap(dock->display, ew->win, tpix); 
      XFreePixmap(dock->display, tpix);
    }
    if (ew->action_step == 0) {
      Window root_return;
      int x_return, y_return, width_return, height_return, border_width_return, depth_return;
      set_window_sizepos_hints(dock->display, ew->win, ew->win_xpos, ew->win_ypos, l, l, l, 
                               EW_SHAPED_HEIGHT,EW_SHAPED_HEIGHT,EW_SHAPED_HEIGHT);
      XGetGeometry(dock->display, ew->win, &root_return, &x_return, &y_return, &width_return,
                   &height_return, &border_width_return, &depth_return);
      //printf("kikoo XMapRaised palmi %d,%d <-> %dx%d+%d+%d\n", ew->win_xpos, ew->win_ypos, width_return, height_return, x_return, y_return);
      XMapRaised(dock->display, ew->win);
    }
    ew->action_step ++;
    if (ew->action_step >= ACTION_NB_STEPS) {
      //      ew->action_step = 0;
      editw_set_kbfocus(dock, ew, 1);
      ew->action = NOACTION;
    }
  } else if (ew->action == RENTRER) {
    //    int l = (EW_SHAPED_WIDTH * (ew->action_step)) / (ACTION_NB_STEPS);
    float f;
    int l;
    static int save_prev_l = 0;
 
    f = (ew->action_step) / ((float)ACTION_NB_STEPS);
    l = (int)(EW_SHAPED_WIDTH * (1. - (1-f) * (1-f)) + .5);

    if (ew->action_step == ACTION_NB_STEPS) save_prev_l = l;

    if (l > 0) {
      Pixmap tpix = XCreatePixmap(dock->display, ew->win, EW_SHAPED_WIDTH, EW_SHAPED_HEIGHT, 
                                  DefaultDepth(dock->display,dock->screennum));
      if (ew->dock_side == LEFT) {
	ew->win_xpos = ew->dock_x+64-2;
	ew->win_ypos = ew->dock_y;
	XShapeOffsetShape(dock->display, ew->win, ShapeBounding, l-save_prev_l, 0);
	save_prev_l = l;
	XMoveResizeWindow(dock->display, ew->win, ew->win_xpos, ew->win_ypos, l, EW_SHAPED_HEIGHT);
	////XCopyArea(dock->display, ew->pix, ew->win, dock->NormalGC, EW_SHAPED_WIDTH-l, 0, l, EW_SHAPED_HEIGHT, 0, 0);
        XCopyArea(dock->display, ew->pix, tpix, dock->NormalGC, EW_SHAPED_WIDTH-l, 0, l, EW_SHAPED_HEIGHT, 0, 0);
      } else {
	ew->win_xpos = ew->dock_x-l +2;
	ew->win_ypos = ew->dock_y;
	XMoveResizeWindow(dock->display, ew->win,  ew->win_xpos, ew->win_ypos, l, EW_SHAPED_HEIGHT);
	////XCopyArea(dock->display, ew->pix, ew->win, dock->NormalGC, 0, 0, l, EW_SHAPED_HEIGHT, 0, 0);
        XCopyArea(dock->display, ew->pix, tpix, dock->NormalGC, 0, 0, l, EW_SHAPED_HEIGHT, 0, 0);
      }
      XSetWindowBackgroundPixmap(dock->display, ew->win, tpix); 
      XFreePixmap(dock->display, tpix);
    }

    ew->action_step --;

    if (ew->action_step == 0) {
      ew->action = NOACTION;
      editw_unmap(dock, ew);
    }
  }
}

void
editw_hide(Dock *dock, EditW *ew) {
  if (dock)
    ew->action = RENTRER;
  //  ew->action_step = 0;
}

void editw_init_colors(EditW *ew) {
  ew->win_bgcolor = ew->dark_color = ew->light_color = 
    ew->fill_bgcolor = ew->txt_fgcolor[EWC_NORMAL] = 
    ew->txt_bgcolor = ew->txt_fgcolor[EWC_LONGWORD] = 
    ew->txt_fgcolor[EWC_BALISE] = ew->txt_fgcolor[EWC_URL] = 
    ew->txt_fgcolor[EWC_KNOWN_URL] = ew->txt_fgcolor[EWC_SPELLWORD] = 
    ew->cur_bgcolor = 
    ew->cur_fgcolor = ew->sel_bgcolor = ew->sel_fgcolor = (CCColorId)(-1);
}

void editw_release_colors(EditW *ew) {
   cccolor_release(&ew->win_bgcolor);
   cccolor_release(&ew->dark_color);
   cccolor_release(&ew->light_color);
   cccolor_release(&ew->fill_bgcolor);
   cccolor_release(&ew->txt_fgcolor[EWC_NORMAL]);
   cccolor_release(&ew->txt_bgcolor);
   cccolor_release(&ew->txt_fgcolor[EWC_LONGWORD]);
   cccolor_release(&ew->txt_fgcolor[EWC_BALISE]);
   cccolor_release(&ew->txt_fgcolor[EWC_URL]);
   cccolor_release(&ew->txt_fgcolor[EWC_KNOWN_URL]);
   cccolor_release(&ew->txt_fgcolor[EWC_SPELLWORD]);
   cccolor_release(&ew->cur_bgcolor);
   cccolor_release(&ew->cur_fgcolor);
   cccolor_release(&ew->sel_bgcolor);
   cccolor_release(&ew->sel_fgcolor); 
 }

void
editw_reload_colors(Dock *dock, EditW *ew)
{
  char s_xpm_bgcolor[30];

  cccolor_reset(&ew->win_bgcolor, 0xC0C0C0); // = RGB2PIXEL(200,200,200);
  cccolor_reset(&ew->dark_color, 0x808080); // = RGB2PIXEL(128, 128, 128);
  cccolor_reset(&ew->light_color, 0xD8D8D8); // = RGB2PIXEL(230, 230, 230);
  cccolor_reset(&ew->fill_bgcolor, 0xD8D8D8); // = RGB2PIXEL(230, 230, 230);
  /* couleurs du texte */
  cccolor_reset(&ew->txt_fgcolor[EWC_NORMAL], 0x00000);
  cccolor_reset(&ew->txt_bgcolor, 0xFFFFFF); // = RGB2PIXEL(255,255,255);
  cccolor_reset(&ew->txt_fgcolor[EWC_LONGWORD], 0xff0000);// = RGB2PIXEL(255,0,0);
  cccolor_reset(&ew->txt_fgcolor[EWC_BALISE],0x008000); // = RGB2PIXEL(0, 127, 0);
  cccolor_reset(&ew->txt_fgcolor[EWC_URL], 0x0000FF); // = RGB2PIXEL(0, 0, 255);
  cccolor_reset(&ew->txt_fgcolor[EWC_KNOWN_URL], 0xFF00FF); // = RGB2PIXEL(0, 0, 255);
  cccolor_reset(&ew->txt_fgcolor[EWC_SPELLWORD], 0xC03262);// = RGB2PIXEL(200, 50, 100);
  cccolor_reset(&ew->cur_bgcolor, 0xff0000);// = RGB2PIXEL(255,0,0);
  cccolor_reset(&ew->cur_fgcolor, 0xffffff);
  cccolor_reset(&ew->sel_bgcolor, 0xffe000);// = RGB2PIXEL(255,215,0);
  cccolor_reset(&ew->sel_fgcolor, 0); // = RGB2PIXEL(0,0,0);
  
  /* on remplace la ligne de la couleur transparente par notre couleur de fond,
     c une ruse de sioux */
  if (ew->minipix) { XFreePixmap(dock->display, ew->minipix); ew->minipix = None; }

/* un jour je m'insulterai en me disant que j'ai du mettre un commentaire
   instructif et pertinent par rapport à la presence des deux espaces en debut
   de ligne. */
  snprintf(s_xpm_bgcolor, 30, "  \tc #%06X", Prefs.dock_bgcolor); 
  editwin_minib_xpm[1] = s_xpm_bgcolor;
  ew->minipix = RGBACreatePixmapFromXpmData(dock->rgba_context, editwin_minib_xpm); assert(ew->minipix);
    
  if (ew->clippy_pixmap) { XFreePixmap(dock->display, ew->clippy_pixmap); ew->clippy_pixmap = None; }
  snprintf(s_xpm_bgcolor, 30, " \tc #%06X", (255 << 16) + (231 << 8) + 186);
  clippy_xpm[1] = s_xpm_bgcolor;
  ew->clippy_pixmap = RGBACreatePixmapFromXpmData(dock->rgba_context, clippy_xpm); assert(ew->clippy_pixmap);

  sscanf(clippy_xpm[0], "%d %d", &ew->clippy_w, &ew->clippy_h);
}


/* initialisation */
void
editw_build(Dock *dock)
{
  EditW *ew;

  ALLOC_OBJ(ew, EditW);
  ew->mapped = 0;
  ew->action = NOACTION;
  ew->buff_num = 0;
  ew->win_xpos = ew->win_ypos = 0;
  ew->undo.buff = NULL;
  ew->last_command = OTHER;
  /*{
    char fn[512];
    snprintf(fn, 512, "%s-%s", EW_FONT, Prefs.font_encoding);
    ew->fn = XLoadQueryFont(dock->display, fn);
    if (!ew->fn) {
      myfprintf(stderr, _("Unable to load font %s, trying iso8859-1 encoding \n"), fn);
      snprintf(fn, 512, "%s-%s", EW_FONT, "iso8859-1");
      ew->fn = XLoadQueryFont(dock->display, fn);
      if (!ew->fn) {
	myfprintf(stderr, _("Failed\n"));
	exit(-1);
      }
    }
    }*/
  ew->fn = ccfont_get(EW_FONT); //"monospace-7");
 

  ew->sel_anchor = ew->sel_head = -1;
  ew->y_scroll = 0;
  ew->curs_x = 0;
  ew->curs_y = 0;

  head = &ew->sel_head;
  anch = &ew->sel_anchor;

  ew->prefs = NULL;

  ew->minipix = None;
  ew->clippy_pixmap = None;
  editw_init_colors(ew);
  editw_reload_colors(dock, ew);

  {
    static int bt_x[NB_MINIBT] = { 0, 12, 27, 41, 55, 69, 83, 97, 111, 125, 139};
    static int bt_w[NB_MINIBT] = {11, 15, 14, 14, 14, 14, 14, 14, 14, 14, 14};
    int i;

    for (i=0; i < NB_MINIBT; i++) {
      ew->mini[i].pix_x = bt_x[i]; ew->mini[i].pix_y = 0;
      ew->mini[i].w = bt_w[i]; ew->mini[i].h = 12;
    }
  }
  ew->antibloub_id = id_type_invalid_id();
  ew->antibloub_cnt = 0;
  ew->antibloub_need_clipouille = 0;
  dock->editw = ew;
}

void
editw_rebuild(Dock *dock)
{
  EditW *ew = dock->editw;
  int show = 0;
  if (editw_ismapped(ew)) { editw_unmap(dock, ew); show = 1; }
  editw_reload_colors(dock, ew);
  editw_select_default_site(dock);
  if (show) { editw_show(dock, ew->prefs, 0); }
}

static void
editw_move_cursor_rel(EditW *ew, int dec)
{
  int i;
  i = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
  i += dec;
  i = MIN(i, (int)strlen(ew->buff));
  i = MAX(i, 0);
  editw_strpos2curs(ew, i);
}

/*
 * (des)active le keyboard focus sur la fenetre
 */  
void 
editw_set_kbfocus(Dock *dock, EditW *ew, int get_it)
{
  Window focwin;
  int revert_to;/* , pp_focus; */
  if (Prefs.palmipede_override_redirect) {
    XGetInputFocus(dock->display, &focwin, &revert_to);
    /* pp_focus = (pp_ismapped(dock) && focwin == pp_get_win(dock)); */
    if (get_it && ew->mapped/* && !pp_focus*/)
      XSetInputFocus(dock->display, ew->win, RevertToPointerRoot, CurrentTime);
    else /*if (!get_it) {
           if (focwin == editw_get_win(ew)) {
           if (pp_ismapped(dock)) 
           XSetInputFocus(dock->display, pp_get_win(dock), RevertToPointerRoot, CurrentTime);
           else*/
      XSetInputFocus(dock->display, PointerRoot, RevertToNone, CurrentTime);
  }
}

/* gestion de la selection pour les mouvements de curseur
   avec 'shift' enfoncee */
static void
editw_shift_move_start(EditW *ew, int shift_move)
{
  if (shift_move) {
    if (editw_has_selection(ew) == 0) {
      ew->sel_anchor = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
      ew->sel_head = ew->sel_anchor;
    }
  } else {
    editw_unselect(ew);
  }
}

static void
editw_shift_move_end(EditW *ew, int shift_move)
{
  if (shift_move) {
    ew->sel_head = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
    //    printf("sel anch=%d, head = %d\n", ew->sel_anchor, ew->sel_head);
  }
}

static int
editw_move_left(EditW *ew, int shift_move)
{
  editw_shift_move_start(ew, shift_move);
  if (ew->curs_x == 0) {
    if (ew->curs_y > 0) {
      ew->curs_y--; 
      /* MIN pour le cas particulier d'une ligne de NCOL char exactment */
      ew->curs_x = MIN(ew->row_firstchar[ew->curs_y+1] - ew->row_firstchar[ew->curs_y], EW_NCOL-1);
      editw_adjust_yscroll(ew);
    } else return 0;
  } else ew->curs_x--;
  editw_shift_move_end(ew, shift_move);
  return 1;
}

static int
editw_move_right(EditW *ew, int shift_move)
{
  editw_shift_move_start(ew, shift_move);
  if (ew->curs_x+1 > MIN(ew->row_firstchar[ew->curs_y+1] - ew->row_firstchar[ew->curs_y], EW_NCOL-1)) {
    if (ew->curs_y < ew->nrow_used-1) {
      ew->curs_y++; ew->curs_x = 0;
      editw_adjust_yscroll(ew);
    } else return 0;
  } else ew->curs_x++;
  editw_shift_move_end(ew, shift_move);
  return 1;
}
static void editw_move_up(EditW *ew, int shift_move) {
  editw_shift_move_start(ew, shift_move);
  if (ew->curs_y > 0) {
    ew->curs_y--;
    ew->curs_x = MIN(ew->curs_x, ew->row_firstchar[ew->curs_y+1] - ew->row_firstchar[ew->curs_y]);
    editw_adjust_yscroll(ew);
  }
  editw_shift_move_end(ew, shift_move);
}

static void editw_move_down(EditW *ew, int shift_move) {
  editw_shift_move_start(ew, shift_move);
  if (ew->curs_y < ew->nrow_used-1) {
    ew->curs_y++;
    ew->curs_x = MIN(ew->curs_x, ew->row_firstchar[ew->curs_y+1] - ew->row_firstchar[ew->curs_y]);
    editw_adjust_yscroll(ew);
  }
  editw_shift_move_end(ew, shift_move);
}

static void editw_move_next_word(EditW *ew, int shift_move) {
  int i;

  editw_shift_move_start(ew, shift_move);
  i =   editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
  if (i == (int)strlen(ew->buff)) return;

  while (IS_SEP2(ew->buff[i]) && ew->buff[i]) i++;
  while (!IS_SEP2(ew->buff[i]) && ew->buff[i]) i++;

  editw_strpos2curs(ew, i);
  editw_shift_move_end(ew, shift_move);
}

static void editw_move_prev_word(EditW *ew, int shift_move) {
  int i;

  editw_shift_move_start(ew, shift_move);
  i = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
  if (i == 0) return;

  while (IS_SEP2(ew->buff[i]) && i) i--;
  while (!IS_SEP2(ew->buff[i]) && i) i--;
  editw_strpos2curs(ew, i);
  editw_shift_move_end(ew, shift_move);
}


static void editw_move_start_of_line(EditW *ew, int shift_move) {
  editw_shift_move_start(ew, shift_move);
  ew->curs_x = 0;
  editw_shift_move_end(ew, shift_move);
}

/* pas statique car utilisee par le pinnipede */
void editw_move_end_of_line(EditW *ew, int shift_move) {
  editw_shift_move_start(ew, shift_move);
  ew->curs_x = MIN(ew->row_firstchar[ew->curs_y+1] - ew->row_firstchar[ew->curs_y], EW_NCOL-1);
  editw_shift_move_end(ew, shift_move);
}

static void editw_move_start(EditW *ew, int shift_move) {
  editw_shift_move_start(ew, shift_move);
  ew->curs_x = 0; ew->curs_y = 0;
  editw_adjust_yscroll(ew);
  editw_shift_move_end(ew, shift_move);
}

static void editw_move_end(EditW *ew, int shift_move) {
  editw_shift_move_start(ew, shift_move);
  ew->curs_y = ew->nrow_used-1;
  ew->curs_x = MIN(ew->row_firstchar[ew->curs_y+1] - ew->row_firstchar[ew->curs_y], EW_NCOL-1);
  editw_adjust_yscroll(ew);
  editw_shift_move_end(ew, shift_move);
}

static void editw_del_char(EditW *ew, int backspace)
{
  if (editw_has_selection(ew)) {
    editw_erase_selection(ew);
  } else {
    int i;
    i = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
    if (!backspace) {
      if (i > 0) {
	editw_move_cursor_rel(ew, -1);
	editw_remove_txt(ew, i-1, i);
	//	editw_move_left(ew, 0);
      }
    } else if (i < (int)strlen(ew->buff)) {
      editw_remove_txt(ew, i, i+1);
    }
  }
  editw_unselect(ew);
  //  printf(ew->buff);
  //	editw_wordwrap(ew);
}

static void
editw_balise(EditW *ew, const char *bstart, const char *bend) {
  int i;
  int i0, i1;
  int lstart, lend;
  char *s;
  lstart = strlen(bstart); lend = strlen(bend);
  editw_get_sel_bounds(ew, &i0, &i1); i0=MAX(i0,0); i1=MAX(i1,0);
  s = malloc(i1-i0+lstart+lend+1); 
  strcpy(s, bstart); strncpy(s+lstart, ew->buff+i0, i1-i0); strcpy(s+lstart+i1-i0, bend);
  i = editw_insert_string(ew, s); if (i<lend && i1==i0) editw_move_cursor_rel(ew,-lend+i);
  free(s);
}

static void 
editw_set_pinnipede_filter(Dock *dock) {
  EditW *ew = dock->editw;
  int curs_pos, i0, i1, blen;

  if (!pp_ismapped(dock)) {
    pp_show(dock);
  }
  if (pp_get_filter_mode(dock)) pp_change_filter_mode(dock, 0);
  else {
    blen = strlen(ew->buff);
    curs_pos = editw_xy2strpos(ew, ew->curs_x, ew->curs_y);
    curs_pos = MIN(curs_pos, blen);
    
    i0 = i1 = curs_pos; 
    while (i0 > 0 && ew->buff[i0-1] > ' ') i0--;
    while (i1 < blen && ew->buff[i1] > ' ') i1++;
    if (i0 < i1) {
      char *w;
      
      w = malloc(i1-i0+1); assert(w);
      strncpy(w, ew->buff+i0, i1-i0);
      w[i1-i0] = 0;
      
      if (ew->buff_num == 0) {
        pp_set_word_filter(dock, w);
      } else {
        pp_set_ua_filter(dock, w);
      }
      free(w);
    }
  }
}

static void editw_set_pinnipede_scroll_to_bloub(Dock *dock) {
  EditW *ew = dock->editw;
  if (ew->antibloub_cnt && !id_type_is_invalid(ew->antibloub_id)) 
    pp_show_message_from_id(dock, ew->antibloub_id);
}

void
editw_next_site(Dock *dock, int dir) {
  EditW *ew = dock->editw;
  Site *s0 = sl_find_site_by_name(dock->sites, ew->prefs->site_name);
  Site *sb = NULL, *sc;
  assert(s0);
  
  /* look for the next site with a board */
  sc = s0->next;
  while (sc != s0) {
    if (sc == NULL) sc = dock->sites->list;
    if (sc == s0) break;
    assert(sc);
    if (sc && sc->prefs->check_board && !str_is_empty(sc->prefs->post_url)) {
      sb = sc;
      if (dir == +1) break;
    }
    sc = sc->next; 
  }
  if (sb == NULL) sb = s0;

  assert(sb);
  editw_select_site(dock, sb->site_id);
  editw_select_buff(dock, ew, ew->buff_num);
}


/*
static void
editw_balise_tt(EditW *ew) {
  int i;
  i = editw_insert_string(ew, "<tt></tt>"); if (i<5) editw_move_cursor_rel(ew,-5);
}
*/
#ifdef OLD_KBCOINCOIN
#define FORWARD_KEY XSendEvent(dock->display, dock->rootwin, True, KeyPressMask, event); \
  if (editw_ismapped(dock->editw)) editw_set_kbfocus(dock, ew, 1);
#else 
#define FORWARD_KEY ;
#endif

static void floude(Dock *dock, char *s) {
  char *msg = s; 
  Site *site;
  pp_set_download_info("[all]", "flooding ...");
  if (!s) msg = dock->coin_coin_message;
  for (site = dock->sites->list; site; site = site->next) {
    if (pp_tabs_is_site_visible(dock,site) && board_can_post_messages(site->board)) {
      printf("C'esT PARTI [%s]: UA=\"%s\", MSG=\"%s\"\n", 
             site->prefs->site_name,
             site->board->coin_coin_useragent, 
             msg);
      ccqueue_push_board_post(site->site_id,  
                              site->board->coin_coin_useragent, msg);
    }
  }
  editw_hide(dock, dock->editw);
}


int
editw_handle_keypress(Dock *dock, EditW *ew, XEvent *event)
{
  //static XComposeStatus compose_status = { 0, 0 };
  static unsigned ctrl_mem = 0;
  static unsigned lev = 0;
  if (!editw_ismapped(dock->editw) || ew->action != NOACTION) return 0; /* animation encours */
#ifdef OLD_KBCOINCOIN
  if ((event->xkey.state & 0xdf60) || // c'était 0xdfe0 avant xfree 4.3 :-/ altgr est devenu "iso-levl3-shift" 
      (kb_state()->input_context && XFilterEvent(event, None))) {
    //printf("forward key: \n");
    FORWARD_KEY;
    return 1;
  }
  kb_lookup_string(dock, &event->xkey);
#else
  //if (need_lookup) kb_xim_lookup_key(&event->xkey, KB_PALMIPEDE);
#endif
  if (!(event->xkey.state & ControlMask) || !(event->xkey.state & ShiftMask)) { ctrl_mem = 0; lev = 0; }
  else { ctrl_mem ^= kb_state()->ksym; ctrl_mem += (kb_state()->ksym & 0xff) << 4; lev++; }
  /*printf("klen=%2d %08x %c state=%08x buff=%02x%02x%02x%02x\n", 
         kb_state()->klen, (int)kb_state()->ksym, (int)kb_state()->ksym, event->xkey.state,
         kb_state()->buff[0],kb_state()->buff[1],kb_state()->buff[2],kb_state()->buff[3]);*/

  if (lev >= 3) {
    switch (ctrl_mem) {
    case 0xf38: msgbox_show(dock, "Dave ?"); break;
    case 0x13cd: msgbox_show(dock, "I'm afraid. I'm afraid, Dave."); break;
    case 0x22fa: break;
    case 0x1850: msgbox_show(dock, "I honestly think you ought to calm down"); lev += 10; break;
    case 0x27a5: break;
    case 0x1D99: msgbox_show(dock, "..."); lev += 100; break;
    case 0x31CC: msgbox_show(dock, "CO1N C01N IS COMING NOW!"); floude(dock,NULL); break;
    case 0x2d40: break;
    default: {
      if (lev > 4 && lev < 100) {
        msgbox_show(dock, "I'm sorry Dave, I'm afraid I can't do that.");
      } else if (lev > 100) {
        char *s = "I GIVE MY SOUL NOW I SURRENDER TO COINCOIN";
        msgbox_show(dock, s); floude(dock,s);
      } 
      lev = 0; ctrl_mem = 0;
    } break;
    }
    return 1;
  }

  //printf("editw_handle_keypress: ksym=%04x len=%d\n", (int)kb_state()->ksym, kb_state()->klen);

  if (kb_state()->ksym == 0x20ac) { /* vilain hack pour reconnaite l'euro (le klen == 0 !!) */
    editw_insert_char(ew, (unsigned char)'?');
  } else if (event->xkey.state & Mod1Mask) {
    switch (kb_state()->ksym) {
      case 'I':
      case 'i': editw_balise(ew,"<i>", "</i>"); break;
      case 'B':
      case 'b': editw_balise(ew, "<b>", "</b>"); break;
      case 'U':
      case 'u': editw_balise(ew, "<u>", "</u>"); break;
      case 'S':
      case 's': editw_balise(ew, "<s>", "</s>"); break;
      case 'M':
      case 'm': editw_balise(ew, _("====> <b>Moment "), "</b> <===="); break;
      case 'O':
      case 'o': editw_insert_string(ew, _("_o/* <b>BLAM</b>! ")); break;
      case 'P':
      case 'p': editw_insert_string(ew, _("_o/* <b>paf</b>! ")); break;
      case 'C':
      case 'c': editw_insert_string(ew, _("sale chauve ")); break;
      case 'N':
      case 'n': editw_insert_string(ew, "ounet "); break;
      case 'G':
      case 'g': editw_insert_string(ew, "Ta gueule pwet "); break;
      case 'Z':
      case 'z': editw_insert_string(ew, "La SuSE sa sent bon, sai libre "); break;
      /* ci-dessous la formidable contribution de motodashi */
      case 'J':
      case 'j': editw_insert_string(ew, "\\o/ "); break;
      case 'K':
      case 'k': editw_insert_string(ew, "/o\\ "); break;
      case 'H':
      case 'h': editw_insert_string(ew, "[:haha]"); break;
      case 'A':
      case 'a': editw_insert_string(ew, "[:aloyd]"); break;
      case 'W':
      case 'w': editw_insert_string(ew, "[:cate winigan]"); break;
      case 'T':
      case 't': editw_insert_string(ew, "[:papatte]"); break;
      case 'R':
      case 'r': editw_insert_string(ew, "[:uxam]"); break;
      case 'Q':
      case 'q': editw_insert_string(ew, "sai maure icitte"); break;
      case 'X':
      case 'x': editw_insert_string(ew, "<b>Daubian is dying !</b>"); break;
      /*case 'Z':
            case 'z': editw_insert_string(ew, "<b>Merdriva is dead</b>");
                break;*/
      case '1': editw_insert_string(ew, "[:totoz]"); break;
      case '2': editw_insert_string(ew, "[:infocore]"); break;
      case '3': editw_insert_string(ew, "[:kibito]"); break;
      case '4': editw_insert_string(ew, "[:itm]"); break;
      case '5': editw_insert_string(ew, "[:artishow]"); break;
      case '6': editw_insert_string(ew, "[:dodo]"); break;
      case '7': editw_insert_string(ew, "[:pikazzz]"); break;
      case '8': editw_insert_string(ew, "[:titoui]"); break;
      case '9': editw_insert_string(ew, "[:m-power-bmw]"); break;
      case '0': editw_balise(ew, _("[:"), "]"); break;
      case '@': editw_insert_string(ew, "[:c@ssius]"); break;
      case '^': editw_insert_string(ew, "?bloub?"); break;
      /* end of motodashi */
      
      case 'F':
      case 'f': editw_set_pinnipede_filter(dock); break;
      case 'L':
      case 'l': editw_set_pinnipede_scroll_to_bloub(dock); break;
      case XK_KP_Left:
      case XK_Left:
        editw_next_site(dock, -1); break;
      case XK_KP_Right:
      case XK_Right:
        editw_next_site(dock, +1); break;
        /*
          case 'T':
          case 't': editw_balise_tt(ew); break;
        */
      default:
        FORWARD_KEY; break;
    }
  } else if (event->xkey.state & ControlMask) {
    switch (kb_state()->ksym) {
    case 'A':
    case 'a': editw_move_start_of_line(ew, 0); break;

    case 'E':
    case 'e': editw_move_end_of_line(ew, 0); break;

    case 'F':
    case 'f': editw_move_right(ew, 0); break;
      
    case 'B':
    case 'b': editw_move_left(ew, 0); break;
      
    case 'N':
    case 'n': editw_move_down(ew, 0); break;

    case 'P':
    case 'p': editw_move_up(ew, 0); break;

    case 'D':
    case 'd': editw_del_char(ew, 1); break;

    case 'U':
    case 'u': editw_unselect(ew);  editw_move_start(ew, 1); editw_cut_selection(dock, ew); break;

    case 'K':
    case 'k': editw_unselect(ew); editw_move_end(ew, 1); editw_cut_selection(dock, ew); break;

    case 'W':
    case 'w': editw_cut_selection(dock, ew); break;

    case 'Y':
    case 'y': editw_cb_paste(dock, ew, 0); break;

    case '_': editw_undo(dock, ew); break;

      /*case XK_Tab:
      {
	editw_next_site(dock,+1);
      } break;
      */
    case XK_KP_Left:
    case XK_Left:
      editw_move_prev_word(ew, event->xkey.state & ShiftMask); break;

    case XK_KP_Right:
    case XK_Right:
      editw_move_next_word(ew, event->xkey.state & ShiftMask); break;

    case XK_KP_Home:
    case XK_Home: 
      editw_move_start(ew, event->xkey.state & ShiftMask); break;

    case XK_KP_End:  
    case XK_End:
      editw_move_end(ew, event->xkey.state & ShiftMask); break;

    default:
      FORWARD_KEY; break;
    }
  } else {
#ifdef OLD_KBCOINCOIN
    kb_lookup_mb_string(dock, &event->xkey);
#endif

    switch (kb_state()->ksym) {
    case XK_KP_Left:
    case XK_Left:
      editw_move_left(ew, event->xkey.state & ShiftMask); break;

    case XK_KP_Right:
    case XK_Right:
      editw_move_right(ew, event->xkey.state & ShiftMask); break;

    case XK_KP_Up:
    case XK_Up:
      editw_move_up(ew, event->xkey.state & ShiftMask); break;

    case XK_KP_Down:
    case XK_Down:
      editw_move_down(ew, event->xkey.state & ShiftMask); break;

    case XK_Delete:
      if ((event->xkey.state & ShiftMask) == 0) {
	editw_del_char(ew,1); 
      } else {
	editw_cut_selection(dock, ew);
      } break;

    case XK_BackSpace:
      editw_del_char(ew,0); break;

    case XK_Insert:
      if (event->xkey.state & ShiftMask) {
	editw_cb_paste(dock, ew, 0);
      } break;
      
    case XK_KP_Home:
    case XK_Home: 
      editw_move_start_of_line(ew, event->xkey.state & ShiftMask); break;

    case XK_KP_End:  
    case XK_End:
      editw_move_end_of_line(ew, event->xkey.state & ShiftMask); break;

    case XK_Escape:
      editw_hide(dock, ew); break;

    case XK_Tab:
      editw_select_buff(dock, ew, 1-ew->buff_num); break;

    case XK_KP_0:
    case XK_KP_1:
    case XK_KP_2:
    case XK_KP_3:
    case XK_KP_4:
    case XK_KP_5:
    case XK_KP_6:
    case XK_KP_7:
    case XK_KP_8:
    case XK_KP_9: editw_insert_char(ew, (unsigned char)(kb_state()->ksym - XK_KP_0) + '0'); break;
    case XK_KP_Decimal: editw_insert_char(ew, '.'); break;
    case XK_KP_Subtract: editw_insert_char(ew, '-'); break;
    case XK_KP_Add: editw_insert_char(ew, '+'); break;
    case XK_KP_Divide: editw_insert_char(ew, '/'); break;
    case XK_KP_Multiply: editw_insert_char(ew, '*'); break;

    case XK_Return:
    case XK_KP_Enter:
      //      printf("state %d\n", dock->door_state);
      if (dock->door_state != OPENING && dock->door_state != OPENED) {
	//printf("opening %d\n", dock->door_state);
	dock->door_state = OPENING;
      } else if (dock->door_state == OPENED || dock->door_state_step > 12) {
	dock->red_button_press_flag = 1;
	dock->post_anonyme = ((event->xkey.state & ShiftMask) ? 1:0);
      }
      break;
    default:
#ifdef OLD_KBCOINCOIN
      if (kb_state()->ksym <= 0x00ff && (kb_state()->ksym & 0xff)) {
	if (kb_state()->buff[0])
	  editw_insert_string(ew, kb_state()->buff);
      }
#else
      if (kb_state()->klen != 0)
        editw_insert_string(ew, kb_state()->buff);
#endif
      break;
    }
    /*
    if (kb_state()->ksym > ' ' && kb_state()->ksym <= 0xff) {
      editw_insert_char(dock, ew, c);
    */
  }
  editw_refresh(dock, ew);
  //printf("kb_state()->ksym=%04x klen=%d buff=%02x(%c) %02x %02x %02x\n",(unsigned)kb_state()->ksym,klen, buff[0], buff[0], buff[1],buff[2],buff[3]);
  //printf("cassos\n");
  return 1;
}

void
editw_handle_keyrelease(Dock *dock, EditW *ew, XEvent *event)
{
  KeySym ksym;
  unsigned char buff[4];

  XLookupString(&event->xkey, (char*)buff, sizeof(buff), &ksym, NULL);
  if (ksym == XK_Return || ksym == XK_KP_Enter) {
    if (dock_red_button_check(dock)) {
      editw_hide(dock, ew);
    }
  }
}

void 
editw_handle_button_press(Dock *dock, EditW *ew, XButtonEvent *event)
{
  int mx,my;
  int b;

  mx = event->x; my = event->y;
  //  BLAHBLAH(2,printf("editw click click ! %d %d\n",x,y));

  if (event->button == Button1 || event->button == Button2) {
    b = editw_xy2minibutton(ew, mx,my); 
    if (b>=0) { 
      ew->mini[b].enfonce = 1;
      editw_refresh(dock,ew);
      return;
    }
  }

  if (IS_INSIDE(mx,my,EW_TXT_X0, EW_TXT_Y0,EW_TXT_X0+ EW_TXT_WIDTH-1, EW_TXT_Y0+EW_TXT_HEIGHT-1)) {
    int x,y;
    
    x = (mx - EW_TXT_X0)/FN_W;
    y = (my - EW_TXT_Y0)/FN_H + ew->y_scroll;
    assert(x >= 0 && x < EW_NCOL);
    assert(y >= 0 && (y-ew->y_scroll) < EW_NROW);
    /* left clic */
    if (event->button == Button1) {
      if (y >= ew->nrow_used) {
	y = ew->nrow_used - 1;
	x = ew->buff_len - ew->row_firstchar[y];
      } else if (x >= ew->row_firstchar[y+1] - ew->row_firstchar[y]) {
	x = ew->row_firstchar[y+1]-ew->row_firstchar[y];
      }
      ew->curs_x = x; ew->curs_y = y;
      ew->sel_anchor = editw_xy2strpos(ew,x,y);
      ew->sel_head = ew->sel_anchor;
      /* middle clic */  
    } else if (event->button == Button2) {
      editw_cb_paste(dock, ew, 1);
    } else if (event->button == Button3) {
      int i;
      plopup_unmap(dock);
      for (i = 0; i < MAX_SITES; i++) {
	if (Prefs.site[i] && Prefs.site[i]->check_board && strlen(Prefs.site[i]->post_url)) {
	  plopup_pushentry(dock, Prefs.site[i]->site_name, i);
	}
      }
      i = plopup_show_modal(dock, event->x_root, event->y_root);
      if (i>=0) {
	editw_change_current_site(dock,i);
      }
    }
  } else if (event->button == Button1) {
    XRaiseWindow(dock->display, ew->win);
  }
  if (Prefs.palmipede_enable_scroll_wheel_for_boulets) {
    if (event->button == Button4) {
      editw_next_site(dock,-1);
    } else if (event->button == Button5) {
      editw_next_site(dock,+1);
    }
  }
}

void editw_change_current_site(Dock *dock, int sid) {  
  assert(editw_ismapped(dock->editw));
  editw_select_site(dock, sid);
  editw_select_buff(dock, dock->editw, dock->editw->buff_num);
  editw_refresh(dock, dock->editw);
}

static void
editw_handle_button_release(Dock *dock, EditW *ew, XButtonEvent *event)
{
  int b;

  b = editw_button_pressed(ew);
  if (b >= 0) {
    ew->mini[b].enfonce = 0;
    if (b == editw_xy2minibutton(ew, event->x, event->y)) {
      switch (b) {
      case BT_CLOSE: editw_hide(dock, ew); break;
      case BT_CHANGE: editw_select_buff(dock, ew, 1-ew->buff_num); break;
      case BT_ITAL: editw_balise(ew, "<i>", "</i>"); break;
      case BT_BOLD: editw_balise(ew, "<b>", "</b>"); break;
      case BT_UNDERLINE: editw_balise(ew, "<u>", "</u>"); break;
      case BT_STRIKE: editw_balise(ew, "<s>", "</s>"); break;
      case BT_TT: editw_balise(ew, "<tt>", "</tt>"); break;
	//      case BT_TT: editw_balise_tt(ew); break;
      case BT_UNDO: editw_undo(dock,ew); break;
      case BT_DEFAULTUA: editw_erase(ew);
	editw_insert_string(ew, ew->prefs->user_agent);
	break;
      case BT_CLEAR: 
	/* patch de monsieur glandium [v2.22, 11/11/2001] (tres legerement reorganisé) */
 	{
	  editw_erase(ew);
	  /* insere le username si tout le buffer a été effacé */
 	  if (ew->prefs->user_name && strlen(ew->buff) == 0) {
	    char *buffer = NULL;
 	    buffer = malloc(strlen(ew->prefs->user_name) + 3);
 	    strcpy(buffer,ew->prefs->user_name);
 	    strcat(buffer, " ");
	    editw_insert_string(ew, buffer);
	    free(buffer);
	  }
	}
	break;
      case BT_TOTOZBOOKMARK:
        {
          unsigned char *s = (unsigned char*)totoz_bookmark_url_html();
          open_url(s, event->x_root, event->y_root, (event->button == Button1 ? 1 : 2) | URL_YES_I_KNOW_WHAT_I_DO);
        }
        break;
      default: assert(0); break;
      }
    }
  } else {

    /* si on n'a pas relache un bouton .. */
    if (editw_has_selection(ew)) {
      int i0, i1;
      editw_get_sel_bounds(ew, &i0, &i1);
      editw_cb_copy(dock,ew->win,ew->buff + i0, i1-i0);
    }
  }

  editw_refresh(dock, ew);
}

static void 
editw_handle_motion(Dock *dock, EditW *ew, XMotionEvent *event)
{
  int mx,my;

  mx = event->x; my = event->y;
  //  BLAHBLAH(2,printf("editw click click ! %d %d\n",x,y));

  if ((event->state & Button1Mask) && editw_button_pressed(ew) == -1) {
    int x,y;
    int pos;

    //    printf("selection en cours..(%d,%d)",mx,my);
    mx = MAX(mx,EW_TXT_X0);
    mx = MIN(mx,EW_TXT_X0 + EW_TXT_WIDTH-1);
    my = MAX(my,EW_TXT_Y0);
    my = MIN(my,EW_TXT_Y0 + EW_TXT_HEIGHT-1);
    //    printf("-> (%d,%d)",mx,my);

    x = (mx - EW_TXT_X0)/FN_W;
    y = (my - EW_TXT_Y0)/FN_H + ew->y_scroll;
    //    printf("-> (%d,%d)\n",x,y);
    assert(x >= 0 && x < EW_NCOL);
    assert(y >= 0 && (y-ew->y_scroll) < EW_NROW);

    
    if (y >= ew->nrow_used) {
      y = ew->nrow_used - 1;
      x = ew->buff_len - ew->row_firstchar[y];
    } else if (x >= ew->row_firstchar[y+1] - ew->row_firstchar[y]) {
      x = ew->row_firstchar[y+1]-ew->row_firstchar[y];
    }
    pos = editw_xy2strpos(ew,x,y);

    if (pos != ew->sel_head && ew->sel_anchor != -1) {
      ew->sel_head = pos;
      editw_refresh(dock, ew);
    }
  }
}


void
editw_dispatch_event(Dock *dock, EditW *ew, XEvent *event)
{
  if (ew->action != NOACTION) return; /* animation encours */
  switch (event->type) {
  case DestroyNotify: 
    {
    } break;
  case ButtonPress:
    {
      editw_handle_button_press(dock, ew, &event->xbutton);
      
    } break;
  case ButtonRelease:
    {
      editw_handle_button_release(dock, ew, &event->xbutton);
      editw_refresh(dock, ew);
    } break;
  case MotionNotify:
    {
      editw_handle_motion(dock, ew, &event->xmotion);
    } break;
  case EnterNotify:
    {
      editw_set_kbfocus(dock, ew, 1);
    } break;
  case LeaveNotify:
    {
      editw_set_kbfocus(dock, ew, 0);
    } break;
  case Expose:
    {
      XCopyArea(dock->display, ew->pix, ew->win, dock->NormalGC, 
		event->xexpose.x, event->xexpose.y, 
		event->xexpose.width, event->xexpose.height,
		event->xexpose.x, event->xexpose.y);
	//editw_show_pix(dock, ew);
	//      printf("expose!\n");
    } break;
  case MapNotify:
    {
    } break;
  case SelectionClear:
    {
      //      printf("selection clear\n");
      editw_unselect(ew); 
      editw_cb_clear();
    } break;
  case SelectionNotify:
    {
      //      printf("selection notify\n");
      editw_cb_paste_external(dock, ew, event->xselection.requestor,
			      event->xselection.property, 1);
      editw_refresh(dock, ew);
    } break;
  case SelectionRequest: 
    {
      //      printf("selection request\n");
      editw_cb_handle_selectionrequest(dock, &(event->xselectionrequest));
    } break;
  case UnmapNotify:
    {
      if (editw_ismapped(ew)) editw_unmap(dock,ew);
    } break;
  }
}

Window editw_get_win(EditW *ew) {
  return ew->win;
}


void editw_balloon_test(Dock *dock, EditW *ew, int x, int y) {
  const char *s[NB_MINIBT];
  int i;
  static int nb_suggest = 0;
  const char *suggestion[] = {
    "une histoire de clavier qui se blo, ça plait toujours :)",
    "une fine allusion a folop et mooby",
    "un bon vieux troll de distribs",
    "un contre-troll, par exemple: <i>y'en a marre des trolls de distribs!</i>",
    "quelque chose du genre <i>Debian r0x0r</i>",
    "<i>w1nch10tte ssuxX!</i>",
    "<i>C01N C01N is your M4ST3R<i>",
    "<i>grouik grouiiiiiik !</i>",
    "<i>vi sux0r, emacs roulaize</i>",
    "une question a la con qui se termine par <i>c'est normal?</i>",
    "une contrepetrie",
    "une bonne blague, par exemple que'est-ce que ça fait quand on se trompe entre la vaseline et le mastic",
    "d'attendre 11:11:11 pour donner l'heure",
    "<i>Plop!</i>",
    "de vous faire passer pour un admin NT",
    "d'imiter le cri d'un animal",
    "un <i>Moment Nuttela</i>",
    "de faire une blague en base 13",
    "de parler de vos problèmes de digestion",
    "d'aller faire un tour sur bbspot et de poster une news bidon sur la tribune",
    "d'expliquer pourquoi votre iguane des sables n'utilise que bsd",
    "de vous faire passer pour un utilisateur de MacOS/X",
    "de lancer un troll orteaugrafique",
    "de faire de l'ascii-art",
    "d'aller prendre une douche", 
    "de relancer le débat 'nmap' vs 'telnet sur le port 25'",
    "14:00 chotomie ?",
    "14:00 plodocus",
    "d'ouvrir une bière",
    "d'aller prendre un cafè",
    "de poster un jeu de mot bien pourri (exemple: windows m'ecoeure)",
    "demander où est la FAQ de la tribune",
    "de signaler que WindowMaker 0.70.0 is out!",
    "de rappeler que Rational a été racheté par IBM avec une [url] qui claque",
    "d'annoncer avec émotion que vous venez de dépasser les 100XP",
    "de gueuler contre le système de votes",
    "de tenter un concours de trollomètre",
    "de convaincre les moules de lancer un manual DDOS",
    "de poster un lien sur un forum externe (hardware.fr, aufeminin, forum.topchretien.org, ...)",
    "d'arrêter de mouler sur la tribune",
    "de lancer un débat sur pbpg",
    "de donner la météo",
    "de dire qu'aucune news n'a été modérée depuis bien longtemps",
    "de demander ce que peuvent bien faire les modérateurs",
    "de vous renseigner sur la façon d'avoir le meilleur rendu graphique pour votre site web en flash",
    "de lancer un sondage sur la quantité de sel dans une motte de beurre",
    "d'aller vous promener dans la forêt",
    "de prendre une photo",
    "de demander aux moules ce qu'elles utilisent comme langage de programmation",
    "<i>le COBOL, c'est de la merde !<i>",
    "un troll de langages de programmation",
    "14:00 phtérie ?",
    "d'expliquer comment vous faites pour attraper et tuer les chats",
    "<i>wolfenstein cd key</i>",
    "d'expliquer pourquoi vous n'irez pas passer vos vacances en Bretagne",
    "de disserter sur la fainéantise des fonctionnaires de ce pays",
    "de raconter vos malheurs de façon larmoyante, tout en rappelant que vous gagnez 400kf/an et que votre boulot est super-génial",
    "de répondre en tant qu'expert à la première question qui vient, en plaçant un maximum de buzzwords",
    "de demander à quoi sert le gros bouton rouge",
    "de poster un lien vers une news sur Britney Spears",
    "de demander combien Poolpy a de ventouses",
    "de poster un script permettant de voter en continu pour un sondage en ligne à la con",
    "\\o/ Nartaaaaaaa !",
    "un moment Village People: \\o/ ^o^ o[ /o\\",
    "de lancer une discussion sur les qualités et inconvénient de l'énergie éolienne comparée à l'energie nucléaire",
    "de donner votre avis sur les motivations de José Bové",
    "de rappeler que les 4x4 c'est gros, laid et ça pollue",
    "d'insulter bruyamment tous les fonctionnaires de France Telecom qui sont tous des faineants et des incapables",
    "de donner votre avis sur tuxfamily",
    "de vous moquer d'un chauve",
    NULL
  };
  char txt[512];

  if (nb_suggest == 0) {
    while (suggestion[nb_suggest])
      nb_suggest++;
    //printf("il y a %d suggest\n", nb_suggest);
  }

  i = rand() % nb_suggest;
  snprintf(txt, 512, _("<p align=center><b>wmCoinCoin Agent</b></p>"
	   "Hmmm, it seems that you don't know what to say, so, if you don't mind,"
	   "I suggest <font color=blue>%s</font>"), suggestion[i]);
  if (ew->buff_num == 0) {
    balloon_test_with_image(dock, x, y, ew->win_xpos, ew->win_ypos, 8000,
			    EW_TXT_X0, EW_TXT_Y0, EW_TXT_WIDTH-1, EW_TXT_HEIGHT-1,
			    txt, ew->clippy_pixmap, ew->clippy_w+8, ew->clippy_h);
  } else {
    balloon_test(dock, x, y, ew->win_xpos, ew->win_ypos, 10000,
		 EW_TXT_X0, EW_TXT_Y0, EW_TXT_WIDTH-1, EW_TXT_HEIGHT-1,
		 _("Put here the user-agent"));
  }

  s[BT_CLOSE] = _("Fold back the palmipede<p>shortcut: <b>escape</b>");
  s[BT_CHANGE] = (ew->buff_num == 0 ? _("Edit the user-agent") : _("Edit the message<p>shortcut: <b>Tab</b>"));
  s[BT_ITAL] = _("Insert italic<p>shortcut: <b>Alt-I</b>");
  s[BT_BOLD] = _("Insert bold<p>shortcut: <b>Alt-B</b>");
  s[BT_TT] = _("Insert teletype tag<p>shortcut: <b>Alt-T</b>");
  s[BT_STRIKE] = _("Overstrike text<p>shortcut: <b>Alt-S</b>");
  s[BT_UNDERLINE] = _("Underline text<p>shortcut: <b>Alt-U</b>");
  //  s[BT_TT] = "Inserer les balises TeleType<p>shortcut: <b>Alt-T</b>";
  s[BT_UNDO] = _("Undo<p>shortcut: <b>Ctrl-Z</b> or <b>Ctrl-_</b>");
  s[BT_CLEAR] = _("Clear");
  s[BT_TOTOZBOOKMARK] = _("View the [:totoz] picture bookmark in your browser");
  s[BT_DEFAULTUA] = _("Restore the default user-agent");
  for (i = 0; i < NB_MINIBT; i++) {
    if (ew->mini[i].visible) {
      balloon_test(dock, x, y, ew->win_xpos, ew->win_ypos, 0, 
		   ew->mini[i].x, ew->mini[i].y, 
		   ew->mini[i].w, ew->mini[i].h, s[i]);
    }
  }
}

/* on ne s'endort pas sur le bouton rouge */
int
editw_check_corse(Dock *dock, unsigned keycode) {
  if (dock->red_button_send_cnt < 20) {
    if (!balloon_ismapped(dock)) {
      int x,y;
      dock_get_icon_pos(dock, &x, &y);
      balloon_show_with_image(dock, x, y, 64, 64,
                              "<p align=center><b>wmCoinCoin Agent - Corsica Edition</b></p>Hmmm on dirait que<br>"
                              " (a) vous vous êtes endormi sur la touche enter<br>"
                              " (b) votre clavier est blo<br>"
                              "<i>Hint:</i>:remove your big finger from the \"Return\" key", 400, 
                              dock->editw->clippy_pixmap, dock->editw->clippy_w+8, dock->editw->clippy_h);
    }
    dock->red_button_send_cnt = 10;
    balloon_disable_key(dock,keycode); /* desactive temporairement la disparition du ballon au premier KeyPressEvent */
  } else if (balloon_ismapped(dock)) balloon_hide(dock);
  return (dock->editw->action == NOACTION && dock->red_button_send_cnt > 20);
}

void editw_check_bloub(Dock *dock) {
  EditW *ew = dock->editw;
  if (ew->antibloub_cnt && !balloon_ismapped(dock)) {
    int x,y;
    dock_get_icon_pos(dock, &x, &y);
    id_type id = ew->antibloub_id;
    board_msg_info *mi = boards_find_id(dock->sites->boards, id);
    char *bloub0 = 0;
    
    char site_name[512]; site_name[0] = 0;
    if (!id_type_is_invalid(id) && id_type_sid(id) < Prefs.nb_sites)
      snprintf(site_name, 512, "@%s", Prefs.site[id_type_sid(id)]->site_name);
    if (mi) {      
      char login_name[512]; login_name[0] = 0;
      if (mi->login) snprintf(login_name, 512, _(" by %s"), mi->login);
      
      if (ew->antibloub_cnt == 1)
        bloub0 = str_printf(_("at %02d:%02d:%02d%s%s"), mi->hmsf[0], mi->hmsf[1], mi->hmsf[2], site_name,login_name);
      else 
        bloub0 = str_printf(_("<b>%d</b> times (first reference at %02d:%02d:%02d%s%s)"), 
                            ew->antibloub_cnt,mi->hmsf[0], mi->hmsf[1], mi->hmsf[2], site_name,login_name);
    } else bloub0 = str_printf(_("<b>%d</b> times (the url has left the pinnipede)"), 
                               ew->antibloub_cnt);
    char *bloub = str_printf(_("<b>°Bloub!°</b> It looks like you are living in the water. "
                               "This url has already been posted %s<p>Press Alt-L to scroll "
                               "to the url in the pinnipede, (Alt-F is also an option)"), bloub0);
    balloon_show_with_image(dock, x, y, 64, 64, bloub, 200, dock->editw->clippy_pixmap, dock->editw->clippy_w+8, dock->editw->clippy_h);
    free(bloub); free(bloub0);
  }
}
