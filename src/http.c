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
#include "fake-getaddrinfo.h"
#include "fake-getnameinfo.h"
#include "inet_aton.c"
#include "fake-getaddrinfo.c"
#include "fake-getnameinfo.c"

static void 
dns_cache_destroy(HostEntry *h) {
  assert(h);
  free(h->host_name); 
  free(h->numeric_host);
  h->next = NULL;
  free(h);
}

static void 
dns_cache_remove_host(HostEntry *h) {
  if (h == NULL) return;
  if (h == dns_cache) {
    dns_cache = h->next;
  } else {
    HostEntry *hh = dns_cache;
    while (hh->next != h) hh = hh->next;
    assert(hh->next == h);
    hh->next = h->next;
  }
  dns_cache_destroy(h);
}

static HostEntry *
dns_cache_find_host_by_name(const char *host_name, int port) {
  HostEntry *h = NULL;
  h = dns_cache;
  while (h) {
    if (strcasecmp(host_name, h->host_name) == 0 && port == h->port) {
      break;
    }
    h = h->next;
  }
  return h;
}

static void
dns_cache_remove_host_by_name(const char *host_name, int port) {
  HostEntry *h = dns_cache_find_host_by_name(host_name,port);
  if (h) dns_cache_remove_host(h);
}

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


static SOCKET net_connect(const char *host_name, int port, int* tic_cnt);

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

static void
set_http_err()
{
  strcpy(http_last_err_url, http_last_url);
  time(&http_err_time);
}

int http_is_ok(HttpRequest *r) { return r->telnet.error == 0; }

int base64_encode(const void *data, int size, char **str)
{
  char *s, *p;
  int i;
  int c;
  const unsigned char *q;

  p = s = (char*)malloc(size*4/3+4);
  if (p == NULL)
      return -1;
  q = (const unsigned char*)data;
  i=0;
  for(i = 0; i < size;){
    c=q[i++];
    c*=256;
    if(i < size)
      c+=q[i];
    i++;
    c*=256;
    if(i < size)
      c+=q[i];
    i++;
    p[0]=base64[(c&0x00fc0000) >> 18];
    p[1]=base64[(c&0x0003f000) >> 12];
    p[2]=base64[(c&0x00000fc0) >> 6];
    p[3]=base64[(c&0x0000003f) >> 0];
    if(i > size)
      p[3]='=';
    if(i > size+1)
      p[2]='=';
    p+=4;
  }
  *p=0;
  *str = s;
  return strlen(s);
}

/* 
   et hop ! les 3 fonctions suivantes ont ete piquees dans wget 1.6 
   (dont le code source est fort joli, soit dit en passant :)

   gpl roulaize :)
*/

/* Wait for file descriptor FD to be readable, MAXTIME being the
   timeout in seconds.  If WRITEP is non-zero, checks for FD being
   writable instead.

   Returns 1 if FD is accessible, 0 for timeout and -1 for error in
   select().  */
static int
http_select_fd (SOCKET fd, int maxtime_sec, int maxtime_usec, int writep)
{
  fd_set fds, exceptfds;
  struct timeval timeout;
  int retval;

  FD_ZERO (&fds);
  FD_SET (fd, &fds);
  FD_ZERO (&exceptfds);
  FD_SET (fd, &exceptfds);
  timeout.tv_sec = maxtime_sec;
  timeout.tv_usec = maxtime_usec;
  ALLOW_X_LOOP_MSG("http_select_fd.1"); ALLOW_ISPELL;  
  /* HPUX reportedly warns here.  What is the correct incantation?  */
  CYGWIN_ENABLE_THREAD_X_LOOP;
  retval = select (fd + 1, writep ? NULL : &fds, writep ? &fds : NULL,
		 &exceptfds, &timeout);
  CYGWIN_DISABLE_THREAD_X_LOOP;
  ALLOW_X_LOOP_MSG("http_select_fd.2"); ALLOW_ISPELL;
  return retval;
}


/* Read at most LEN bytes from FD, storing them to BUF.  This is
   virtually the same as read(), but takes care of EINTR braindamage
   and uses select() to timeout the stale connections (a connection is
   stale if more than OPT.TIMEOUT time is spent in select() or
   read()).  */
