#!/bin/sh
set -eu
: "${M17_SDL_INCLUDE:?Informe os headers SDL2 do runtime}"
: "${M17_SDL_LIBRARY:?Informe a biblioteca SDL2 exata do aparelho}"
ZIG=${ZIG:-zig}
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUT="$ROOT/build/m17"
mkdir -p "$OUT"
set -- -target arm-linux-gnueabi.2.29 -mcpu=cortex_a9 -std=c++11 -O2 -g0 -I "$M17_SDL_INCLUDE" -I "$ROOT/shared/fmg-runtime/src"
for game in juices-sort new-blocks color-mix sudoku traffic-jam mahjong boca color-loop; do
 set -- "$@" -I "$ROOT/games/$game/src"
done
"$ZIG" c++ "$@" -c "$ROOT/shared/fmg-runtime/src/runtime_entry.cpp" -o "$OUT/runtime.o"
for game in juices-sort new-blocks color-mix sudoku traffic-jam mahjong boca color-loop; do
 "$ZIG" c++ "$@" "$ROOT/games/$game/src/main.cpp" "$OUT/runtime.o" "$M17_SDL_LIBRARY" -Wl,--allow-shlib-undefined -o "$OUT/$game"
done
printf 'Oito executaveis ARM criados. Teste no hardware ainda necessario.\n'
