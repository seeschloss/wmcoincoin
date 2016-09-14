#include "board_priv.h"

/* pinaise j'en ai marre de faire ce genre de fonctions */
char* rss_nettoie_la_soupe_de_tags(char *src) {
  strbuf sb; strbuf_init(&sb, "");
  char *p, *p2;
  while (src[0]) {
    int skip;
    skip = 0;
    if (*src == '\t') {
      p = src+1;
      if (*p == '<') {
        p++;
        while (*p && (isspace(*p) || *p == '/')) ++p;
        p2 = p;
        while (*p2 && !isspace(*p2) && *p2 != '/' && *p2 != '>') ++p2;
        if (*p2) {
          if ((strncasecmp(p, "br", 2)==0 && p2-p == 2) ||
              (strncasecmp(p, "p", 1)==0 && p2-p == 1) ||
              (strncasecmp(p, "div", 3)==0 && p2-p == 3) ||
              (strncasecmp(p, "table", 5)==0 && p2-p == 5) ||
              (strncasecmp(p, "tr", 2)==0 && p2-p == 2) ||
              (strncasecmp(p, "td", 2)==0 && p2-p == 2)) {
            skip = 1;
          } else if (strncasecmp(p, "img", 3)==0 && p2 - p == 3) {            
            char *ps, *pe;
            //printf("img tag here : %.50s\n", p);
            ps = str_case_str(p, "src=");
            pe = strstr(p, "\t>");
            if (ps && pe && ps < pe) {
              ps += 4;
              while (*ps && (isspace(*ps) || *ps == '"' || *ps == '\'')) ++ps;
              if (*ps) {                
                strbuf_cat(&sb, "\t<a href=\"./");
                while (*ps && *ps != '"' && *ps != '\'' && *ps != '\t' && !isspace(*ps)) 
                  strbuf_putc(&sb,*ps++);
                strbuf_cat(&sb, "\"\t><img>\t</a\t>");
                //printf("sbuff = %s\n", sb.str);
              }
            }
            ++p; skip = 1;
          }
        }
      }
      if (skip) {
        p = strchr(src+1,'\t'); 
        if (p) { src = p+1; if (*src) src++; strbuf_putc(&sb, ' '); }
        else { return sb.str; }
      }
    }
    if (!skip) strbuf_putc(&sb, *src++);
  }
  return sb.str;
  //*dest = 0;
}


RSSBonusInfo *rss_register(Boards *boards, md5_byte_t mimd5[16], char *link, board_msg_info *mi) {
  RSSBonusInfo *ri;
  int i;
  ALLOC_OBJ(ri, RSSBonusInfo); 
  ri->link = NULL;
  ri->id = mi->id;

  if (link && strlen(link)) {
    ri->link = strdup(link);
  } else { 
    ri->link = strdup("(none)");
  }
  md5_digest(ri->link,ri->linkmd5);

  //printf("reghister = '%s', md5 = %02x%02x..%02x\n", link, ri->linkmd5[0], ri->linkmd5[1], ri->linkmd5[15]);

  memcpy(ri->md5,mimd5,16);
  if (boards->nb_rss_e == boards->max_rss_e) {
    boards->max_rss_e *= 2; 
    boards->rss_e = realloc(boards->rss_e, sizeof(RSSBonusInfo*)*boards->max_rss_e); 
    assert(boards->rss_e);
  }
  boards->rss_e[boards->nb_rss_e++] = ri;
  for (i = boards->nb_rss_e - 1; i; --i) {
    if (memcmp(boards->rss_e[i-1]->linkmd5, boards->rss_e[i]->linkmd5, 16) > 0) {
      RSSBonusInfo *tmp = boards->rss_e[i];
      boards->rss_e[i] = boards->rss_e[i-1]; 
      boards->rss_e[i-1] = tmp;
    } else break;
  }
  /*for (i=0; i < boards->nb_rss_e; ++i) {
    printf(" rss %d/%d : %02x%02x..%02x\n", i, boards->nb_rss_e, boards->rss_e[i]->linkmd5[0], boards->rss_e[i]->linkmd5[1], boards->rss_e[i]->linkmd5[15]);
    }*/
  return ri;
}

