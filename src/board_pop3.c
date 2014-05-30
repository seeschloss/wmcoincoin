#include "board_priv.h"
#include "base64.h"

int pop3_ok(const char *s) {
  if (s == NULL || strlen(s) < 3 || strncmp(s, "+OK", 3)) return 0;
  else return 1;
}

int pop3_end_multiline(char *s) {
  str_rtrim_lf(s);
  if (strlen(s) == 1 && s[0] == '.') return 1;
  else return 0;
}

int
pop3_wait_ok_line(TelnetSession *ts) {
  char line[512];
  if (ts->error) return 0;
  telnet_get_line(ts, line, 512);
  if (!ts->error) {
    return pop3_ok(line);
  }
  return 0;
}

static void unhtml_body(char *txt) {
  char *src, *dest, in_tag = 0;
  for (src = txt, dest = txt; *src; ++src) {
    if (*src == '<' && !in_tag) in_tag++;
    else if (*src == '>' && in_tag != 0) in_tag--;
    else if (!in_tag) { *dest++ = *src; }
  }
  *dest=0;
}

char* copy_text_between_angles(const char *src, char *dest, size_t dest_sz) {
  char *a, *b;
  size_t n;
  a = strchr(src, '<'); if (!a) return NULL;
  b = strchr(a, '>'); if (!b) return NULL;
  n = MIN(dest_sz, (size_t)(b-a+1));
  memmove(dest, a, dest_sz);
  if ((size_t)(b-a+1) < dest_sz) dest[b-a+1] = 0;
  else dest[dest_sz-1] = 0;
  return b;
}

/* ref: rfc1521 rfc1522
   dequoteur à coulisse
*/
void dequote_printable(char *s, int is_quoted_printable) {
  int rfc1522_section = 0;
  char *d = s, *p;
  while (*s) {
    if (s[0] == '=' && s[1] == '?') {
      p = strchr(s+2,'?');
      if (p && p[1] == 'Q') { /* pour l'instant le B-encoding c'est dtc, base64 ça fait chier */
        rfc1522_section = 1; s = p+3;
      }
    }
    if (rfc1522_section && s[0] == '?' && s[1] == '=') {
      rfc1522_section = 0; s+=2;
    }
    if (*s != '=' || !(is_quoted_printable || rfc1522_section)) {
      *d++ = *s++;
    } else {
      unsigned code;
      s++;
      if (*s > ' ' && sscanf(s, "%02X", &code)==1 && code < 256) {
        *d++ = MAX(' ',code);
        s+=2;
      } else if (*s <= ' ') s++;
    }
  }
  *d = 0;
}

