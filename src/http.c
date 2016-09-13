#ifdef __CYGWIN__
#  define __USE_W32_SOCKETS
#  include "windows.h"
#  include "winsock.h"
#  include <unistd.h>
#  define AI_NUMERICHOST 4
#  define __INSIDE_HTTP
#  include "global.h"
#  include "http.h"
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <signal.h>
#  include <time.h>
#  include "myprintf.h"
#  include "coin_util.h"

#else

#include "../config.h"
#  include <unistd.h>
#  include <fcntl.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <sys/time.h>
#  include <sys/stat.h>

#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <stdarg.h>
#  include <stdlib.h>
#  include <stdio.h>
#  include <string.h>
#  include <errno.h>

/* pour le waitpid */
#include <sys/types.h>
#include <sys/wait.h>

#  include "myprintf.h"
#  include "coincoin.h"
#  include "http.h"

#endif /* ifdef __CYGWIN */

#ifdef __CYGWIN__
#  define LASTERR_EINTR (WSAGetLastError() == WSAEINTR)
#  define LASTERR_EAGAIN (WSAGetLastError() == WSAEINPROGRESS)
#  define SETERR_TIMEOUT WSASetLastError(WSAETIMEDOUT)
#  define STR_LAST_ERROR (flag_cancel_task ? "donwload canceled" : strerror(WSAGetLastError()))
#  define GAI_STRERROR(x) strerror(WSAGetLastError())
#  define LASTERR_ESUCCESS (WSAGetLastError() == 0) /* à tester ... */
#else
#  define LASTERR_EINTR (errno==EINTR)
#  define SETERR_TIMEOUT errno=ETIMEDOUT
#  define STR_LAST_ERROR (flag_cancel_task ? "donwload canceled" : strerror(errno))
#  define GAI_STRERROR(x) gai_strerror(x)
#  define LASTERR_EAGAIN (errno==EAGAIN)
#  define LASTERR_ESUCCESS (errno==0)
#endif

#include <libintl.h>
#define _(String) gettext (String)

#define HTTP_ERR_MSG_SZ 512
#define HTTP_LAST_ERR_URL_SZ 128

static char http_last_err_msg[HTTP_ERR_MSG_SZ] = "";
static char http_last_err_url[HTTP_LAST_ERR_URL_SZ] = "";
time_t http_err_time = 0;
static char http_last_url[HTTP_LAST_ERR_URL_SZ] = "";
static char http_used_ip[100] = "xxx.xxx.xxx.xxx";

typedef struct _HostEntry {
  char *host_name;
  char *numeric_host; /* resolved host name, if more than one answer, the entries are separated by a '|' */
  int port;
  struct _HostEntry *next;
} HostEntry;


HostEntry *dns_cache = NULL;

int http_close(SOCKET fd);

/* pas de fioritures */
#include <ctype.h>
#include "inet_aton.h"
#include "inet_aton.c"

/* 
   renvoie une chaine mallocee contenant l'urlencodage de string 
   encore un fonction piquee dans curl
 */
char *
http_url_encode(const char *string, int use_plus)
{
   int alloc=strlen(string)+1;
   char *ns = malloc(alloc);
   unsigned char in;
   int newlen = alloc;
   int index=0;

   while(*string) {
      in = *string;
      if(' ' == in && use_plus)
	 ns[index++] = '+';
      else if(!(in >= 'a' && in <= 'z') &&
	      !(in >= 'A' && in <= 'Z') &&
	      !(in >= '0' && in <= '9')) {
	 /* encode it */
	 newlen += 2; /* the size grows with two, since this'll become a %XX */
	 if(newlen > alloc) {
	    alloc *= 2;
	    ns = realloc(ns, alloc);
	    if(!ns)
	       return NULL;
	 }
	 sprintf(&ns[index], "%%%02X", in);
	 index+=3;
      }
      else {
	 /* just copy this */
	 ns[index++]=in;
      }
      string++;
   }
   ns[index]=0; /* terminate it */
   return ns;
}


