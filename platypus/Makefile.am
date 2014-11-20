bin_PROGRAMS = @WMCOINCOIN_PLAYER@
EXTRA_PROGRAMS = wmcoincoin_player

LIBS = `imlib2-config --libs`  `pkg-config --libs x11`

wmcoincoin_player_SOURCES = fmalloc.c giffunc.c gifread.c wmcoincoin_player.c gif.h

AM_CPPFLAGS = -I. `imlib2-config --cflags`

