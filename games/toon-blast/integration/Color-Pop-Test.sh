#!/bin/sh
BASE=/sdcard/fmg-test
[ -d "$BASE" ] || exit 1
mkdir -p "$BASE/saves" || exit 1
exec >> "$BASE/color-pop-teste.log" 2>&1
printf '\n=== Color Pop native Ports test ===\n'
date
export LD_LIBRARY_PATH="/sdcard/simulator/3168/lib_3168:/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export FMG_SAVE_DIR="$BASE/saves"
export FMG_FULLSCREEN=1
[ ! -f "$BASE/controls.sh" ] || . "$BASE/controls.sh"
"$BASE/bin/color-pop-test"