static int
net_iread (SOCKET fd, char *buf, int len)
{
  int res;

  flag_http_transfert++;


  do
    {
      if (flag_cancel_task) goto error;
      //#ifdef HAVE_SELECT
      if (Prefs.http_timeout)
	{
	  int tic0;
	  tic0 = wmcc_tic_cnt;
	  do
	    {
	      res = http_select_fd (fd, Prefs.http_timeout, 0, 0);	
	      ALLOW_X_LOOP; ALLOW_ISPELL; 
	      if ((wmcc_tic_cnt - tic0) > Prefs.http_timeout*(1000/WMCC_TIMER_DELAY_MS)) {
		SETERR_TIMEOUT;
		printf(_("timeout (t=%d milliseconds)..\n"), (wmcc_tic_cnt - tic0)*WMCC_TIMER_DELAY_MS);
	      }
	      if (flag_cancel_task) goto error;
	    }
	  while (res == SOCKET_ERROR && LASTERR_EINTR);
#ifndef __CYGWIN__
	  if (res <= 0) {
	    /* Set errno to ETIMEDOUT on timeout.  */
	    if (res == 0)
	      /* #### Potentially evil!  */
	      SETERR_TIMEOUT;
	    goto error;
	  }
#else
	  if (res == SOCKET_ERROR) {
	    printf(_("net_iread: socket error, res=%d (%s)\n"), res, STR_LAST_ERROR);
            goto error;
	  }
	  if (res == 0) {
	    printf (_("net_iread: Timeout...\n"));
	    goto error;
	  }
#endif
	}
      //#endif

#ifdef __CYGWIN__      	  
      res = recv (fd, buf, len, 0);
#else
      res = read(fd, buf, len); 
#endif
      if (res != SOCKET_ERROR) global_http_download_cnt += res;
      ALLOW_X_LOOP; ALLOW_ISPELL; 
    }
  while (res == SOCKET_ERROR && LASTERR_EINTR);

  flag_http_error = 0;
  flag_http_transfert--;

  ALLOW_X_LOOP; ALLOW_ISPELL;
  return res;

 error:
  flag_http_error = 1;
  flag_http_transfert--;
  ALLOW_X_LOOP; ALLOW_ISPELL;
  return SOCKET_ERROR;
}

/* Write LEN bytes from BUF to FD.  This is similar to iread(), but
   doesn't bother with select().  Unlike iread(), it makes sure that
   all of BUF is actually written to FD, so callers needn't bother
   with checking that the return value equals to LEN.  Instead, you
   should simply check for -1.  */
static int
net_iwrite (SOCKET fd, char *buf, int len)
{
  int res = 0;

  flag_http_transfert++;

  /* `write' may write less than LEN bytes, thus the outward loop
     keeps trying it until all was written, or an error occurred.  The
     inner loop is reserved for the usual WSAEINTR f*kage, and the
     innermost loop deals with the same during select().  */
  while (len > 0) {
    do {
      if (flag_cancel_task) goto error;
      //#ifdef HAVE_SELECT
      if (Prefs.http_timeout) {
	do {
	  res = http_select_fd (fd, Prefs.http_timeout, 0, 1);
	  ALLOW_X_LOOP; ALLOW_ISPELL;
	  if (flag_cancel_task) goto error;
	} while (res == SOCKET_ERROR && LASTERR_EINTR);
#ifndef __CYGWIN__
	if (res <= 0) {
	  /* Set errno to ETIMEDOUT on timeout.  */
	  if (res == 0)
	    /* #### Potentially evil!  */
	    SETERR_TIMEOUT;
	  goto error;
	}
#else
	if (res == SOCKET_ERROR)
	  goto error;
	if (res == 0) {
	  printf (_("net_iwrite: Timeout...\n"));
	  goto error;
	}
#endif /* ifndef __CYGWIN__ */
      }
      //#endif

#ifdef __CYGWIN__
      res = send (fd, buf, len, 0);
#else
      res = write(fd, buf, len);
#endif
      if (res != SOCKET_ERROR) global_http_upload_cnt += res;
    } while (res == SOCKET_ERROR && LASTERR_EINTR);
    if (res == SOCKET_ERROR)
      break;


    buf += res;
    len -= res;
  }
  flag_http_error = 0;
  flag_http_transfert--;
  return res;

 error:
  flag_http_error = 1;
  flag_http_transfert--;
  return SOCKET_ERROR;
}

/* c'est fini pour les fonctions de wget */

char *
http_error() {
  return http_last_err_msg;
}

