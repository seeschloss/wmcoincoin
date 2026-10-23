## Process this file with automake to produce Makefile.in

## libxwl: the subset of Xlib that wmcoincoin's pinnipede/palmipede use, implemented on top of
## Wayland (xdg-shell, wl_shm, data-device, primary-selection), cairo, fontconfig and xkbcommon.
## It is only a convenience library, linked into src/wmcoincoin; nothing here is installed.
## See README.md in this directory.

noinst_LIBRARIES = libxwl.a

libxwl_a_SOURCES = xwl.c xwl_cursor.c xwl_draw.c xwl_input.c xwl_sel.c xwl_stubs.c \
	xwl.h xwl_priv.h xwl_colors.h

# Wayland protocol bindings, generated at build time by the wayland-scanner found by configure
nodist_libxwl_a_SOURCES = xdg-shell-protocol.c xdg-shell-client-protocol.h \
	primary-selection-protocol.c primary-selection-client-protocol.h

# XLIB_ILLEGAL_ACCESS: libxwl embeds struct _XDisplay, whose layout is otherwise private to Xlib
libxwl_a_CPPFLAGS = -DXLIB_ILLEGAL_ACCESS @XWL_CFLAGS@ @XHDR_CFLAGS@

# the cursor-shape-v1 protocol (wayland-protocols >= 1.32) is optional: without it no cursor is requested.  Its generated code refers to the tablet protocol's interfaces, hence the second one.
if HAVE_CURSOR_SHAPE
libxwl_a_CPPFLAGS += -DHAVE_CURSOR_SHAPE
nodist_libxwl_a_SOURCES += cursor-shape-protocol.c cursor-shape-client-protocol.h \
	tablet-protocol.c tablet-client-protocol.h
endif

BUILT_SOURCES = $(nodist_libxwl_a_SOURCES)
CLEANFILES = $(nodist_libxwl_a_SOURCES)

xdg_shell_xml = $(WAYLAND_PROTOCOLS_DATADIR)/stable/xdg-shell/xdg-shell.xml
primary_selection_xml = $(WAYLAND_PROTOCOLS_DATADIR)/unstable/primary-selection/primary-selection-unstable-v1.xml
cursor_shape_xml = $(WAYLAND_PROTOCOLS_DATADIR)/staging/cursor-shape/cursor-shape-v1.xml
tablet_xml = $(WAYLAND_PROTOCOLS_DATADIR)/unstable/tablet/tablet-unstable-v2.xml

xdg-shell-client-protocol.h: $(xdg_shell_xml)
	$(WAYLAND_SCANNER) client-header $(xdg_shell_xml) $@

xdg-shell-protocol.c: $(xdg_shell_xml)
	$(WAYLAND_SCANNER) private-code $(xdg_shell_xml) $@

primary-selection-client-protocol.h: $(primary_selection_xml)
	$(WAYLAND_SCANNER) client-header $(primary_selection_xml) $@

primary-selection-protocol.c: $(primary_selection_xml)
	$(WAYLAND_SCANNER) private-code $(primary_selection_xml) $@

cursor-shape-client-protocol.h: $(cursor_shape_xml)
	$(WAYLAND_SCANNER) client-header $(cursor_shape_xml) $@

cursor-shape-protocol.c: $(cursor_shape_xml)
	$(WAYLAND_SCANNER) private-code $(cursor_shape_xml) $@

tablet-client-protocol.h: $(tablet_xml)
	$(WAYLAND_SCANNER) client-header $(tablet_xml) $@

tablet-protocol.c: $(tablet_xml)
	$(WAYLAND_SCANNER) private-code $(tablet_xml) $@

# manual test programs and helpers (not built by default, see README.md)
EXTRA_DIST = README.md \
	tests/test1.c tests/test2.c tests/test_input.c tests/test_sel.c \
	tools/vptr.c tools/wlr-virtual-pointer-unstable-v1.xml
