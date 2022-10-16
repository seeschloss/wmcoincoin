bin_PROGRAMS = @WMCOINCOIN_PLAYER@
EXTRA_PROGRAMS = wmcoincoin_player

LIBS = `pkg-config --libs imlib2`  `pkg-config --libs x11`

wmcoincoin_player_SOURCES = fmalloc.c giffunc.c gifread.c wmcoincoin_player.c gif.h

AM_CPPFLAGS = -I. `pkg-config --cflags imlib2`

