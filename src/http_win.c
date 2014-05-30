/*
  rcsid=$Id: http_win.c,v 1.16 2004/03/07 13:51:12 pouaite Exp $
  ChangeLog:
  $Log: http_win.c,v $
  Revision 1.16  2004/03/07 13:51:12  pouaite
  commit du dimanche

  Revision 1.15  2003/03/06 20:24:17  pouaite
  cygwin remarche, ou pas

  Revision 1.14  2003/03/01 17:31:22  pouaite
  compat ipv6 a tester

  Revision 1.13  2002/09/05 23:11:57  pouaite
  <blog>ce soir g mangé une omelette</blog>

  Revision 1.12  2002/08/26 00:52:22  pouaite
  coin coin coin

  Revision 1.11  2002/06/23 10:44:05  pouaite
  i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !

  Revision 1.10  2002/06/01 17:54:04  pouaite
  nettoyage

  Revision 1.9  2002/05/13 08:01:11  pouaite
  bugfix (à tester) compil sous cygwin

  Revision 1.8  2002/05/12 22:06:27  pouaite
  grosses modifs dans http.c

  Revision 1.7  2002/02/26 09:18:23  pouaite
  bugfixes divers

  Revision 1.6  2002/02/02 23:49:17  pouaite
  plop

  Revision 1.5  2002/01/20 20:53:22  pouaite
  bugfix configure.in && http_win.c pour cygwin + 2-3 petis trucs

  Revision 1.4  2002/01/20 00:37:06  pouaite
  bugfix qui permet d'utiliser l'option 'http.proxy_use_nocache:' sur les horribles proxy transparents

  Revision 1.3  2002/01/10 09:03:06  pouaite
  integration du patch de glandium (requetes http/1.1 avec header 'If-Modified-Since' --> coincoin plus gentil avec dacode)

  Revision 1.2  2001/12/02 18:26:06  pouaite
  modif http (affreux hack pr ispell + http.c fait maintenant un #include de http_unix/win.c )

*/




/* vole dans une mailing liste (je sais plus laquelle) */

static int
net_tcp_connect_with_timeout (SOCKET fd, struct sockaddr *sock, int salen, int timeout_secs)
{
  struct timeval timeout;
  fd_set write_fds;
  unsigned long argp;
  int error;
  int res;

  assert(fd != INVALID_SOCKET);
  
  /*
   * Set the socket to be non-blocking so that connect()
   * doesn't block.
   */
  argp=1;
  if (ioctlsocket (fd, FIONBIO, &argp) == SOCKET_ERROR)
    return -1;

  /*
   * Setup the connection timeout.
   */
  //printf("timeout=%d sec\n", timeout_secs);
  timeout.tv_sec = timeout_secs;
  timeout.tv_usec = 0;
  
  while (1) {
    
    /*
     * Try to connect.
     */
    if (connect (fd, sock, salen) < 0) {
      error = WSAGetLastError();
      if (error != WSAEWOULDBLOCK &&
	  error != WSAEISCONN
          ) {
	perror ("connect");
	return -1;
      }
      if (flag_cancel_task) return -1;
    } else {
      printf (_("Connected succesfully!\n"));
      return 0;
    }

    /*
     * We couldn't connect, so we select on the fd and
     * wait for the timer to run out, or for the fd to be
     * ready.
     */
    FD_ZERO (&write_fds);
    FD_SET (fd, &write_fds);

    
    while ((res = select (fd + 1, NULL, &write_fds, NULL, &timeout)) == SOCKET_ERROR) {
      if (WSAGetLastError() != WSAEINTR) {
	perror ("select");
	return -1;
      }
      if (flag_cancel_task) return -1;
    }
    if (res == 0) {
      printf(_("Connection timed out (timeout=%d sec)!\n"), timeout_secs);
      return -1;
    }
    if (res > 0) {
      return 0;
    }
  }
  // set socket to blocking
  argp=0;
  if (ioctlsocket (fd, FIONBIO, &argp) == SOCKET_ERROR) {
    printf (_("could not set socket to blocking\n"));
    return -1;
  }
  return 0;
}

void net_init (void) {
  WSADATA wsaData;

  global_http_download_cnt = 0;
  global_http_upload_cnt = 0;
  WSAStartup (MAKEWORD (2, 0), &wsaData);
}

int net_close (SOCKET fd) {     
  return closesocket (fd);   
}

char *
get_host_ip_str_nonbloq(const char *hostname, int port) {
  printf("todo: s'arranger pour faire marcher le gethostbyname non bloquant sous win \n(il suffit de foutre tout ça dans un fichier séparé de http.c, pour ne pas\n batailler avec le #include \"windows.h\"\n");
  return get_host_ip_str_bloq(hostname, port);
}