int
pop3_check_message(Board *board, TelnetSession *ts, int msgnum, const char *uidl) {
  Boards *boards = board->boards;
  int was_already_viewed = 0;
  md5_byte_t md5[16];
  md5_digest(uidl,md5);
  /* cherche le news dans le cache (au premier dl uniquement) */
  if (board->oldmd5) {
    md5_and_time *m = find_md5_in_md5_array(md5,board->oldmd5);
    if (m && strlen(m->tstamp) == 14) {
      was_already_viewed = m->viewed;
      BLAHBLAH(1, myprintf("the mail '%<GRN %s>' was found in the cache!\n", uidl));
    }
  }
  /* cherche dans la liste des news dejà lues (après le premier dl) */
  if (board_find_md5(board, md5)) {
    BLAHBLAH(1,myprintf("the mail %<MAG %s>/%<CYA %s> is already known\n", ts->host, uidl));
  } else {
    char ua[100];
    char subject[100];
    char body[4000];
    char sender[100];
    char message_id[200];
    char refmsg[500];
    char mime_boundary[100];
    char s[40]; 
    int in_body = 0, html_junk=0, is_quoted_printable = 0, is_base64 = 0, body_line_count = 0;
    time_t timestamp = time(NULL);
    ua[0] = 0; subject[0] = 0; body[0] = 0; sender[0] = 0; mime_boundary[0] = 0; 
    message_id[0] = 0; refmsg[0] = 0;
    snprintf(s, 40, "TOP %d 40\r\n", msgnum);
    BLAHBLAH(1,myprintf("sending %s", s));
    telnet_send(ts, s);
    if (!pop3_wait_ok_line(ts)) return -1;
    do {
      char line[512];
      telnet_get_line(ts, line, 512); str_rtrim_lf(line);
      if (ts->error) return -2;
      if (line[0] == '.') {
        if (strlen(line) == 1) { 
          break;
        } else memmove(line, line+1, strlen(line)+1);
      }
      if (str_startswith(line, "User-Agent:")) {
        strncpy(ua, line+11, 100); ua[99] = 0; str_trim(ua); 
      }
      if (str_startswith(line, "X-Mailer:")) {
        strncpy(ua, line+9, 100); ua[99] = 0; str_trim(ua); 
      }
      if (str_startswith(line, "Return-Path:")) {
        strncpy(sender, line+12, 100); sender[99] = 0; str_trim(sender); 
      }
      if (str_startswith(line, "From:")) {
        strncpy(sender, line+6, 100); sender[99] = 0; str_trim(sender); 
      }
      if (str_startswith(line, "Subject:")) {
        strncpy(subject, line+8, 100); subject[99] = 0; str_trim(subject);         
      }
      if (str_startswith(line, "Date:")) {
        if (str_to_time_t(line+5, &timestamp)) {
          char stimestamp[15];
          time_t_to_tstamp(timestamp, stimestamp);
          BLAHBLAH(1,myprintf("got time! : %<CYA %s> -> %<YEL %ld> -> %<MAG %s>\n", line, (long)timestamp, stimestamp));
          if (timestamp > time(NULL)) timestamp = time(NULL);
        }
        else printf("could not convert '%s' to a date\n", line+5);
      }
      if (!in_body && strstr(line, "boundary=")) {
        char *sbound = strstr(line, "boundary=");
        if (sbound) { 
          str_rtrim_lf(line);
          char *tmp = str_dup_unquoted(sbound+9);
          strncpy(mime_boundary, tmp, 100); mime_boundary[99] = 0; 
          BLAHBLAH(1,printf("looking mime-boundary: '%s'\n", mime_boundary)); 
          free(tmp);
        }
      } else if (strlen(mime_boundary) && strstr(line, mime_boundary)) {
        BLAHBLAH(1,printf("found mime-boundary %s\n", mime_boundary));
        mime_boundary[0] = 0;
      }
      if (!in_body && str_startswith(line, "Message-Id:")) {
        if (copy_text_between_angles(line, message_id, sizeof message_id)) {
          BLAHBLAH(1,myprintf("got message_id : %<grn %s>\n", message_id));
        }
      }
      if (!in_body && (str_startswith(line, "In-Reply-To:") || 
                       str_startswith(line, "References:"))) {
        char in_reply_to[200] ,*p = line;
        BLAHBLAH(1,myprintf("got reply : %<GRN %s>", in_reply_to));
        while ((p=copy_text_between_angles(p, in_reply_to, sizeof in_reply_to))) {
          RSSBonusInfo *bi = rss_find_from_link(boards, in_reply_to);
          if (bi) {
            board_msg_info *ref_mi = boards_find_id(boards, bi->id);
            if (ref_mi) {
              char reftmp[100];
              snprintf(reftmp, 100, 
                       "%02d:%02d:%02d ", ref_mi->hmsf[0], ref_mi->hmsf[1], ref_mi->hmsf[2]);              
              BLAHBLAH(1,printf(" FOUND!!! : %s (id=%d)", refmsg, id_type_lid(ref_mi->id)));
              if (!strstr(refmsg,reftmp) && strlen(refmsg)+strlen(reftmp)+1 < sizeof refmsg) {
                strcat(refmsg, reftmp);
              }
            }
          }
          BLAHBLAH(1,printf("\n")); 
        }
      }
      if (!in_body && str_startswith(line, "Content-Transfer-Encoding: quoted-printable")) {
        is_quoted_printable = 1;
      }
      if (!in_body && str_startswith(line, "Content-Transfer-Encoding: base64")) {
        is_base64 = 1;
      }
      
      if (!in_body && strstr(line, "text/html")) {
        html_junk = 1;
      }
      if (strlen(line) == 0 && mime_boundary[0] == 0) in_body = 1;
      else if (in_body) {
        str_trim(line);
        if (strlen(line)) body_line_count++;
        if ((str_startswith(line, ">") || str_startswith(line, "|")) &&
            body_line_count == 2 && strrchr(body, ':')) { /* detecte les "On Tue, Mar 09, 2004 at 02:29:43PM -0800, plop wrote:" */
          body[0] = 0;
        }
        if (!str_startswith(line, ">") && !str_startswith(line, "|")) {
          strncat(body, " ", 3999 - strlen(body)); body[3999] = 0;
          strncat(body, line, 3999 - strlen(body)); body[3999] = 0;
        }
      }
    } while (1);
    if (strlen(sender) == 0) strcpy(sender, "(unknown)");    

    dequote_printable(body, is_quoted_printable);
    if (is_base64) {
      BLAHBLAH(1,myprintf("%<yel DECODING:> '%<CYA %s>'\n", body));
      from64tobits(body,body,0); //base64_decode(body);
    }
    if (html_junk) unhtml_body(body);
    
    str_trunc_nice(body, 512);
    { int i; for (i = 0; body[i]; ++i) if (body[i] > 0 && body[i] < ' ') body[i] = ' '; }
    dequote_printable(subject, 0);
    dequote_printable(sender, 0);

    BLAHBLAH(1,myprintf("fin, tout est ok: subject=%<YEL %s>\nbody=%s\nua=%s\nsender=%<yel %s>\nmessageid=%s\nreply_to=%s\n\n\n", subject, body, ua, sender, message_id, refmsg));
    {
      char msgd[BOARD_MSG_MAX_LEN];
      char *subject2, *body2, /**ua2,*/ /**sender2,*/ *msg = NULL;
      static const char *htmlkeys[] = { "<", ">", "&", "\"" };
      static const char *htmlsubs[] = { "&lt;", "&gt;", "&amp;", "&quot;" };
      subject2 = str_multi_substitute(subject, htmlkeys, htmlsubs, 4);
      body2 = str_multi_substitute(body, htmlkeys, htmlsubs, 4);
      //ua2 = str_multi_substitute(ua, htmlkeys, htmlsubs, 4);
      //sender2 = str_multi_substitute(sender, htmlkeys, htmlsubs, 4);

      msg = str_printf("%s<u><b>%s</b></u> -- %s", refmsg, subject2, body2);
      board_decode_message(board, msgd, msg);

      prelog_add(ua,sender,timestamp,msgd,message_id[0] ? message_id : NULL, md5, was_already_viewed);
      board->nb_msg_at_last_check++;
      if (!was_already_viewed) board->nb_msg_since_last_viewed++;
      free(msg); /*free(ua2);*/ free(body2); free(subject2); /*free(sender2);*/
    }
  }
  return 0;
}

