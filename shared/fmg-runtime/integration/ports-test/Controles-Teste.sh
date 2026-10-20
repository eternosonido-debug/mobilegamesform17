#!/bin/sh
BASE=/sdcard/fmg-test
[ -x "$BASE/bin/new-blocks-test" ] || exit 1
mkdir -p "$BASE/input-test-saves" || exit 1
exec >> "$BASE/controles-teste.log" 2>&1
printf '\n=== FMG controls diagnostic ===\n'
date
export LD_LIBRARY_PATH="/sdcard/simulator/3168/lib_3168:/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export FMG_SAVE_DIR="$BASE/input-test-saves"
export FMG_FULLSCREEN=1
[ ! -f "$BASE/controls.sh" ] || . "$BASE/controls.sh"
"$BASE/bin/new-blocks-test" --game new-blocks --input-test
