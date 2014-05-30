#include <sys/types.h>
#include <sys/wait.h>
#include "coincoin.h"
#include "site.h"
#include <libintl.h>
#define _(String) gettext (String)

typedef struct _ccqueue {
  ccqueue_elt *first;
  int canceled;
  int state; /* -1 ou ccqueue_elt_type */
} ccqueue;

ccqueue queue;

void
ccqueue_build() {
  queue.first = NULL;
  queue.canceled = 0;
}

const char*
ccqueue_elt_type_2_str(ccqueue_elt_type t) {
  switch (t) {
  case Q_PREFS_UPDATE: return "PREFS_UPDATE"; break;
  case Q_BOARD_POST: return "BOARD_POST"; break;
  case Q_BOARD_UPDATE: return "BOARD_UPDATE"; break;
  case Q_NEWSLST_UPDATE: return "NEWSLST_UPDATE"; break;
  case Q_SMILEY_DL: return "SMILEY_DL"; break;
  default: return "Oups BUG"; break;
  }
  return NULL;
}

static void
ccqueue_push(ccqueue *q, ccqueue_elt_type what, int sid, char *ua, char *msg, int nid) {
  ccqueue_elt *qe, *pqe, *iq;
  int is_dup = 0;

  BLAHBLAH(2, printf("push: %s (%s) (%s)\n", ccqueue_elt_type_2_str(what), ua, msg));

  /* look for duplicates */
  for (qe = q->first; qe && is_dup == 0; qe = qe->next ) {
    if (qe->what == what) {
      if (sid != qe->sid) continue;
      if (nid != qe->nid) continue;
      if (what == Q_BOARD_POST) {
	assert(ua); assert(msg);
	if (strcmp(ua, qe->ua)) continue;
	if (strcmp(msg, qe->msg) && strcmp(msg, "pan ! pan !")) continue; /* on fait une exception pour le ball-trap, c'est tres tres moche */
      }
      if (what == Q_SMILEY_DL) {
        if (strcmp(msg, qe->msg)) continue;
      }
      is_dup = 1;
    }
  }

  if (is_dup) {
    BLAHBLAH(0, myprintf("dupliquate request: %s %s%s\n", ccqueue_elt_type_2_str(what), sid >= 0 ? "for " : "", sid >= 0 ? Prefs.site[sid]->site_name : ""));
    return;
  }

  ALLOC_OBJ(iq, ccqueue_elt);
  iq->what = what; iq->sid = sid; 
  iq->ua = ua ? strdup(ua) : NULL; 
  iq->msg = msg ? strdup(msg) : NULL; 
  iq->nid = nid;

  for (qe = q->first, pqe = NULL; qe; pqe = qe, qe = qe->next ) {
    if (qe->what > what) break;
  }
  iq->next = qe;
  if (pqe) {
    pqe->next = iq;
  } else {
    q->first = iq;
  }
}

void ccqueue_push_prefs_update(int whatfile)
{
  ccqueue_push(&queue, Q_PREFS_UPDATE, -1, NULL, NULL, whatfile);
}
void ccqueue_push_board_post(int sid, char *ua, char *msg)
{
  ccqueue_push(&queue, Q_BOARD_POST, sid, strdup(ua), strdup(msg), -1);
}
void ccqueue_push_board_update(int sid)
{
  ccqueue_push(&queue, Q_BOARD_UPDATE, sid, NULL, NULL, -1);
}
void ccqueue_push_newslst_update(int sid)
{
  ccqueue_push(&queue, Q_NEWSLST_UPDATE, sid, NULL, NULL, -1);
}
void ccqueue_push_smiley_dl(char *imgname)
{
  ccqueue_push(&queue, Q_SMILEY_DL, -1, NULL, imgname, -1);
}
ccqueue_elt*
ccqueue_find_next(ccqueue_elt_type what, int sid, ccqueue_elt *q) {
  if (q == NULL) q = queue.first;
  else q = q->next;
  for (; q; q = q->next) {
    if (q->what == what && (sid == -1 || q->sid == sid))
      return q;
  }
  return NULL;
}

ccqueue_elt*
ccqueue_find(ccqueue_elt_type what, int sid) {
  return ccqueue_find_next(what, sid, NULL);
}

/* is there a board to update ? */
/*
int
is_board_update_requested(SiteList *sl)
{
  Site *site;
  for (site = sl->list; site; site = site->next) {
    if (site->prefs->check_board && site->board->update_request == 1) {
      return 1;
    }
  }
  return 0;
}
*/

static void ccqueue_pop(ccqueue_elt *q) {
  ccqueue_elt *qq, *pq;
  qq = queue.first; pq = NULL;
  assert(q);
  while (qq && qq != q) {
    pq = qq;
    qq = qq->next;
  }
  assert(qq);
  if (pq == NULL) {
    queue.first = q->next;
  } else {
    pq->next = q->next;
  }
  if (q->ua) free(q->ua);
  if (q->msg) free(q->msg);
  free(q);
}
 
int ccqueue_state() {
  return queue.state;
}