size_t
_http_request_send_curl_header_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ALLOW_X_LOOP; ALLOW_ISPELL; 

  size_t realsize = size * nmemb;
  HttpRequest *r = (HttpRequest *)userp;

  r->header_data = realloc(r->header_data, r->header_size + realsize + 1);
  if(r->header_data == NULL) {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(r->header_data[r->header_size]), contents, realsize);
  r->header_size += realsize;
  r->header_data[r->header_size] = 0;

  global_http_download_cnt += realsize;

  ALLOW_X_LOOP; ALLOW_ISPELL; 

  return realsize;
}

size_t
_http_request_send_curl_write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ALLOW_X_LOOP; ALLOW_ISPELL; 

  size_t realsize = size * nmemb;
  HttpRequest *r = (HttpRequest *)userp;

  r->response_data = realloc(r->response_data, r->response_size + realsize + 1);
  if(r->response_data == NULL) {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(r->response_data[r->response_size]), contents, realsize);
  r->response_size += realsize;
  r->response_data[r->response_size] = 0;

  global_http_download_cnt += realsize;

  ALLOW_X_LOOP; ALLOW_ISPELL; 

  return realsize;
}

void
_http_request_send_parse_headers(HttpRequest *r)
{
  char *line_end;
  char *line = r->header_data;

  while ((line_end = strchr(line, '\n'))) {
    if (r->p_last_modified) {
      if (strncmp(line, "Last-Modified: ", 15) == 0) {
        if (*r->p_last_modified) {
          free(*r->p_last_modified);
          *r->p_last_modified = NULL;
        }
        *r->p_last_modified = strndup(line + 15, line_end - line - 15);
        str_trim(*r->p_last_modified);
        BLAHBLAH(Prefs.verbosity_http, printf("last modified: %s\n", *r->p_last_modified));
      }
    }
    if (strncmp(line, "Transfer-Encoding: chunked", 26) == 0) {
      r->is_chunk_encoded = 1;
      r->chunk_num = -1;
    }
    if (strncmp(line, "Content-Length:", 15) == 0) {
      r->content_length = atoi(line+15);
      BLAHBLAH(Prefs.verbosity_http, printf("content length: %d\n", r->content_length));
    }
    if (strncmp(line, "X-Post-Id:", 10) == 0) {
      r->post_id = atoi(line+10);
      BLAHBLAH(Prefs.verbosity_http, printf("x-post-id: %d\n", r->post_id));
    }
    if (strncmp(line, "Content-Type:", 13) == 0) {
      r->content_type = strndup(line+13, line_end - line - 13);
      str_trim(r->content_type);
      BLAHBLAH(Prefs.verbosity_http, printf("content type: %s\n", r->content_type));
    }
    if (strncmp(line, "Set-Cookie:", 11) == 0) {
      /* Format: Set-Cookie: <name>=<value>[; <name>=<value>]...
                             [; expires=<date>][; domain=<domain_name>]
                             [; path=<some_path>][; secure][; httponly] */
      char *garbage;
      if ((garbage = strcasestr(line+11, "expires="))) {
          r->new_cookie = strndup(line+11, garbage - line - 11);
      } else if ((garbage = strcasestr(line+11, "domain="))) {
          r->new_cookie = strndup(line+11, garbage - line - 11);
      } else if ((garbage = strcasestr(line+11, "path"))) {
          r->new_cookie = strndup(line+11, garbage - line - 11);
      } else if ((garbage = strcasestr(line+11, "secure"))) {
          r->new_cookie = strndup(line+11, garbage - line - 11);
      } else if ((garbage = strcasestr(line+11, "httponly"))) {
          r->new_cookie = strndup(line+11, garbage - line - 11);
      } else {
          r->new_cookie = strdup(line+11);
      }

      BLAHBLAH(Prefs.verbosity_http,printf("new cookie: %s\n", r->new_cookie));
    }

    line = line_end + 1;
  }
}