int pop3_board_update(Board *board, char *path) {
  TelnetSession ts; 
  SplittedURL su;
  char greeting[512],line[512];
  char *user, *pass;
  int nb_msg = 0, mailbox_size = 0, msgcnt;
  char **uidls = NULL;
  int use_normal_auth = 1;
  BLAHBLAH(1,printf("update de %s\n",path));

  prelog_clear();

  user = board->site->prefs->pop3_user;
  pass = board->site->prefs->pop3_pass;
  if (str_is_empty(user) || str_is_empty(pass)) {
    myprintf("%<RED I can't log you into %s, stupid !> no login/pass known for this site (fill .wmcoincoin/options.auth)\n", path);
    return 1;
  };

  telnet_session_init(&ts);
  if (split_url(path,&su)) assert(0);
  assert(su.type == POP3_URL || su.type == APOP_URL);
  ts.host = strdup(su.host);
  ts.port = su.port;
  telnet_session_open(&ts);
  if (ts.error) goto err;
  telnet_get_line(&ts, greeting, 512);
  BLAHBLAH(1,myprintf("got greeting from %s : '%<YEL %s>'\n", path, greeting));
  if (ts.error || !pop3_ok(greeting)) goto err;
  if (0 && /* APOP ne marche pas sur free :-/ pas pigé pourquoi, le digest a pourtant l'air bon */
      copy_text_between_angles(greeting, greeting, sizeof(greeting))) {
    char *msgid, *apop_cmd = NULL;
    md5_byte_t md5[16]; 
    BLAHBLAH(1,printf("trying APOP\n"));
    msgid = str_printf("%s%s", greeting, pass);
    memset(md5,0,16);
    md5_digest(msgid, md5);
    apop_cmd = str_printf("APOP %s %s\r\n", user, md5txt(md5));
    BLAHBLAH(1,printf("sending %s", apop_cmd));
    telnet_send(&ts, apop_cmd);
    if (pop3_wait_ok_line(&ts)) use_normal_auth = 0;
    free(apop_cmd);
    free(msgid);
  }
  if (use_normal_auth) {
    BLAHBLAH(1,myprintf("sending USER %s\n", user));
    telnet_send(&ts, "USER "); telnet_send(&ts, user); telnet_send(&ts, "\r\n");
    if (!pop3_wait_ok_line(&ts)) goto err;
    BLAHBLAH(1,myprintf("sending PASS ..\n", pass));
    telnet_send(&ts, "PASS "); telnet_send(&ts, pass); telnet_send(&ts, "\r\n");
    if (!pop3_wait_ok_line(&ts)) goto err;
  }
  telnet_send(&ts, "STAT\r\n");
  telnet_get_line(&ts, line, 512);
  if (ts.error || !pop3_ok(line)) goto err;
  if (sscanf(line, "+OK %d %d", &nb_msg, &mailbox_size) != 2) goto err;
  telnet_send(&ts, "UIDL\r\n");
  if (!pop3_wait_ok_line(&ts)) goto err;
  ALLOC_VEC(uidls, nb_msg, char *);
  for (msgcnt = 0; msgcnt < nb_msg; ++msgcnt) {
    char uidl[500];
    int n;
    telnet_get_line(&ts, line, 512);
    if (ts.error) goto err;
    if (sscanf(line, "%d %500s", &n, uidl) != 2 || n != msgcnt+1) goto err;
    uidls[msgcnt] = strdup(uidl);
    //printf("got uidl=%s\n", uidl);
  }
  telnet_get_line(&ts, line, 512);
  if (!pop3_end_multiline(line)) goto err;
  for (msgcnt = 0; msgcnt < nb_msg; ++msgcnt) {
    if (pop3_check_message(board, &ts, msgcnt+1, uidls[msgcnt]) != 0) {
      printf("error during d/l of %s ..\n", uidls[msgcnt]); goto err;
    }
  }

  BLAHBLAH(1,myprintf("sending QUIT\n"));
  telnet_send(&ts, "QUIT\r\n");
  if (uidls) { int i; for (i = 0; i < nb_msg; ++i) FREE_STRING(uidls[i]); free(uidls); }
  telnet_session_close(&ts);
  prelog_commit(board);
  return 0;
 err:
  telnet_session_close(&ts);
  if (uidls) { int i; for (i = 0; i < nb_msg; ++i) FREE_STRING(uidls[i]); free(uidls); }
  prelog_commit(board);
  return 1;
}