/* stolen from woof patch and from wget debian sources (vanilla wget does not have ipv6) */
static char * 
get_host_ip_str(const char *hostname, int port) {
  int error;
  struct addrinfo hints, *res=0;
  char service_name[256];
  char *s = NULL;

  snprintf(service_name, 256, "%d", port);

  if (flag_cancel_task) return NULL;
  memset(&hints, 0, sizeof(hints));
  if (Prefs.http_inet_ip_version == 4) {
    hints.ai_family = AF_INET;
  } else if (Prefs.http_inet_ip_version == 6) {
    hints.ai_family = AF_INET6;
  } else {
    hints.ai_family = AF_UNSPEC;
  }
  hints.ai_socktype = SOCK_STREAM;
  error = getaddrinfo(hostname, service_name, &hints, &res);
  if (!error) {
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    struct addrinfo *r;
    for (r = res; r; r = r->ai_next) {
      if (getnameinfo(r->ai_addr, r->ai_addrlen, hbuf, sizeof(hbuf), sbuf,
                      sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
        s = str_cat_printf(s, "%s%s", (s == NULL ? "" : "|"), hbuf);
      }
    }
    freeaddrinfo(res);
  } else {
    myfprintf(stderr, "error from getaddrinfo: %s", GAI_STRERROR(error));
    if (res) freeaddrinfo(res);
  }
  return s;
}

char *
get_host_ip_str_bloq(const char *hostname, int port) {
  char *s = NULL;
  BLAHBLAH(Prefs.verbosity_http, printf(_("get_host_ip_str_bloq('%s') -> if the network lags, the coincoin can be blocked here\n"), hostname));
  ALLOW_X_LOOP; usleep(30000); /* juste pour laisser le temps à l'affichage de mettre à jour la led indiquant 'gethostbyname' */
  ALLOW_X_LOOP_MSG("get_host_ip_str_bloq(1)"); ALLOW_ISPELL;
  s = get_host_ip_str(hostname, port);
  ALLOW_X_LOOP_MSG("get_host_ip_str_bloq(2)"); ALLOW_ISPELL;  
  return s;
}

/* inclusion du code specifique */
#ifdef __CYGWIN__
#include "http_win.c"
#else
#include "http_unix.c"
#endif


/* resolution d'un nom (en ipv4 ou ipv6) avec gestion d'un cache trisomique */
static HostEntry *
http_resolv_name(const char *host_name, int port, int force_dns_query)
{
  HostEntry *h = NULL;
  int h_found = 0;
  int do_dns_query = force_dns_query;

  /* recherche de l'host_name dans la liste des noms déjà connus */
  h = dns_cache_find_host_by_name(host_name, port);

  if (h == NULL) {
    h = (HostEntry*) calloc(1, sizeof(HostEntry)); assert(h);
    h->host_name = strdup(host_name);
    h->numeric_host = NULL;
    h->port = port;
    h->next = dns_cache;
    do_dns_query = 1;
  } else h_found = 1;

  if (do_dns_query) {
    flag_gethostbyname = 1;
    if (h->numeric_host) { free(h->numeric_host); h->numeric_host = NULL; }
    if ((Prefs.debug & 8) == 0) {
      h->numeric_host = get_host_ip_str_nonbloq(host_name, port);
    } else {
      h->numeric_host = get_host_ip_str_bloq(host_name, port);
    }
    flag_gethostbyname = 0;
    if (h_found == 0) {
      if (h->numeric_host) {
	dns_cache = h;
      } else {
        dns_cache_destroy(h);
	h = NULL;
      }
    }
  }

  if (Prefs.http_inet_ip_version != 6)
    snprintf(http_used_ip, 20, "???.???.???.???");
  else 
    snprintf(http_used_ip, 20, "?:?:?:?:?:?:?:?");

  if (h && h->numeric_host) {
    snprintf(http_used_ip, 100, "%s", h->numeric_host);
    BLAHBLAH(Prefs.verbosity_http, myprintf("--> host='%<YEL %s>', ip=%<MAG %s>\n", host_name, http_used_ip));
    return h;
  } else return NULL;
}

static SOCKET
http_try_connect_to_resolved_host(HostEntry *h) {
  SOCKET sockfd = INVALID_SOCKET;
  char *hostnumstr;

  hostnumstr = h->numeric_host; assert(hostnumstr);
  /* boucle sur toutes les ips */
  do {
    /* convert the the string containing numeric ip into the adequate structure */
    struct sockaddr_storage sock_name;
    struct addrinfo hints, *res;
    char portstr[NI_MAXSERV];
    int err;
    char *end;
    char *currenthost;
    int salen;

    /* remplissage de sock_name */

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST;
    snprintf(portstr, sizeof(portstr), "%d", h->port);
    end = strchr(hostnumstr, '|'); 
    if (end) { currenthost = str_ndup(hostnumstr, end-hostnumstr); ++end; }
    else currenthost = strdup(hostnumstr);
    err = getaddrinfo(currenthost, portstr, &hints, &res);
    if (err) {
      printf("erreur dans getaddrinfo(%s) : %s\n", currenthost, GAI_STRERROR (err));
      printf("comme a priori ça ne devrait pas arriver, ==> moment suicide <==\n");
      printf("si vous pensez que cette décision n'est pas justifiée, "
             "addressez-vous aux autorités compétetentes\n");
      assert(0);
    }
    memset (&sock_name, 0, sizeof (sock_name));
    memcpy (&sock_name, res->ai_addr, res->ai_addrlen);
    salen = res->ai_addrlen;
    freeaddrinfo (res);

    /* creation du sockect */

    sockfd = socket (((struct sockaddr *)&sock_name)->sa_family, SOCK_STREAM, 0);
    ALLOW_X_LOOP; ALLOW_ISPELL;
    if (sockfd == INVALID_SOCKET) {
      if (end) {
        BLAHBLAH(Prefs.verbosity_http,myfprintf(stderr, "avertissement sans frais: la chaussette sur '%<YEL %s>' "
                                       "a renvoyé '%<MAG %s>'\n",
                                       currenthost, STR_LAST_ERROR));
      } else {
        set_http_err();
        snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, _("Unable to create a socket ! (%s) [ip=%s]"), STR_LAST_ERROR, h->numeric_host);
      }
    } else {
      /* y'a le probleme des timeout de connect ...
         d'ailleurs je n'ai toujours pas compris pourquoi tous les
         sigalrm balance par l'itimer de wmcoincoin n'interferent
         pas avec le connect...
      */
      BLAHBLAH(Prefs.verbosity_http, printf(_("connecting on port %d...\n"), h->port));
#ifdef CONNECT_WITHOUT_TIMEOUT // a definir pour les os chiants
      err = net_tcp_connect(sockfd, (struct sockaddr *)&sock_name, salen);
#else
      err = net_tcp_connect_with_timeout(sockfd, (struct sockaddr *)&sock_name, 
                                         salen, Prefs.http_timeout);
#endif
      if (err) {
        net_close(sockfd); sockfd = INVALID_SOCKET;
        if ( end == NULL) {
          set_http_err();
          snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, "connect(): %s", STR_LAST_ERROR);
          ALLOW_X_LOOP; ALLOW_ISPELL;
          BLAHBLAH(Prefs.verbosity_http, printf(_("connection failed: %s..\n"), http_last_err_msg));
          dns_cache_remove_host(h); /* pour relancer un gethostbyname au prochain coup */
        } else {
          BLAHBLAH(Prefs.verbosity_http,printf("avertissement sans frais: le connect vers '%s' vient d'echouer (%s), on tente l'ip suivante\n", currenthost, STR_LAST_ERROR));
        }
      } else { /* ouéééé */
        /* ruse de sioux: comme cette ip a l'air de bien marcher, on la fait passer en premier */
        if (hostnumstr != h->numeric_host) {
          end = h->numeric_host+strlen(h->numeric_host); assert(*end == 0);
          memmove(h->numeric_host+strlen(currenthost)+1, 
                  h->numeric_host, hostnumstr-h->numeric_host);
          *end = 0; /* on l'a écrasé si hostnumstr était le dernier */
          strcpy(h->numeric_host, currenthost);
          h->numeric_host[strlen(currenthost)] = '|';
          BLAHBLAH(Prefs.verbosity_http,myprintf("version rearrangée de la liste d'ips: '%<YEL %s>'\n", h->numeric_host));
        }
      }
    }
    free(currenthost); currenthost=NULL;
    hostnumstr = end;
  } while (sockfd == INVALID_SOCKET && hostnumstr);
  
  return sockfd;
}

