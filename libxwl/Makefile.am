## Process this file with automake to produce Makefile.in

## libxwl: the subset of Xlib that wmcoincoin's pinnipede/palmipede use, implemented on top of
## Wayland (xdg-shell, wl_shm, data-device, primary-selection), cairo, fontconfig and xkbcommon.
## It is only a convenience library, linked into src/wmcoincoin; nothing here is installed.
## See README.md in this directory.

noinst_LIBRARIES = libxwl.a

libxwl_a_SOURCES = xwl.c xwl_draw.c xwl_input.c xwl_sel.c xwl_stubs.c \
	xwl.h xwl_priv.h xwl_colors.h

# Wayland protocol bindings, generated at build time by the wayland-scanner found by configure
nodist_libxwl_a_SOURCES = xdg-shell-protocol.c xdg-shell-client-protocol.h \
	primary-selection-protocol.c primary-selection-client-protocol.h

# XLIB_ILLEGAL_ACCESS: libxwl embeds struct _XDisplay, whose layout is otherwise private to Xlib
libxwl_a_CPPFLAGS = -DXLIB_ILLEGAL_ACCESS @XWL_CFLAGS@ @XHDR_CFLAGS@

BUILT_SOURCES = $(nodist_libxwl_a_SOURCES)
CLEANFILES = $(nodist_libxwl_a_SOURCES)

xdg_shell_xml = $(WAYLAND_PROTOCOLS_DATADIR)/stable/xdg-shell/xdg-shell.xml
primary_selection_xml = $(WAYLAND_PROTOCOLS_DATADIR)/unstable/primary-selection/primary-selection-unstable-v1.xml

xdg-shell-client-protocol.h: $(xdg_shell_xml)
	$(WAYLAND_SCANNER) client-header $(xdg_shell_xml) $@

xdg-shell-protocol.c: $(xdg_shell_xml)
	$(WAYLAND_SCANNER) private-code $(xdg_shell_xml) $@

primary-selection-client-protocol.h: $(primary_selection_xml)
	$(WAYLAND_SCANNER) client-header $(primary_selection_xml) $@

primary-selection-protocol.c: $(primary_selection_xml)
	$(WAYLAND_SCANNER) private-code $(primary_selection_xml) $@

# manual test programs and helpers (not built by default, see README.md)
EXTRA_DIST = README.md \
	tests/test1.c tests/test2.c tests/test_input.c tests/test_sel.c \
	tools/vptr.c tools/wlr-virtual-pointer-unstable-v1.xml
