#!/bin/sh
set -eu
: "${M17_SDL_INCLUDE:?Informe os headers SDL2}"
: "${M17_SDL_LIBRARY:?Informe a biblioteca SDL2 exata do runtime}"
ZIG=${ZIG:-zig}
GAME=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$GAME/../.." && pwd)
OUT="$ROOT/build/m17"
mkdir -p "$OUT"
"$ZIG" c++ -target arm-linux-gnueabi.2.29 -mcpu=cortex_a9 -std=c++11 -O2 -g0 -I "$M17_SDL_INCLUDE" -I "$ROOT/shared/fmg-runtime/src" "$GAME/src/main.cpp" "$M17_SDL_LIBRARY" -Wl,--allow-shlib-undefined -o "$OUT/cooking"
printf 'Cooking ARM compilado; teste no hardware ainda pendente.\n'