/* -1 => erreur */
static SOCKET
net_connect(const char *host_name, int port, int *connect_tic_cnt)
{
  SOCKET sockfd = INVALID_SOCKET;

  int num_try;

  HostEntry *h;

   /* 
     un peu tordu : 
     on ne fait qu'un gethostbyname au debut et on sauve son addresse (economie de 0.25s en moyenne)
     ensuite, on rappelle la structure sauvee. en cas de probleme de connect, on retente un gethostbyname
     (des fois que l'ip aurait change...)

     BIEN SUR CA NE MARCHE QUE SI L'ON S'ADDRESSE TOUJOURS AU MEME SITE !
  */

  for (num_try = 0; num_try < 2; num_try++) {

    /* fait un gethostbyname */
    h = http_resolv_name(host_name, port, (num_try == 1));
    
    if (h == NULL) {
      if (num_try == 0) continue; /* on a droit a un deuxième essai */
      else {
        set_http_err();
        snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, _("Unable to resolve '%s'"), host_name);
        return INVALID_SOCKET;
      }
    }
    if (connect_tic_cnt && *connect_tic_cnt == -1)
      *connect_tic_cnt = wmcc_tic_cnt;
    
    if ((sockfd = http_try_connect_to_resolved_host(h)) != INVALID_SOCKET) break;
  }
  return sockfd;
}



void
http_print_request(HttpRequest *r)
{
  assert(r);
  printf("------------HttpRequest-----------\n");
  myprintf("type = %s\n", r->type == HTTP_GET ? "GET" : "POST");
  myprintf("host  = '%<YEL %s>'\n", r->telnet.host);
  myprintf("port  = '%<YEL %d>'\n", r->telnet.port);
  myprintf("path  = '%<YEL %s>'\n", r->host_path);
  myprintf("proxy = '%<YEL %s>'\n", r->proxy_name);
  myprintf("proxy_user_pass = '%<YEL censored>'\n");
  myprintf("proxy_port = '%<YEL %d>'\n", r->proxy_port);
  myprintf("pragma_nocache = '%<YEL %d>'\n", r->pragma_nocache);

  myprintf("useragent = '%<YEL %s>'\n", r->user_agent);
  myprintf("cookie = '%<YEL %s>'\n", r->cookie);
  myprintf("accept = '%<YEL %s>'\n", r->accept); /* Triton> Accept: header/http */
  myprintf("last_modified = '%<YEL %s>'\n", r->p_last_modified ? *r->p_last_modified : "unused");
  myprintf("is_chunk_encoded = '%<YEL %d>'\n", r->is_chunk_encoded);
  myprintf("chunk_num = '%<YEL %d>'\n", r->chunk_num);
  myprintf("chunk_size = '%<YEL %d>'\n", r->chunk_size);
  myprintf("chunk_pos = '%<YEL %d>'\n", r->chunk_pos);
  myprintf("fd = '%<YEL %d>'\n", (int)r->telnet.fd);
  myprintf("error = '%<YEL %d>'\n", r->telnet.error);
}


