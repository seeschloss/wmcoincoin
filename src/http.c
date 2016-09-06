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

static char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


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


int http_is_ok(HttpRequest *r) { return r->telnet.error == 0; }

/*
  renvoie le descripteur de fichier vers les donnees renvoyees ,
  descripteur a fermer par un close(d)
*/
size_t
_http_request_send_curl_write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  //struct MemoryStruct *mem = (struct MemoryStruct *)userp;
  HttpRequest *r = (HttpRequest *)userp;

  /*
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  */

  return realsize;
}

#include <curl/curl.h>
void
http_request_send(HttpRequest *r)
{
  CURL *curl;
  CURLcode res;

  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);

  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, r->url);

  	if (r->type == HTTP_POST) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS,r->post);
	}

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _http_request_send_curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)r);

    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
          curl_easy_strerror(res));

    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
}

void
http_request_init(HttpRequest *r) {
  memset(r, 0, sizeof(HttpRequest));
  r->content_length = -1;
  r->is_chunk_encoded = 0;
  /* proxy par défaut choppé dans la var d'env "http_proxy" 
     
  TODO: gerer aussi la var. d'env "no_proxy"
  */
  if (!str_is_empty(getenv("http_proxy"))) {
    char *http_proxy_ = str_ndup(getenv("http_proxy"),1024), *p;
    char *http_proxy = http_proxy_;
    for (p = http_proxy; *p; ++p)
      if ((*p) >= 0 && *p < ' ') *p = ' ';
    if (str_startswith(http_proxy, "http://")) http_proxy += 7;
    
    if ((p=strchr(http_proxy, '@'))) {
      char auth[200]; strncpy(auth, http_proxy, MIN(200, p - http_proxy)); auth[(sizeof auth) - 1] = 0;
      http_proxy = p+1; 
      ASSIGN_STRING_VAL(r->proxy_user_pass, auth);
    }

    /* remove trailing slashes */
    for (p = http_proxy; *p; ++p)
      if (*p == '/') *p = 0;

    if ((p=strchr(http_proxy, ':'))) {
      r->proxy_name = str_ndup(http_proxy, p - http_proxy);
      r->proxy_port = atoi(p+1);
    } else {
      ASSIGN_STRING_VAL(r->proxy_name, http_proxy);
      r->proxy_port = 3128;
    }
  }
  telnet_session_init(&r->telnet);
}

void http_request_close (HttpRequest *r) {
  FREE_STRING(r->host_path);
  FREE_STRING(r->proxy_name);
  FREE_STRING(r->proxy_user_pass);
  FREE_STRING(r->user_agent);
  FREE_STRING(r->referer);
  FREE_STRING(r->cookie);
  FREE_STRING(r->accept); /* Triton> Accept: header/http */
  FREE_STRING(r->post);
  r->content_length = -1;
  telnet_session_close(&r->telnet);
}

