/* This file was stolen from openssh-3.5p1
   (original name openssh-3.5p1/openbsd-compat/fake-getaddrinfo.h
   merged with openssh-3.5p1/openbsd-compat/fake-gai-errnos.h
   ) */

/* Id: fake-getaddrinfo.h,v 1.2 2001/02/09 01:55:36 djm Exp $ */

#ifndef _FAKE_GETADDRINFO_H
#define _FAKE_GETADDRINFO_H

#include "config.h"

/* for old netdb.h */
#ifndef EAI_NODATA
#define EAI_NODATA	1
#define EAI_MEMORY	2
#endif

#ifndef AI_PASSIVE
# define AI_PASSIVE        1
# define AI_CANONNAME      2
#endif

#ifndef NI_NUMERICHOST
# define NI_NUMERICHOST    2
# define NI_NAMEREQD       4
# define NI_NUMERICSERV    8
#endif

#ifndef AF_INET6            // vilain solaris 7
  #define AF_INET6 666      // satan l'habite
#endif
#ifndef AI_NUMERICHOST 
  #define AI_NUMERICHOST 4  // n'importe quelle valeur fait l'affaire
#endif

/* stolen from openssh.. */
#ifndef HAVE_STRUCT_SOCKADDR_STORAGE
# define        _SS_MAXSIZE     128     /* Implementation specific max size */
# define       _SS_PADSIZE     (_SS_MAXSIZE - sizeof (struct sockaddr))

struct sockaddr_storage {
        struct  sockaddr ss_sa;
        char            __ss_pad2[_SS_PADSIZE];
};
# define ss_family ss_sa.sa_family
#endif /* !HAVE_STRUCT_SOCKADDR_STORAGE */

#ifndef HAVE_STRUCT_ADDRINFO
struct addrinfo {
	int	ai_flags;	/* AI_PASSIVE, AI_CANONNAME */
	int	ai_family;	/* PF_xxx */
	int	ai_socktype;	/* SOCK_xxx */
	int	ai_protocol;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
	size_t	ai_addrlen;	/* length of ai_addr */
	char	*ai_canonname;	/* canonical name for hostname */
	struct sockaddr *ai_addr;	/* binary address */
	struct addrinfo *ai_next;	/* next structure in linked list */
};
#endif /* !HAVE_STRUCT_ADDRINFO */

#ifndef HAVE_GETADDRINFO
int getaddrinfo(const char *hostname, const char *servname, 
                const struct addrinfo *hints, struct addrinfo **res);
#endif /* !HAVE_GETADDRINFO */

#ifndef HAVE_GAI_STRERROR
char *gai_strerror(int ecode);
#endif /* !HAVE_GAI_STRERROR */

#ifndef HAVE_FREEADDRINFO
void freeaddrinfo(struct addrinfo *ai);
#endif /* !HAVE_FREEADDRINFO */

#endif /* _FAKE_GETADDRINFO_H */
