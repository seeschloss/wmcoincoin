# wmcoincoin (pinnipede + palmipede) on native Wayland, via libxwl.
#   make            -> ./wmcoincoin-wl
# Build deps (Debian/Ubuntu): gcc make pkg-config libwayland-dev wayland-protocols wayland-utils
#   libxkbcommon-dev libcairo2-dev libfontconfig-dev libfreetype-dev libcurl4-openssl-dev
#   libx11-dev libxmu-dev xorgproto   (X11/Xmu: *headers only* are used, nothing links against X)
CC      ?= gcc
SRCDIR  := wmcoincoin/src
BLD     := build
LIBXWL  := libxwl/libxwl.a
PKGS    := wayland-client cairo cairo-ft xkbcommon fontconfig freetype2 libcurl

# every file except the GTK config tool (wmccc*) and legacy/portability leftovers
SRCS := $(filter-out wmccc% http_unix.c http_win.c inet_aton.c,$(notdir $(wildcard $(SRCDIR)/*.c)))
OBJS := $(addprefix $(BLD)/,$(SRCS:.c=.o))

CFLAGS += -O2 -g -std=gnu99 -w -DHAVE_CONFIG_H -DXLIB_ILLEGAL_ACCESS \
          -I$(SRCDIR) -Iwmcoincoin -Ilibxwl $(shell pkg-config --cflags $(PKGS))
LDLIBS := $(shell pkg-config --libs $(PKGS)) -lm -lpthread

wmcoincoin-wl: $(OBJS) | libxwl-build
	$(CC) -o $@ $(OBJS) $(LIBXWL) $(LDLIBS)

libxwl-build:
	$(MAKE) -C libxwl

$(LIBXWL): libxwl-build

$(BLD)/%.o: $(SRCDIR)/%.c | $(BLD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BLD):
	mkdir -p $@

clean:
	rm -rf $(BLD) wmcoincoin-wl
	$(MAKE) -C libxwl clean
.PHONY: clean libxwl-build
