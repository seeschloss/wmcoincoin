CC ?= gcc
CFLAGS += -DXLIB_ILLEGAL_ACCESS -O2 -g -Wall -fPIC $(shell pkg-config --cflags wayland-client cairo xkbcommon fontconfig cairo-ft)
LIBS = $(shell pkg-config --libs wayland-client cairo xkbcommon fontconfig) -lm
OBJS = xwl.o xwl_draw.o xwl_input.o xwl_sel.o xwl_stubs.o xdg-shell-protocol.o primary-selection-protocol.o
libxwl.a: $(OBJS)
	ar rcs $@ $(OBJS)
%.o: %.c xwl_priv.h xwl.h xdg-shell-client-protocol.h primary-selection-client-protocol.h
	$(CC) $(CFLAGS) -c $< -o $@
clean: ; rm -f *.o libxwl.a