void
ccqueue_print() {
  ccqueue_elt *q = queue.first;
  myprintf("----- queue : \n");
  for (; q; q = q->next) {
    myprintf("%<MAG %15s> [%s] [%d] [%s] [%s]\n", ccqueue_elt_type_2_str(q->what),
	     q->sid >= 0 ? Prefs.site[q->sid]->site_name : "??",
	     q->nid, q->ua, q->msg);
  }
  myprintf("------\n");
}

const ccqueue_elt *
ccqueue_doing_what()
{
  return (queue.state != -1 ? queue.first : NULL);
}

void ccqueue_loop(Dock *dock) {
  int save_state_cnt = 0;
  int nb_news_site = 0;
  Site *s;
  for (s = dock->sites->list; s; s = s->next) {
    //    if (s->prefs->check_news) nb_news_site++;
  }
  if (nb_news_site)
    strcpy(dock->newstitles, _("Transfer in progress..."));
  else strcpy(dock->newstitles, "coin!");

  while (1) {
    while (queue.first) {
      ccqueue_elt *q = queue.first;
      BLAHBLAH(2, ccqueue_print());
      queue.state = q->what;
      flag_cancel_task = 0;
      BLAHBLAH(2, printf("dealing with %s\n", ccqueue_elt_type_2_str(q->what)));
      switch (q->what) {
      case Q_PREFS_UPDATE: {
	wmcc_prefs_relecture(dock, q->nid); 
      } break;
      case Q_BOARD_POST: {
	Site *s;
	exec_coin_coin(dock, q->sid, q->ua, q->msg); 
	/*	dock->coin_coin_request = -50;*/ /* on va le repasser progressivement à zero (pour permettre à la led
						    de s'éteindre progressivement) */
	s = sl_find_site_id(dock->sites, q->sid);
	if (s == NULL) printf("RROOOOOOARRRRR!\n");
	if (s && s->board)
	  ccqueue_push_board_update(q->sid); /* va falloir mettre la tribune à jour */
      } break;
      case Q_BOARD_UPDATE: {
	Site *s = sl_find_site_id(dock->sites, q->sid);
	if (s && s->board) {
	  board_update(s->board);
          /*
          {
            board_msg_info *mi = dock->sites->boards->first;
            printf("SUMMARY OF GLOBAL BOARDS\n");
            while (mi) {
              printf("%20s id=%05d tstamp=%10ld corrected=%10ld\n", 
                     sl_find_site_id(dock->sites, id_type_sid(mi->id))->prefs->site_name,
                     id_type_lid(mi->id), mi->timestamp, mi->timestamp + dock->sites->boards->btab[mi->id.sid]->time_shift);
              mi = mi->g_next;
            }
            char s[15];
            time_t t0 = time(NULL);
            time_t_to_tstamp(t0,s);
            printf("t0 = %ld -> s=%s ->",t0,s);
            str_to_time_t(s,&t0); printf("t1=%ld\n", t0);
          }
          */
	}
      } break;
      case Q_NEWSLST_UPDATE: {
	/*Site *s = sl_find_site_id(dock->sites, q->sid);
	if (s) {
	  site_news_dl_and_update(s);
          }*/
      } break;
      case Q_SMILEY_DL: {
        static char download_info[100];
        /* L'url de l'image à télécharger devrait être dans q->msg */
        if ( q->msg == NULL )
          myfprintf(stderr, "mmm, il me semble que la fonction de récupération de smiley a ete programmee avec les pieds\n");
        else {
          snprintf(download_info, sizeof download_info, "downloading HFR smiley %s", q->msg);
          pp_set_download_info(NULL, download_info);
          pp_totoz_get_image(dock, q->msg);
        }
      } break;
      }
      if (flag_cancel_task) {
	myprintf("you interrupted %<yel %s>\n", ccqueue_elt_type_2_str(q->what));
      }
      ccqueue_pop(q);
      queue.state = -1;
      pp_set_download_info(NULL,NULL); /* pour les cas où ça a été mal remis à zero */
    }

    if (Prefs.ew_do_spell) ispell_run_background(Prefs.ew_spell_cmd, Prefs.ew_spell_dict);
    
    /* sauvegarde auto de la pos/dim du pinni & newswin */
    if (save_state_cnt == ((1000/WMCC_TIMER_DELAY_MS)*60*10)) {
      save_state_cnt = 0;
      wmcc_save_or_restore_state(dock, 0); 
    }
    //    dock->wmcc_state_info = WMCC_IDLE;
    ALLOW_X_LOOP;
#if (defined(__CYGWIN__) || defined(NOSIGNALS))
    usleep(10000); 
#else
    pause(); 
#endif
    /* ramasse zombie */
    {
      pid_t pid;
      int status;
      if ((pid = waitpid(0, &status, WNOHANG))) {
	if (pid > 1 && WIFEXITED(status)) {
	  BLAHBLAH(1,myfprintf(stderr, "fiston n° %u vient de mourir, son dernier mot a été %d\n", pid, WEXITSTATUS(status)));
	}
	if (pid == dock->wmccc_pid && dock->wmccc_pid > 1) {
	  BLAHBLAH(1,myfprintf(stderr, "wmccc RIP\n"));
	  dock->wmccc_pid = -1;
	}
      }
    }
    ALLOW_X_LOOP;
    temps_depuis_dernier_event++;
    save_state_cnt++;
  }
}