/*
  analyse tres rapidement la reponse du serveur

  si il renvoie un vrai header, avec un 200 OK ou 302 Found, ça roule
  Triton> s'il (Tu devrais apprendre le francais mon cher pouaite<) renvoie 201 Created, c'est bon aussi
          c'est a cause de zorel< qui fait rien qu'a donner des comportements bizarres a sa future Tribune Web 4.2
  si il renvoie autre chose (404 etc..) on renvoie r->error=1
  si il y a une connexion timeout, on renvoie r->error=2
*/
void
http_skip_header(HttpRequest *r)
{
  char buff[512];
  int i, got, lnum;
  char last;
  int ok = 0;

  lnum = 0;
  i = 0;
  //  printf("http_skip header\n");
  buff[511] = 0;
  last = 0;

  r->is_chunk_encoded = 0;
  do {
    while((got = net_iread(r->telnet.fd, buff+i, 1)) > 0) {
      buff[i+1] = 0;
      BLAHBLAH(Prefs.verbosity_http, myprintf("%<GRN %c>", buff[i]););
      if(buff[i] == '\n' && (last == '\n')) {
	ok = 1; /* on vient de lire le header tranquillement */
	break;
      }
      if(buff[i] == '\r')
	continue;
      last = buff[i];
      if (buff[i] == '\n') {
	if (lnum == 0) {
	  int j;
	  j = 0;
	  while (buff[j] != ' ' && buff[j]) j++;
	  if (buff[j] == ' ') {
	    r->response = atoi(buff+j+1);
            if (r->response == 304) {
              //if (strcasecmp("304 Not Modified\n", buff+j+1) == 0) {
	      r->content_length = 0; /* ça sert à rien d'essayer de lire un truc vide 
					c'est pas super joli de faire ça ici mais ça ira pour cette fois
				      */
	    } else if ((r->response != 200) && (r->response != 201) && (r->response != 302)) {
              /* Triton> maintenant, le 201 Created renvoyé par la tribune de test de zorel n'indique plus d'erreur */
              /*if (strcasecmp("200 OK\n", buff+j+1) != 0 && 
		strcasecmp("302 Found\n", buff+j+1) != 0 &&
		strcasecmp("302 Moved Temporarily\n", buff+j+1) != 0) {*/
	      set_http_err();
	      snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, "%s",buff+j+1); 
	      myprintf(_("[%<MAG %s>]: %<yel %s>"), http_last_url, buff+j+1);
	      r->telnet.error = 1;
              dns_cache_remove_host_by_name(r->telnet.host,r->telnet.port);
	    }
	  }
	} else {
	  if (r->p_last_modified) {
	    if (strncmp(buff,"Last-Modified: ",15) == 0) {
	      if (*r->p_last_modified) { free(*r->p_last_modified); *r->p_last_modified = NULL; }
	      *r->p_last_modified = strdup(buff+15);
	    }
	  }
	  if (strncmp(buff, "Transfer-Encoding: chunked", 26) == 0) {
	    r->is_chunk_encoded = 1;
	    r->chunk_num = -1;
	  }
	  if (strncmp(buff, "Content-Length:", 15) == 0) {
	    r->content_length = atoi(buff+15);
	    BLAHBLAH(Prefs.verbosity_http,printf("content length: %d\n", r->content_length));
	  }
	  if (strncmp(buff, "X-Post-Id:", 10) == 0) {
	    r->post_id = atoi(buff+10);
	    BLAHBLAH(Prefs.verbosity_http,printf("post id: %d\n", r->post_id));
	  }
	  if (strncmp(buff, "Set-Cookie:", 11) == 0) {
	    /* Format: Set-Cookie: <name>=<value>[; <name>=<value>]...
	                           [; expires=<date>][; domain=<domain_name>]
	                           [; path=<some_path>][; secure][; httponly] */
	    char *garbage;
		if (garbage = strstr(buff+11, "expires=")) {
			r->new_cookie = strndup(buff+11, garbage - buff - 11);
		} else if (garbage = strstr(buff+11, "domain=")) {
			r->new_cookie = strndup(buff+11, garbage - buff - 11);
		} else if (garbage = strstr(buff+11, "path")) {
			r->new_cookie = strndup(buff+11, garbage - buff - 11);
		} else if (garbage = strstr(buff+11, "secure")) {
			r->new_cookie = strndup(buff+11, garbage - buff - 11);
		} else if (garbage = strstr(buff+11, "httponly")) {
			r->new_cookie = strndup(buff+11, garbage - buff - 11);
		} else {
			r->new_cookie = strdup(buff+11);
		}

	    BLAHBLAH(Prefs.verbosity_http,printf("new cookie: %s\n", r->new_cookie));
	  }
	}
	lnum++;
	i=0;
      } else {
	i++; if (i >= 511) i = 510;
      }
    }
  } while (got==SOCKET_ERROR && LASTERR_EAGAIN); 
  if (got == SOCKET_ERROR || ok == 0) {
    set_http_err();
    snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, _("http_skip_header has a socket issue ! (%s)"), STR_LAST_ERROR);
    r->telnet.error = 2;
    return;
  }
  //  r->telnet.error = 0;
  return;
}


