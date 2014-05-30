## rcsid=$Id: Makefile.am,v 1.27 2004/05/16 12:54:29 pouaite Exp $
## ChangeLog:
##   $Log: Makefile.am,v $
##   Revision 1.27  2004/05/16 12:54:29  pouaite
##   250c
##
##   Revision 1.26  2004/04/26 20:32:31  pouaite
##   roger demande le commit
##
##   Revision 1.25  2004/04/18 15:37:28  pouaite
##   un deux un deux
##
##   Revision 1.24  2004/03/07 13:51:12  pouaite
##   commit du dimanche
##
##   Revision 1.23  2004/02/29 19:01:26  pouaite
##   et hop
##
##   Revision 1.22  2004/02/29 15:01:19  pouaite
##   May the charles bronson spirit be with you
##
##   Revision 1.21  2003/06/29 23:58:35  pouaite
##   suppression de l'overrideredirect du palmi et ajout de pinnipede_totoz.c et wmcoincoin-totoz-get etc
##
##   Revision 1.20  2003/03/12 21:09:44  pouaite
##   mega patch de gle + micro fix du referer
##
##   Revision 1.19  2003/03/11 23:40:36  pouaite
##   rajout fake-getaddrinfo .. si ça compile alors cygwin remarche (pas testé)
##
##   Revision 1.18  2002/10/05 18:08:14  pouaite
##   ajout menu contextuel + fix de la coloration des boutons du wmccc
##
##   Revision 1.17  2002/09/21 22:51:01  pouaite
##   ajout du patch de shift pour le pdfm
##
##   Revision 1.16  2002/09/08 14:28:45  pouaite
##   bugfixes salutaires
##
##   Revision 1.15  2002/09/05 23:11:56  pouaite
##   <blog>ce soir g mangé une omelette</blog>
##
##   Revision 1.14  2002/08/31 21:26:46  pouaite
##   ajout du wmccc
##
##   Revision 1.13  2002/08/22 18:50:38  pouaite
##   et là ça compile ?
##
##   Revision 1.12  2002/08/21 01:11:48  pouaite
##   commit du soir, espoir
##
##   Revision 1.11  2002/08/17 18:33:38  pouaite
##   grosse commition
##
##   Revision 1.10  2002/06/23 10:44:05  pouaite
##   i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !
##
##   Revision 1.9  2002/05/29 22:38:12  pouaite
##   bidouilles dans configure.in et cie
##
##   Revision 1.8  2002/04/01 01:39:38  pouaite
##   grosse grosse commition (cf changelog)
##
##   Revision 1.7  2002/02/24 22:13:56  pouaite
##   modifs pour la v2.3.5 (selection, scrollcoin, plopification, bugfixes)
##
##   Revision 1.6  2002/01/31 11:15:42  pouaite
##   fix compilation
##
##   Revision 1.5  2002/01/18 00:28:42  pouaite
##   le ménage continue + grosses modifs (experimentales pour l'instant)
##
##   Revision 1.4  2002/01/16 21:27:35  pouaite
##   gros coup de balai dans wmcoincoin.c qui s'est du coup splitté en trois: wmcoincoin.c, dock.c et useragents_file.c
##
##   Revision 1.3  2002/01/13 15:19:00  pouaite
##   double patch: shift -> tribune.post_cmd et lordOric -> tribune.archive
##
##   Revision 1.2  2001/12/02 18:32:05  pouaite
##   update suite à la modif sur http.c et a la generation automatique de src/default*.h
##
## Process this file with automake to produce Makefile.in

localedir = $(datadir)/locale
LIBS = @LIBINTL@ @LIBS@

bin_PROGRAMS = wmcoincoin @WMCCC@
EXTRA_PROGRAMS = wmccc

# include dans http.c
EXTRA_wmcoincoin_SOURCES = http_win.c http_unix.c fake-getaddrinfo.c fake-getnameinfo.c inet_aton.c

#salete d'autoconf j'y comprends rien
DEFS = @DEFS@ -DWMCCDATADIR="\"$(pkgdatadir)\"" -DLOCALEDIR="\"$(localedir)\"" -I$(top_srcdir)/intl -I$(top_srcdir)

wmcoincoin_SOURCES = cc_queue.c plopup.c \
	md5.c md5.h base64.c base64.h \
        board_util.c board_util.h \
	xmlcoincoin.c xmlcoincoin.h \
	regexp.h regexp.c general.h \
	myprintf.c myprintf.h \
	raster.c raster.h \
	fontcoincoin.c fontcoincoin.h \
	kbcoincoin.c kbcoincoin.h \
	swallowcoincoin.c \
        troll_detector.c troll_data.h \
	pinnipede.c pinnipede_widgets.c pinnipede_pdfm.c pinnipede_totoz.c pinnipede.h \
	totoz_bookmark.c \
        palmipede.c \
	dock.c dock.h \
        board_priv.h board.c board_rss.c board_pop3.c \
	site.c site.h \
	balltrap.c balltrap.h \
        keylist.c keylist.h \
	scrollcoin.h scrollcoin.c \
	picohtml.c picohtml.h \
        msgbox.c balloon.c \
	wmcoincoin.c coincoin.h global.h \
        http.c http.h \
	coin_util.c coin_util.h \
        coin_xutil.c coin_xutil.h \
	prefs.c prefs.h options_list.h prefs_gestion.c\
	spell_coin.c spell_coin.h

wmcoincoin_CFLAGS = \
	@XFT_CFLAGS@

wmcoincoin_LDADD = \
	@XFT_LIBS@ -lm

wmccc_CFLAGS = \
	@GTK_CFLAGS@

wmccc_SOURCES = \
	wmccc.c wmccc.h wmccc_dialogs.c \
	wmccc_support.c wmccc_support.h \
	wmccc_interface.c wmccc_interface.h \
	wmccc_callbacks.c wmccc_callbacks.h \
	wmccc_save_prefs.c \
	myprintf.c myprintf.h \
	prefs.c prefs.h md5.h md5.c \
	coin_util.c coin_util.h \
	options_list.h \
	keylist.c keylist.h \
	fake-getaddrinfo.h fake-getnameinfo.h inet_aton.h

wmccc_LDADD = @GTK_LIBS@



#useragents_file.c

#defaultuseragents.h: ../useragents
#	cat ../useragents | sed -e '/^#%.*/d' | sed -e 's/\\/\\\\/g' | sed -e 's/	/\\t/g' | sed -e 's/^/fprintf(f,"/g' | sed -e 's/$$/\\n");/g' > ./defaultuseragents.h

#defaultoptionsfile.h: ../options
#	cat ../options | sed -e '/^#%.*/d' | sed -e 's/\\/\\\\/g' | sed -e 's/"/\\"/g' | sed -e 's/	/\\t/g' | sed -e 's/^/fprintf(f,"/g' | sed -e 's/$$/\\n");/g' | sed -e 's/%/%%/g' > ./defaultoptionsfile.h

options_list.h: ../options
	cd ..; /bin/sh ./genoptlst.sh

#useragents_file.o: defaultuseragents.h

#coincoin_prefs.o: defaultoptionsfile.h

install: install-am
# petite update merci anvil !
#	ln -sf $(DESTDIR)$(bindir)/wmcoincoin $(DESTDIR)$(bindir)/wmcoincoin-kde
#	(cd $(DESTDIR)$(bindir) ; ln -sf wmcoincoin wmcoincoin-kde)