void
http_request_send(HttpRequest *r)
{
  curl_global_init(CURL_GLOBAL_ALL);

  r->curl = curl_easy_init();
  if(r->curl) {
    curl_easy_setopt(r->curl, CURLOPT_URL, r->url);

    memcpy(http_last_url, r->url, strlen(r->url) + 1);

    if (r->type == HTTP_POST) {
      curl_easy_setopt(r->curl, CURLOPT_POSTFIELDS, r->post);
    }

    if (r->cookie) {
      curl_easy_setopt(r->curl, CURLOPT_COOKIE, r->cookie);
    }

    if (r->referer) {
      curl_easy_setopt(r->curl, CURLOPT_REFERER, r->referer);
    }

    if (r->user_agent) {
      curl_easy_setopt(r->curl, CURLOPT_USERAGENT, r->user_agent);
    }

    struct curl_slist *list = NULL;
    if (r->accept) {
      char *accept = malloc(8 + strlen(r->accept) + 1);
      snprintf(accept, 8 + strlen(r->accept) + 1, "Accept: %s", r->accept);
      list = curl_slist_append(list, accept);
      free(accept);
    }
    if (r->p_last_modified && *(r->p_last_modified) && r->use_if_modified_since) {
      char *last_modified = *r->p_last_modified;

      char *if_modified_since = malloc(19 + strlen(last_modified) + 1);
      snprintf(if_modified_since, 19 + strlen(last_modified) + 1, "If-Modified-Since: %s", last_modified);
      printf("if-plop: %s\n", if_modified_since);
      list = curl_slist_append(list, if_modified_since);
      free(if_modified_since);
    }
    if (r->pragma_nocache) {
      list = curl_slist_append(list, "Pragma: no-cache");
      list = curl_slist_append(list, "Cache-Control: no-cache");
    }
    curl_easy_setopt(r->curl, CURLOPT_HTTPHEADER, list);

    curl_easy_setopt(r->curl, CURLOPT_HEADERFUNCTION, _http_request_send_curl_header_callback);
    curl_easy_setopt(r->curl, CURLOPT_HEADERDATA, (void *)r);
    curl_easy_setopt(r->curl, CURLOPT_WRITEFUNCTION, _http_request_send_curl_write_callback);
    curl_easy_setopt(r->curl, CURLOPT_WRITEDATA, (void *)r);

    curl_easy_setopt(r->curl, CURLOPT_FAILONERROR, 1); /* pas la peine de récupérer des données pour une >= 400 */
    curl_easy_setopt(r->curl, CURLOPT_ERRORBUFFER, http_last_err_msg);

    curl_easy_setopt(r->curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(r->curl, CURLOPT_NOSIGNAL, 1); /* sinon ça interfère avec pause() */

    if (r->proxy_name) {
      curl_easy_setopt(r->curl, CURLOPT_PROXY, r->proxy_name);

      if (r->proxy_port) {
        curl_easy_setopt(r->curl, CURLOPT_PROXYPORT, r->proxy_port);
      }

      if (r->proxy_user) {
        curl_easy_setopt(r->curl, CURLOPT_PROXYUSERNAME, r->proxy_user);

        if (r->proxy_pass) {
          curl_easy_setopt(r->curl, CURLOPT_PROXYUSERPWD, r->proxy_pass);
        }
      } else {
        if (r->proxy_pass) {
          curl_easy_setopt(r->curl, CURLOPT_PROXYPASSWORD, r->proxy_pass);
        }
      }
    }

    int still_running;
    CURLM *multi_handle;
    multi_handle = curl_multi_init();
    curl_multi_add_handle(multi_handle, r->curl);
    curl_multi_perform(multi_handle, &still_running);

    do {
      struct timeval timeout;
      int rc; /* select() return code */ 
      CURLMcode mc; /* curl_multi_fdset() return code */ 

      fd_set fdread;
      fd_set fdwrite;
      fd_set fdexcep;
      int maxfd = -1;

      long curl_timeo = -1;

      FD_ZERO(&fdread);
      FD_ZERO(&fdwrite);
      FD_ZERO(&fdexcep);

      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      curl_multi_timeout(multi_handle, &curl_timeo);
      if(curl_timeo >= 0) {
        timeout.tv_sec = curl_timeo / 1000;
        if(timeout.tv_sec > 1)
          timeout.tv_sec = 1;
        else
          timeout.tv_usec = (curl_timeo % 1000) * 1000;
      }

      mc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

      if(mc != CURLM_OK) {
        fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
        break;
      }

      if(maxfd == -1) {
        struct timeval wait = { 0, 100 * 1000 }; /* 100ms */ 
        rc = select(0, NULL, NULL, NULL, &wait);
      }
      else {
        rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
      }
      ALLOW_X_LOOP; ALLOW_ISPELL; 

      switch(rc) {
        case -1:
          if (errno != EINTR) {
            /* select error */ 
            break;
          }
        case 0: /* timeout */ 
        default: /* action */ 
          curl_multi_perform(multi_handle, &still_running);
          break;
      }
    } while(still_running);

    curl_multi_remove_handle(multi_handle, r->curl);
    curl_multi_cleanup(multi_handle);

    if (r->error != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(r->error));
	} else {
      long sizep;
      curl_easy_getinfo(r->curl, CURLINFO_REQUEST_SIZE, &sizep);

      global_http_upload_cnt += sizep;

      long codep;
      curl_easy_getinfo(r->curl, CURLINFO_RESPONSE_CODE, &codep);

      r->response = codep;
      if (r->response >= 400) {
            /* Triton> maintenant, le 201 Created renvoyé par la tribune de test de zorel n'indique plus d'erreur */
            /* See> on peut aussi s'attendre à une 303 See Other quand un post est créé */
        r->error = 1;
        strcpy(http_last_err_url, http_last_url);
        time(&http_err_time);

        myprintf(_("[%<MAG %s>]: %<yel %s>\n"), http_last_err_url, http_last_err_msg);
      }


      _http_request_send_parse_headers(r);
	}

    curl_slist_free_all(list);
  }
  curl_global_cleanup();
}