/* lecture de la REPONSE du serveur 
   (après analyse de l'entête)

   
*/
int
http_read(HttpRequest *r, char *buff, int len)
{
  int got;
  assert(r->telnet.error == 0);

  if (len>1) {
    BLAHBLAH(Prefs.verbosity_http+1, printf(_("http_read: request of length %d, pos = %ld, chunk=%d (size %ld)\n"), len, r->chunk_pos, r->chunk_num, r->chunk_size));
  }

  if (r->is_chunk_encoded == 1) {
    if (r->chunk_pos > r->chunk_size) {
      printf(_("Damned, the chunk_encoding has gone down the tubes on the following request:\n"));
      http_print_request(r);
    }
    if (r->chunk_num == -1 || (r->chunk_pos == r->chunk_size && r->chunk_size != 0)) {
      char s_chunk_size[512];
      int i;
      int lcnt;

      r->chunk_num++; r->chunk_pos = 0;
      i = 0; lcnt = 0;
      while(i < 511 && (got = net_iread(r->telnet.fd, s_chunk_size+i, 1)) > 0) {
	if (s_chunk_size[i] == '\n') {
	  lcnt++;
	  if (lcnt == 2 || (lcnt == 1 && r->chunk_num == 0)) 
	    break;
	  else { i = 0; continue; }
	}
	if (s_chunk_size[i] == '\r') s_chunk_size[i] = ' ';
	i++;
      }
      s_chunk_size[i] = 0;
      if (sscanf(s_chunk_size, "%lx", &r->chunk_size) != 1) {
	r->telnet.error = 1; 
	printf(_("error in chunk '%s'\n"), s_chunk_size);
	return 0;
      }
      BLAHBLAH(Prefs.verbosity_http, printf("http_read: CHUNK %d, size = %ld ['0x%s']\n", r->chunk_num, r->chunk_size, s_chunk_size));
    }

    if (len > r->chunk_size - r->chunk_pos) len = r->chunk_size - r->chunk_pos;
  } else if (r->content_length != -1) {
    /*
    if (r->content_length - r->chunk_pos < 200) {
      printf("http_read/cl len=%d, pos=%ld, clen=%d\n",
	     len, r->chunk_pos, r->content_length);
    }
    */
    if (len > r->content_length - r->chunk_pos) len = r->content_length - r->chunk_pos;
  }
  
  if (len <= 0) return 0;

  got = net_iread(r->telnet.fd, buff, len);
  if (got > 0) { r->chunk_pos += got; }
  else if (got == SOCKET_ERROR) {
    if (!LASTERR_EAGAIN) {
      /* erreur non récupérable */
      set_http_err();
      snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, _("http_read has encountered a socket problem, pos=%d, len=%d !(%s)"), (int)r->chunk_pos, (int)len, STR_LAST_ERROR);
      r->telnet.error = 1;
    } else {
      got = 0; /* on n'a rien lu ce coup-ci, mais ça viendra */
    }
  }

  if (len>1) { // || (r->content_length != -1 && (r->content_length - r->chunk_pos < 200))) {
    BLAHBLAH(Prefs.verbosity_http+1,printf(_("http_read: length finally requested: %d, received: %d, new pos=%ld\n"),
	   len, got, r->chunk_pos));
  }

  return got;
}

int
http_get_line(HttpRequest *r, char *s, int sz)
{
  int i, got=0,cnt;


  flag_http_transfert++;

  i = 0;
  cnt = 0;
  s[0] = 0;
#ifndef __CYGWIN__ 
  errno = 0;
#else
  WSASetLastError(0);
#endif
  do {

    while (r->telnet.error == 0 && (got = http_read(r, s+i, 1)) > 0) {
      cnt++;
      if (s[i] == '\n' || s[i] == 0) {
	s[i] = 0; break;
      }
      if ((unsigned char)s[i] >= (unsigned char)' ') {
	i++;
	if (i >= sz) i = sz-1; /* pas cool */
      }
      s[i] = 0;
    }

    if (got == 0 && r->chunk_pos != r->content_length && !LASTERR_EAGAIN && !LASTERR_ESUCCESS && (!(r->is_chunk_encoded && r->chunk_size == 0))) {
      printf(_("http_get_line: weird, got=0 while reading %d/%d [r->telnet.error=%d, errmsg='%s']\n"), (int)r->chunk_pos, (int)r->content_length, r->telnet.error, STR_LAST_ERROR);
    }
  } while (got == 0 && LASTERR_EAGAIN && r->telnet.error == 0 && r->chunk_pos != r->content_length);

  if (r->telnet.error) {
    set_http_err();
    snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, _("http_get_line messed up (got=%d): %s!"), got, STR_LAST_ERROR);
    printf("[%s] %s\n", http_last_url, http_last_err_msg);
    goto error;
  }
  BLAHBLAH(Prefs.verbosity_http+1,myprintf(_("http_get_line sent (cnt=%d): '%<yel %s>'\n"), cnt, s));
  flag_http_transfert--;

  flag_http_error = 0;

  BLAHBLAH(Prefs.verbosity_http,myprintf("%<yel .>"); fflush(stdout));
  return cnt;


 error:
  flag_http_error = 1;
  flag_http_transfert--;
  return SOCKET_ERROR;
}