RSSBonusInfo *
rss_find_from_link(Boards *boards, char *link) {
  int i0, i1;
  md5_byte_t md5[16];
  md5_digest(link,md5);
  //printf("link = '%s', md5 = %02x%02x..%02x\n", link, md5[0], md5[1], md5[15]);
  i0 = 0; i1 = boards->nb_rss_e-1;
  while (i1>=i0) {
    int i = (i0 + i1)/2;
    //printf("i0=%d, i1=%d, compare with %02x%02x..%02x\n", i0, i1, boards->rss_e[i]->linkmd5[0], boards->rss_e[i]->linkmd5[1], boards->rss_e[i]->linkmd5[15]);
    int cmp = memcmp(boards->rss_e[i]->linkmd5, md5, 16);
    if (i1 <= i0 && cmp) return NULL;        
    if (cmp < 0) {
      i0 = i+1;
    } else if (cmp > 0) {
      i1 = i-1;
    } else return boards->rss_e[i];
  }
  return NULL;
}

RSSBonusInfo *
rss_find_from_id(Boards *boards, id_type id) {
  int i;
  for (i=0; i < boards->nb_rss_e; ++i) {
    if (id_type_eq(boards->rss_e[i]->id, id)) return boards->rss_e[i];
  }
  return NULL;
}

/*
  ce qui suit sert à stocker temporairement les nouveau messages et a les 
  trier en fonction de la date (il ne sont jamais dans le bon ordre sinon, et en 
  plus ils n'ont pas d'id)
  quand ils sont tous connus, on peut leur filer un id et les logger pour de bon
*/
typedef struct prelog_msg {
  char *ua, *login, *msg;
  time_t tstamp;
  char *link;
  int viewed;
  md5_byte_t md5[16];  
  struct prelog_msg *next;
} prelog_msg;

static struct prelog_msg *prelog = NULL;

void
prelog_clear() {
  prelog_msg *pl = prelog, *n;
  while (pl) {
    n = pl->next;
    FREE_STRING(pl->ua); FREE_STRING(pl->login); FREE_STRING(pl->msg); FREE_STRING(pl->link);
    free(pl); pl = n;
  }
  prelog = NULL;
}

void prelog_add(char *ua, char *login, time_t tstamp, char *message, char *link, md5_byte_t md5[16], int viewed) {
  prelog_msg *pl, *p, *pp;
  ALLOC_OBJ(pl, prelog_msg);
  pl->ua = strdup(ua); pl->login = strdup(login); 
  pl->msg = strdup(message); 
  pl->link = link ? strdup(link) : NULL;
  pl->tstamp = tstamp;
  pl->viewed = viewed;
  memcpy(pl->md5, md5, 16);

  pp = NULL; p = prelog;
  while (p && p->tstamp < tstamp) {
    pp = p;
    p = p->next;
  }
  if (pp) {
    pl->next = pp->next;
    pp->next = pl;
  } else {  
    pl->next = prelog;
    prelog = pl;
  }
}

void prelog_commit(Board *board) {
  prelog_msg *pl = prelog;
  board_msg_info *mi;
  id_type id;
  int count = board->last_post_id+1;
  char stimestamp[15];
  while (pl) {
    id_type_set_lid(&id, count);
    id_type_set_sid(&id, board->site->site_id);
    assert(board_find_id(board,id_type_lid(id)) == NULL);
    time_t_to_tstamp(pl->tstamp, stimestamp);
    flag_updating_board++;
    BLAHBLAH(4,printf("prelog_commit(%s) %p id=%d, ts=%s, login=%s\n", board->site->prefs->site_name, pl, count, stimestamp, pl->login));
    mi = board_log_msg(board, pl->ua, pl->login, stimestamp, pl->msg, id_type_lid(id), NULL);
    mi->ri = rss_register(board->boards, pl->md5, pl->link, mi);
    flag_updating_board--;
    if (pl->viewed) board_set_viewed(board,count); //->last_viewed_id = MAX(board->last_viewed_id, count);
    pl = pl->next; ++count;
  }
  prelog_clear();
}

