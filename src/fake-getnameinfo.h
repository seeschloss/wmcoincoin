/* This file was stolen from openssh-3.5p1
   (original name openssh-3.5p1/openbsd-compat/fake-getnameinfo.h) */

#ifndef _FAKE_GETNAMEINFO_H
#define _FAKE_GETNAMEINFO_H

#include "config.h"

#ifndef HAVE_GETNAMEINFO
int getnameinfo(const struct sockaddr *sa, size_t salen, char *host, 
                size_t hostlen, char *serv, size_t servlen, int flags);
#endif /* !HAVE_GETNAMEINFO */

#ifndef NI_MAXSERV
# define NI_MAXSERV 32
#endif /* !NI_MAXSERV */
#ifndef NI_MAXHOST
# define NI_MAXHOST 1025
#endif /* !NI_MAXHOST */

#endif /* _FAKE_GETNAMEINFO_H */