int
http_get_line_trim(HttpRequest *r, char *s, int sz) {
  int err;
  assert(sz); s[0] = 0;  
  while ((err = http_get_line(r,s,sz)) > 0) {
    str_trim(s);
    if (s[0]) break;
  }
  if (err == SOCKET_ERROR) return err; 
  else return strlen(s);
}

#define CRLF "\015\012"



/*
  renvoie le descripteur de fichier vers les donnees renvoyees ,
  descripteur a fermer par un close(d)
*/
void
http_request_send(HttpRequest *r)
{
  char *header = NULL;

  /* corriger ce ifndef un de ces jours */
#ifndef __CYGWIN__
  if (Prefs.debug & 2) {
    r->telnet.fd = open(r->host_path, O_RDONLY);
    if (r->telnet.fd < 0) {
      fprintf(stderr, _("http_send_request/debug, unable to open '%s':%s\n"), 
	      r->host_path, STR_LAST_ERROR);
      r->telnet.error = 1;
    }
    return;
  }
#endif

  flag_http_transfert++;

  BLAHBLAH(Prefs.verbosity_http,myprintf("http_request_send: %<grn %s>\n", r->host_path));


  header = strdup("");


  /* GET ou POST */
  if (r->type == HTTP_GET) {
    if (r->proxy_user_pass == NULL) {
      if (r->proxy_name == NULL) {
	header = str_cat_printf(header, "GET %s HTTP/1.1" CRLF, r->host_path);
      } else {
	header = str_cat_printf(header, "GET http://%s%s HTTP/1.1" CRLF,
			      r->telnet.host, r->host_path);
      }
    } else {
      header = str_cat_printf(header, "GET http://%s:%d%s HTTP/1.1" CRLF,
			      r->telnet.host, r->telnet.port, r->host_path);
    }
  } else if (r->type == HTTP_POST) {
    if (r->proxy_name == NULL) {
      header = str_cat_printf(header, "POST %s HTTP/1.1" CRLF, 
			    r->host_path);
    } else {
      header = str_cat_printf(header, "POST http://%s:%d%s HTTP/1.1" CRLF, 
			    r->telnet.host, r->telnet.port, r->host_path);
    }
  }
  
  if (r->telnet.port != 80) {
    header = str_cat_printf(header, "Host: %s:%d" CRLF, r->telnet.host, r->telnet.port);
  } else {
    /* qd le port est celui par défaut, on ne le précise pas
       pour faire plaisir à f-cpu.tuxfamily.org qui n'en veut pas sinon */
    header = str_cat_printf(header, "Host: %s" CRLF, r->telnet.host);
  }

  if (r->cookie) {
    header = str_cat_printf(header, "Cookie: %s" CRLF, r->cookie);
  }

  if (r->pragma_nocache) {
    header = str_cat_printf(header, "Pragma: no-cache" CRLF "Cache-Control: no-cache" CRLF);
  }

  /* on ne gère que le schema d'authentification basique [base64(USER:PASS)]
     wget 1.6 fait mieux, mais pas curl 7.6 donc ca ira...
  */
  if (r->proxy_user_pass) {
    char *auth;
    base64_encode(r->proxy_user_pass, strlen(r->proxy_user_pass), &auth); assert(auth);
    header = str_cat_printf(header, "Proxy-Authorization: Basic %s" CRLF, auth);
    free(auth);
  }

  if (r->p_last_modified && *(r->p_last_modified) && r->use_if_modified_since) {
    unsigned char *s = *r->p_last_modified;
    int l;
    l = strlen(s); l--;
    while (l>=0 && s[l] < ' ') s[l--]=0;
    header = str_cat_printf(header, "If-Modified-Since: %s" CRLF, s);
  }

  snprintf(http_last_url, HTTP_LAST_ERR_URL_SZ, "%s:%d%s", r->telnet.host, r->telnet.port, r->host_path);

  if (r->user_agent) {
    header = str_cat_printf(header, "User-Agent: %s" CRLF,
			  r->user_agent);
  }

  if (r->referer) {
    header = str_cat_printf(header, "Referer: %s" CRLF,
			  r->referer);
  }

  /* Triton>
     le champ HttpRequest.accept a ete ajoute
     zorel< a decide que text/xml etait mieux que * / * donc on fait comme il a dit
     maintenant, le accept _devrait_ etre sette partout
   */
  if (r->accept) {
    header = str_cat_printf(header, "Accept: %s" CRLF, r->accept);
  }
  else {
  header = str_cat_printf(header, "Accept: */*" CRLF);
  }

  header = str_cat_printf(header, "Connection: close" CRLF);
  if (r->type == HTTP_GET) {
    header = str_cat_printf(header, CRLF);
  } else {
    header = str_cat_printf(header, "Content-Type: application/x-www-form-urlencoded" CRLF
			  "Content-Length: %d" CRLF CRLF "%s", (int)strlen(r->post),r->post);
  }
  

  if (r->proxy_name) {
    r->telnet.fd = net_connect(r->proxy_name, r->proxy_port, &r->telnet.tic_cnt_tstamp);
  } else {
    r->telnet.fd = net_connect(r->telnet.host, r->telnet.port, &r->telnet.tic_cnt_tstamp);
  }
  if (r->telnet.fd == INVALID_SOCKET) goto error_close;

  BLAHBLAH(Prefs.verbosity_http, myprintf("HTTP_REQUEST: \n%<YEL %s>\n", header));

  if (net_iwrite(r->telnet.fd, header, strlen(header)) == SOCKET_ERROR) {
    set_http_err();
    snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, _("http_get couldn't send its request: %s"), STR_LAST_ERROR);
    goto error_close;
  }

  BLAHBLAH(Prefs.verbosity_http ,printf(_("ok, request sent\n")));
  
  http_skip_header(r);
  if (r->telnet.error) {
    goto error_close;
  }

  if (header) free(header);
  flag_http_error = 0;
  flag_http_transfert--;
  return;

 error_close:
  http_request_close(r); r->telnet.fd = SOCKET_ERROR;
  if (header) free(header);
  r->telnet.error = 1;
  flag_http_error = 1;
  flag_http_transfert--;
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

