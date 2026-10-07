# libxwl - Xlib API subset on Wayland (for porting wmcoincoin's pinnipede/palmipede)

Compile the application against the real X11 *headers* (`-DXLIB_ILLEGAL_ACCESS`), link `libxwl.a`
instead of libX11. No X server, no XWayland.

Build deps (Debian/Ubuntu): libwayland-dev wayland-protocols libxkbcommon-dev libcairo2-dev
libfontconfig-dev libfreetype-dev libx11-dev (headers only) pkg-config

    make                       # builds libxwl.a
    gcc -DXLIB_ILLEGAL_ACCESS -o test1 test1.c libxwl.a $(pkg-config --cflags --libs wayland-client cairo cairo-ft xkbcommon fontconfig) -lm

## Status
| Area | State | Tested with |
|---|---|---|
| Windows (xdg-shell), shm double buffering, resize | done | weston, sway |
| Drawing: lines/rects/points/polygons/arcs, dashed, clip rects/masks, XCopyArea, XImage | done | weston + sway (grim) |
| Pointer: enter/leave/motion/buttons/wheel, X-accurate state masks | done | sway + tools/vptr |
| Keyboard: xkbcommon, UTF-8 XLookupString, modifiers, key repeat, hotplug | done | sway + wtype |
| Fonts: fontcoincoin.c on fontconfig + cairo-ft (Latin-1 and UTF-8) | done | test2 |
| Selections: PRIMARY/CLIPBOARD via data-device + primary-selection | done | 2 processes in sway |
| XIM (kbcoincoin.c), main loop, dock/balloon/popup removal | TODO | |

fontcoincoin.c is a *project* file: copy it over wmcoincoin's src/fontcoincoin.c (it is built into
libxwl here only so the tests can link it, with -DXWL_FONT_TEST).

## Known deviations from X11
- XMoveWindow/positioning is a no-op; root-window capture (pseudo transparency) unsupported.
- Selection requests while owning the selection ourselves are short-circuited (set XWL_SEL_NOSHORTCUT=1
  to force the compositor round trip).

## Testing without a desktop
Headless sway + synthetic input:
    WLR_BACKENDS=headless WLR_LIBINPUT_NO_DEVICES=1 WLR_RENDERER=pixman sway -c cfg &
    wtype -s 1500 "text" -k Return          # keyboard (keep ONE wtype alive; each exit removes the device)
    tools/vptr "a 100 100" "b 272 1" "b 272 0" "s 10"   # persistent virtual pointer (build tools/vptr.c)
    grim out.png                            # screenshot of the compositor output
