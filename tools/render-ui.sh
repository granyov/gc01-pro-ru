#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build
cc -std=c11 -O2 -DGC01 -DCH32 -DDISPLAY_320X240 -DDISPLAY_COLOR -DMCURENDERER_WITHOUT_BITMAP_SUPPORT \
  -DSTRINGS='"strings/ru.h"' -DFONT_SMALL='"fonts/font_small_ru_color_17_1bpp.h"' \
  -DFONT_MEDIUM='"fonts/font_medium_ru_color_24_1bpp.h"' -DFONT_LARGE='"fonts/font_large_color_80_1bpp.h"' \
  -DFONT_SYMBOLS='"fonts/font_symbols_color_1bpp.h"' -Ifirmware/lib/mcu-renderer \
  tests/render_ui.c firmware/lib/mcu-renderer/mcu-renderer.c firmware/src/ui/draw.c \
  firmware/src/ui/system.c firmware/src/ui/measurements.c firmware/src/ui/session.c \
  firmware/src/system/cstring.c firmware/src/system/cmath.c firmware/src/system/session.c \
  -lm -o build/render-ui
./build/render-ui
