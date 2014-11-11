## rcsid=$Id: Makefile.am,v 1.31 2005/09/25 12:08:55 pouaite Exp $
## ChangeLog:
##   $Log: Makefile.am,v $
##   Revision 1.31  2005/09/25 12:08:55  pouaite
##   ca marche encore ca ?
##
##   Revision 1.30  2005/06/11 22:47:39  pouaite
##   prout?
##
##   Revision 1.29  2005/02/22 18:44:42  pouaite
##   *** empty log message ***
##
##   Revision 1.28  2004/04/26 20:32:15  pouaite
##   roger demande le commit
##
##   Revision 1.27  2004/04/18 15:37:27  pouaite
##   un deux un deux
##
##   Revision 1.26  2004/02/29 15:01:18  pouaite
##   May the charles bronson spirit be with you
##
##   Revision 1.25  2003/08/26 21:50:47  pouaite
##   2.6.4b au mastic
##
##   Revision 1.24  2003/07/20 22:22:27  pouaite
##   ce commit est dedie a Pierre Tramo
##
##   Revision 1.23  2003/03/11 23:40:30  pouaite
##   rajout fake-getaddrinfo .. si ça compile alors cygwin remarche (pas testé)
##
##   Revision 1.22  2003/03/02 17:56:25  pouaite
##   wmcoincoin-2.4.4a.tar.gz is ready for distribution
##
##   Revision 1.21  2002/09/08 14:28:44  pouaite
##   bugfixes salutaires
##
##   Revision 1.20  2002/09/03 22:42:16  pouaite
##   coin
##
##   Revision 1.19  2002/08/31 21:26:46  pouaite
##   ajout du wmccc
##
##   Revision 1.18  2002/08/23 00:25:21  pouaite
##   oué
##
##   Revision 1.17  2002/08/22 20:04:09  pouaite
##   fin de la premiere manche contre gettextize
##
##   Revision 1.16  2002/08/22 18:50:38  pouaite
##   et là ça compile ?
##
##   Revision 1.15  2002/08/17 18:33:37  pouaite
##   grosse commition
##
##   Revision 1.14  2002/06/23 14:01:36  pouaite
##   ouups, j'avais flingué les modifs depuis la v2.3.8b
##
##   Revision 1.13  2002/06/23 10:44:05  pouaite
##   i18n-isation of the coincoin(kwakkwak), thanks to the incredible jjb !
##
##   Revision 1.11  2002/06/01 20:21:18  pouaite
##   version 2.3.8a
##
##   Revision 1.10  2002/06/01 17:54:04  pouaite
##   nettoyage
##
##   Revision 1.9  2002/05/29 22:38:12  pouaite
##   bidouilles dans configure.in et cie
##
##   Revision 1.8  2002/05/25 10:53:05  pouaite
##   aujourd'hui c'est la saint beugue
##
##   Revision 1.7  2002/04/15 19:56:38  pouaite
##   v2.3.7a
##
##   Revision 1.6  2002/04/13 11:55:19  pouaite
##   fix kde3 + deux trois conneries
##
##   Revision 1.5  2002/03/05 21:04:28  pouaite
##   bugfixes suite à l'upgrade de dlfp [et retour au comportement à l'ancienne du clic sur les horloges pour les moules ronchonnes]
##
##   Revision 1.4  2001/12/16 16:46:12  pouaite
##   Clippouille joins C0IN C0IN
##
##   Revision 1.3  2001/12/02 20:29:31  pouaite
##   rajout de 'options' et 'useragents' dans le Makefile.am ...
##
##   Revision 1.2  2001/12/02 18:32:05  pouaite
##   update suite à la modif sur http.c et a la generation automatique de src/default*.h
##
## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = 1.5 dist-bzip2
ACLOCAL_AMFLAGS = -I m4

MAINT_CHARSET = latin1

# pour faire plaisir à woof :°)
#prefix=$(DESTDIR)@prefix@

SUBDIRS = intl  m4  po src scripts platypus
EXTRA_DIST = config.rpath  MACOSX xpms/porte.h xpms/editwin_minib.xcf xpms/editwin_minib.xpm \
             xpms/coin.xpm  xpms/leds.h  xpms/leds.xcf \
             xpms/porte.xcf  xpms/weekday.xpm xpms/month.xpm \
             xpms/clock.xpm xpms/date.xpm xpms/led.xpm xpms/clippy.xpm xpms/icon.xpm \
	     xpms/duck.xcf xpms/duck.xpm xpms/mkduck.sh \
            xpms/badnews.xpm  xpms/bracket.xpm  xpms/logo.xpm xpms/totoz.xpm xpms/miniduck.xpm ./wmcoincoin.spec \
             ./options ./config.rpath wmccc.glade genoptlst.sh pixmaps/cbronson.jpg pixmaps/logoh.png pixmaps/logo.xpm

rpm-dist: dist
	cp $(PACKAGE)-$(VERSION).tar.gz $(HOME)/rpm/SOURCES
	-rm -f $(HOME)/rpm/SOURCES/$(PACKAGE)-$(VERSION).tar.bz2
	bzme $(HOME)/rpm/SOURCES/$(PACKAGE)-$(VERSION).tar.gz
	rpm -ba $(PACKAGE).spec

bin-dist: all
	cp src/wmcoincoin .
	cp src/wmccc .
	strip wmcoincoin
	strip wmccc
	tar czvf $(PACKAGE)-$(VERSION)-$(COIN_ARCH).tgz ChangeLog README options wmcoincoin wmccc

nightly: dist
	cp $(PACKAGE)-$(VERSION).tar.gz ../wmcoincoin-cvs.tar.gz
	(cd ..; ./up.sh ./wmcoincoin-cvs.tar.gz download/devel)

pkgdata_DATA = options README wmcoincoin.spec xpms/bracket.xpm  xpms/logo.xpm xpms/badnews.xpm \
	xpms/totoz.xpm pixmaps/cbronson.jpg pixmaps/logoh.png xpms/icon.xpm

CLEANFILES = stamp-h
