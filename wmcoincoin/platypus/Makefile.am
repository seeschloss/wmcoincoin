bin_PROGRAMS = @WMCOINCOIN_PLAYER@
EXTRA_PROGRAMS = wmcoincoin_player

LDADD = $(IMLIB2_LIBS) $(x11_LIBS)

wmcoincoin_player_SOURCES = fmalloc.c giffunc.c gifread.c wmcoincoin_player.c gif.h

AM_CPPFLAGS = -I. $(IMLIB2_CFLAGS) $(x11_CFLAGS)