/* fonction a-la-con: lecture de toutes les données en mémoire.. */
unsigned char *
http_read_all(HttpRequest *r, char *what)
{
  unsigned char *s;
  int bchunk = 1024;
  int bsize;

  /* on lit tout en un coup */
  bsize = bchunk;
  s = malloc(bsize+1); strcpy(s,_("Quack ! Missed"));
  if (s) {
    int got;
    int bi;
    bi = 0;
    
    /* attention : les ames sensible pourraient etre choques
       par la brutalite de ce qui va suivre ... */
    while ((got=http_read(r, s+bi, bchunk)) > 0 && r->telnet.error == 0) {
      bi += got;
      s[bi] = 0;
      bsize += bchunk;
      if (bsize > 300000) {
	  BLAHBLAH(0, myprintf(_("%s: too big (bsize=%d!), let's cut\n"),
			       what, bsize));
	  break;
      }
      s = realloc(s, bsize+1);
      if (!s) break;
    }
    if (got == -1) {
      fprintf(stderr, _("problem while reading %s: %s\n"), what, http_error());
      if (s) free(s);
      s = NULL;
    } else {
      s[bi] = 0;
    }
    BLAHBLAH(Prefs.verbosity_http+3, myprintf(_("%s, read: %<mag %s>\n"), what, s));
  }
  return s;
}

void telnet_session_init(TelnetSession *ts) {
  memset(ts, 0, sizeof(TelnetSession));
  ts->tic_cnt_tstamp = -1;
}

void telnet_session_open(TelnetSession *ts) {
  flag_http_transfert++;

  ts->fd = net_connect(ts->host, ts->port, &ts->tic_cnt_tstamp);
  if (ts->fd == INVALID_SOCKET) {
    ts->error = 1;
    flag_http_error = 1;
  }
  flag_http_transfert--;
}

void telnet_session_close(TelnetSession *ts) {
  if (ts->host) free(ts->host); ts->host = NULL;
  if (ts->fd != INVALID_SOCKET) {
    net_close(ts->fd);
    ts->fd = INVALID_SOCKET;
  }
}

void telnet_get_line(TelnetSession *ts, char *buff, int sz) {
  int i, got;
  int ok = 0;
  i = 0;

  flag_http_transfert++;
  buff[sz-1] = 0;  
  do {
    while((got = net_iread(ts->fd, buff+i, 1)) > 0) {
      buff[i+1] = 0;
      BLAHBLAH(Prefs.verbosity_http, myprintf("%<GRN %c>", buff[i]););
      if(buff[i] == '\n') {
	ok = 1;
	break;
      }
      if(buff[i] == '\r')
	continue;
      i++; if (i >= sz-1) i = sz-2;
    }
  } while (got==SOCKET_ERROR && LASTERR_EAGAIN);
  if (got == SOCKET_ERROR || ok == 0) {
    set_http_err();
    snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, _("telnet_get_line has a socket issue ! (%s)"), STR_LAST_ERROR);
    ts->error = 2;
    flag_http_transfert--;
    return;
  }
  flag_http_transfert--;
  flag_http_error = 0;
  return;
}

void telnet_send(TelnetSession *ts, char *s) {
  flag_http_transfert++;
  if (net_iwrite(ts->fd, s, strlen(s)) == SOCKET_ERROR) {
    set_http_err();
    snprintf(http_last_err_msg, HTTP_ERR_MSG_SZ, _("telnet_send couldn't send: %s"), STR_LAST_ERROR);
    goto error_close;
  }
  flag_http_error = 0;
  flag_http_transfert--;
  return;
 error_close:
  telnet_session_close(ts); ts->fd = SOCKET_ERROR;
  ts->error = 1;
  flag_http_error = 1;
  flag_http_transfert--;
}

int telnet_is_ok(TelnetSession *ts) { return ts->error == 0; }