int
rss_board_update(Board *board, char *path) {
  HttpRequest r;
  //int http_err_flag = 0;
  //char *errmsg = NULL;
  char *p;
  char *rss_title = NULL;
  XMLBlock xmlb;
  int pos, refresh_request = -1, cest_bon_je_connais_la_suite = 0;
  time_t temps_debut = time(NULL), temps_last_modified;

  prelog_clear();
  clear_XMLBlock(&xmlb);
  wmcc_init_http_request(&r, board->site->prefs, path);
  /* Triton>    Tant qu'a faire de mettre un header "Accept:", autant le mettre partout
                Hooo, c'est cool, y'en a un prevu pour les flux rss au lieu d'un bete
                text/xml generique et banal [:freekill]
     SeeSchloß> ouais ouais sauf qu'il y a plein de serveurs de merde qui ne comprennent
                pas ce type, alors non [:benou] */
  //r.accept = strdup("application/rss+xml");
  if (board->site->prefs->use_if_modified_since) { r.p_last_modified = &board->last_modified; }
  http_request_send(&r);
  if (!http_is_ok(&r)) {
    http_request_close(&r);
    goto ratai;
  }
  wmcc_log_http_request(board->site, &r);

  if (r.response_size <= 0) {
    http_request_close(&r);
    goto RAS; /* certainement du not modified */
  }

  char *rsstxt = malloc(r.response_size);
  memcpy(rsstxt, r.response_data, r.response_size);
  http_request_close(&r);

  if (!rsstxt) goto RAS; /* "not modified" */
  
  if (strlen(rsstxt)==0) goto RAS;

  /* tentative de conversion vers utf8 */
  if ((pos = get_XMLBlock(rsstxt, strlen(rsstxt), "?xml", &xmlb))>=0) {
    XMLAttr *a;
    int found = 0;
    if (board->encoding) { free(board->encoding); board->encoding = NULL; }
    for (a = xmlb.attr; a; a = a->next) {
      if (str_case_startswith(a->name, "encoding")) {
        board->encoding = str_ndup(a->value,a->value_len);
        BLAHBLAH(1,printf("%s: found encoding: value = '%s'\n", board->site->prefs->site_name, board->encoding));
        found = 1;
        break;
      }
    }
    if (!found) board->encoding = strdup("UTF-8"); /* defaut si pas d'encoding specifie */
    convert_to_utf8(board->encoding, &rsstxt);
  }

  pos = get_XMLBlock(rsstxt, strlen(rsstxt), "title", &xmlb);
  if (pos < 0 || xmlb.content_len == 0) goto ratai;
  /*if (board->rss_title) free(board->rss_title);
    board->rss_title = str_ndup(xmlb.content, xmlb.content_len);*/
  rss_title = str_ndup(xmlb.content, xmlb.content_len);
  BLAHBLAH(1, myprintf("got TITLE: '%<YEL %s>'\n", rss_title));

  if (board->rss_title) {
    free(board->rss_title);
  }
  board->rss_title = str_ndup(rss_title, 100);

  if (get_XMLBlock(rsstxt, strlen(rsstxt), "ttl", &xmlb) >= 0) {
    refresh_request = atoi(xmlb.content) * 60; /* en minutes */
    //printf("ttl detected, %d\n", refresh_request);
  } if (get_XMLBlock(rsstxt, strlen(rsstxt), "*:updatePeriod", &xmlb) >= 0) {
    int period = 1;
    if (str_case_startswith(xmlb.content, "hour")) period = 3600;
    else if (str_case_startswith(xmlb.content, "min")) period = 60;
    if (get_XMLBlock(rsstxt, strlen(rsstxt), "*:updateFrequency", &xmlb) >= 0) {
      refresh_request = period * atoi(xmlb.content);
    }
  }
  if (refresh_request != -1 && board->site->prefs->board_check_delay < refresh_request) {
    BLAHBLAH(0, myprintf("Changing update frequency for %<grn %s> to %<MAG %d> sec.\n", rss_title, refresh_request));
    board->site->prefs->board_check_delay = refresh_request;
  }

  p = rsstxt;

  temps_last_modified = temps_debut;
  if (board->last_modified) {
    str_to_time_t(board->last_modified, &temps_last_modified);
    //printf("last_modified='%s' -> time_t = %ld\n", board->last_modified, temps_last_modified);
  }
  do {
    int pos_next_item;
    pos_next_item = get_XMLBlock(p, strlen(p), "item", &xmlb);
    if (pos_next_item < 0) {
        BLAHBLAH(1, myprintf("couldn't find <item>, looking for <entry>\n"));
        pos_next_item = get_XMLBlock(p, strlen(p), "entry", &xmlb);
    }

    if (pos_next_item >= 0) {
      BLAHBLAH(1, myprintf("found something, either item or entry\n"));
      XMLBlock b2;
      char *title, *link, *description, *msg, *author, *comments_url, *pubdate, *fake_ua;
      char msgd[BOARD_MSG_MAX_LEN];
      char stimestamp[15];
      time_t timestamp = time(NULL);
      title = link = description = msg = author = comments_url = pubdate = fake_ua = NULL;

      //time_t_to_tstamp(temps_debut, stimestamp); 

      //temps_debut--; /* pour eviter d'avoir un paquet de news avec le meme tstamp */
      clear_XMLBlock(&b2);
      if (get_XMLBlock(xmlb.content, xmlb.content_len, "title", &b2) &&  b2.content_len) {
        title = str_ndup(b2.content, b2.content_len);
        BLAHBLAH(1, myprintf("found title: '%s'\n", title));
      }
      if (get_XMLBlock(xmlb.content, xmlb.content_len, "link", &b2)) {
        if (b2.content_len) {
          link = str_ndup(b2.content, b2.content_len);
          BLAHBLAH(1, myprintf("found link: '%s'\n", link));
        } else {
          XMLAttr *a;
          for (a = b2.attr; a; a = a->next) {
            if (str_case_startswith(a->name, "href")) {
              link = str_ndup(a->value, a->value_len);
              BLAHBLAH(1,printf("found link: value = '%s'\n", link));
              break;
            }
          }
        }
      }
      if (!board->site->prefs->rss_ignore_description) {
        if (get_XMLBlock(xmlb.content, xmlb.content_len, "description", &b2) &&  b2.content_len) {
          description = str_ndup(b2.content, b2.content_len);
        } else if (get_XMLBlock(xmlb.content, xmlb.content_len, "content", &b2) &&  b2.content_len) {
          description = str_ndup(b2.content, b2.content_len);
        } else if (get_XMLBlock(xmlb.content, xmlb.content_len, "summary", &b2) &&  b2.content_len) {
          description = str_ndup(b2.content, b2.content_len);
        }
      }
      if (get_XMLBlock(xmlb.content, xmlb.content_len, "author", &b2) &&  b2.content_len) {
        author = str_ndup(b2.content, b2.content_len);
        //printf("found author: '%s'\n", author);
      }
      if (get_XMLBlock(xmlb.content, xmlb.content_len, "name", &b2) &&  b2.content_len) {
        author = str_ndup(b2.content, b2.content_len);
        //printf("found author: '%s'\n", author);
      }
      if (get_XMLBlock(xmlb.content, xmlb.content_len, "comments", &b2) &&  b2.content_len) {
        comments_url = str_ndup(b2.content, b2.content_len);
      }
      /* format date: http://www.w3.org/TR/NOTE-datetime */
      if (get_XMLBlock(xmlb.content, xmlb.content_len, "pubDate", &b2) &&  b2.content_len) {
        pubdate = str_ndup(b2.content, b2.content_len);
      }
      if (get_XMLBlock(xmlb.content, xmlb.content_len, "published", &b2) &&  b2.content_len) {
        pubdate = str_ndup(b2.content, b2.content_len);
      }
      if (pubdate == NULL && get_XMLBlock(xmlb.content, xmlb.content_len, "*:date", &b2) &&  b2.content_len) {
        pubdate = str_ndup(b2.content, b2.content_len);        
      }

      /* une petite remarque pour poser la problematique calmement:
         Comment determiner raisonnablement la date de publication d'une news
          - <pubDate>date_format_rfc_822</pubDate>

          - <dc:date>date_iso_8601</dc:date>

          - sinon :
            . si la news était connue par wmcc lors de sa precedente execution,
            on reprend la meme date sans paniquer.

            . sinon, on prend l'heure courante.
               * si  le serveur web a renvoye un last-modified, on prend cette valeur.

         Pour un fun toujours plus extreme, il faut bien gérer tous les
         problemes de timezone:
           PUTAIN DE BORDERL DE MARDE
      */

      /* c'est trop la merde avec les decalages horaires.. */
      if (pubdate) {
              BLAHBLAH(1,printf("found date: value = '%s'\n", pubdate));
        if (str_to_time_t(pubdate, &timestamp)) {
          time_t_to_tstamp(timestamp, stimestamp);
          BLAHBLAH(1,myprintf("converted %<YEL %s> to %<YEL %s> !\n", pubdate, stimestamp));
        } else {
          BLAHBLAH(1,myprintf("could not convert '%s' to a valid date..\n", pubdate));
        }
      }

      timestamp = MIN(timestamp, temps_debut);
      timestamp = MIN(timestamp, temps_last_modified);
      time_t_to_tstamp(timestamp, stimestamp);

      destroy_XMLBlock(&b2);
      
      str_trunc_nice(description, 512);
      if (link) {
        char *p = strstr(link, "*http://"); // enleve une couche de merde dans les liens yahoo
        if (p) { p++; memmove(link, p, strlen(p)+1); }
      }

      msg = NULL;
      if (title && link) {
        msg = str_cat_printf(msg, "{<a href=\"%s\"><b>%s</b></a>}", link, title);
        BLAHBLAH(1, myprintf("formatting title with link\n"));
      } else if (title) {
        msg = str_cat_printf(msg, "{<b>%s</b>}", title);
        BLAHBLAH(1, myprintf("formatting title without link\n"));
      } else if (link) {
        msg = str_cat_printf(msg, "{<a href=\"%s\">[News]</a>}", link);
        BLAHBLAH(1, myprintf("formatting link without title\n"));
      }

      if (description) {
        msg = str_cat_printf(msg, " %s", description);
      }

      if (comments_url) {
        msg = str_cat_printf(msg, " <a href=\"%s\">[comments]</a>", comments_url);
      }

      if (msg) {
        md5_byte_t md5[16];
        md5_state_t ms; md5_init(&ms);
        int was_already_viewed = 0;
        if (title) md5_append(&ms, title, strlen(title));
        if (link) md5_append(&ms, link, strlen(link));
        if (description) md5_append(&ms, description, strlen(description));
        md5_finish(&ms,md5);

        /* cherche le news dans le cache (au premier dl uniquement) */
        if (board->oldmd5) {
          md5_and_time *m = find_md5_in_md5_array(md5,board->oldmd5);
          if (m && strlen(m->tstamp) == 14) {
            was_already_viewed = m->viewed;
            strcpy(stimestamp, m->tstamp);
            str_to_time_t(stimestamp, &timestamp);
            BLAHBLAH(1, myprintf("the news '%<GRN %s>' was found in the cache!\n", title));
          }
        }

        /* cherche dans la liste des news dejà lues (après le premier dl) */
        if (board_find_md5(board, md5)) {
          BLAHBLAH(1,myprintf("the news %<MAG %s>/%<CYA %s> is already known\n", rss_title, md5txt(md5)));
          //cest_bon_je_connais_la_suite = 1; // si on suppose que les rss se remplissent toujours par le haut..
        } else {
          /* nettoyage des codes < 32 dans le message */
          {
            int i; 
            for (i=0; i < BOARD_MSG_MAX_LEN && msg[i]; ++i)
              if ((unsigned char)msg[i] < ' ') msg[i] = ' ';
          }          
          fake_ua = str_printf("%s", rss_title ? rss_title : "?");
          if (pubdate) { fake_ua = str_cat_printf(fake_ua, " pubDate: %s", pubdate); }

          /* attention, les '&lt;' deviennent '\t<' et les '&amp;lt;' devienne '<' */
          board_decode_message(board, msgd, msg);

          {
            char *soupe = rss_nettoie_la_soupe_de_tags(msgd);
            strncpy(msgd, soupe, sizeof msgd); free(soupe); msgd[(sizeof msgd) - 1] = 0;
          }
          if (author && strlen(author)) {
            author = str_cat_printf(author, "@%s", rss_title);
          } else {
            FREE_STRING(author); author = strdup(rss_title);
          }
          {
            char author_tmp[1024];
            convert_to_ascii(author_tmp, author, sizeof author_tmp);
            FREE_STRING(author); author = strdup(author_tmp);
          }
          prelog_add(fake_ua, author, timestamp, msgd, link, md5, was_already_viewed);
          board->nb_msg_at_last_check++;
          if (!was_already_viewed) board->nb_msg_since_last_viewed++;
        }
      }
      FREE_STRING(title); FREE_STRING(link); FREE_STRING(description); FREE_STRING(author); 
      FREE_STRING(comments_url); FREE_STRING(msg); FREE_STRING(pubdate); FREE_STRING(fake_ua);
    } else { 
      BLAHBLAH(1,printf("fin de '%s'\n", rss_title));
      break;
    }
    
    p += pos_next_item;
  } while (!cest_bon_je_connais_la_suite);

 RAS:
  if (board->oldmd5 && board->last_post_id > 0) release_md5_array(board);
  destroy_XMLBlock(&xmlb);
  FREE_STRING(rss_title);
  FREE_STRING(rsstxt); 
  prelog_commit(board);
  return 0;
 ratai:
  if (board->oldmd5 && board->last_post_id > 0) release_md5_array(board);
  destroy_XMLBlock(&xmlb);
  FREE_STRING(rss_title);
  FREE_STRING(rsstxt);
  prelog_commit(board);
  return 1;
}