void
http_request_init(HttpRequest *r) {
  memset(r, 0, sizeof(HttpRequest));
  r->content_length = -1;
  r->is_chunk_encoded = 0;
  r->header_data = str_ndup("", 1);
  r->header_size = 0;
  r->response_data = str_ndup("", 1);
  r->response_size = 0;
}

char *
http_error(HttpRequest *r) {
  return http_last_err_msg;
}

int http_is_ok(HttpRequest *r) {
  return r->error == 0;
}

void http_request_close (HttpRequest *r) {
      long portp;
      curl_easy_getinfo(r->curl, CURLINFO_LOCAL_PORT, &portp);

  FREE_STRING(r->url);
  FREE_STRING(r->proxy_name);
  FREE_STRING(r->proxy_user);
  FREE_STRING(r->proxy_pass);
  FREE_STRING(r->user_agent);
  FREE_STRING(r->referer);
  FREE_STRING(r->cookie);
  FREE_STRING(r->new_cookie);
  FREE_STRING(r->accept); /* Triton> Accept: header/http */
  FREE_STRING(r->post);
  FREE_STRING(r->header_data);
  FREE_STRING(r->response_data);
  FREE_STRING(r->content_type);
  r->content_length = -1;
  r->header_size = -1;
  r->response_size = -1;
  curl_easy_cleanup(r->curl);
}

char*
http_complete_error_info()
{
  char s[2048];
  char s_err[1024];
  char s_heure[80];

  if (flag_http_transfert == 0) {
    if (http_err_time == 0) {
      snprintf(s_heure, 80, _("<i>There hasn't been any http error yet</i>"));
    } else {
      struct tm *t;
      t = localtime(&http_err_time);
      
      snprintf(s_heure, 80, _("Last error occured at: <b>%02d:%02d:%02d</b>"),
	       t->tm_hour, t->tm_min, t->tm_sec);
    }

    if (flag_http_error) {
      snprintf(s_err, 1024, _("Error: <b><font color=#800000>%s</font></b><br>"), http_last_err_msg);
    } else {
      if (http_err_time) {
	snprintf(s_err, 1024, _("<br>The last error was: %s<br>for the URL:<tt>%s</tt><br>"),
		 http_last_err_msg, http_last_err_url);
      } else {
	s_err[0] = 0;
      }
    }
    snprintf(s, 2048, _("%s<br>%s<br>%s: <tt>%s</tt><br>Host IP: <font color=#0000ff>%s</font><br>%s"),
	     flag_http_error ? _("<b>There has just been an error !!</b>") : _("The last transfer went fine."),
	     s_heure, flag_http_error ? _("faulty URL") : _("We have just downloaded"), http_last_url, http_used_ip, s_err);
  } else {
    if (flag_gethostbyname == 0) {
      snprintf(s, 2048, _("Download going on...<br>URL: <tt>%s</tt><br>IP: <font color=#0000ff>%s</font><br>"), http_last_url, http_used_ip);
    } else {
      snprintf(s, 2048, _("Resolving name '%s'...<br>"), http_last_url);
    }
  }
  return strdup(s);
}

