#!/bin/sh
# Isolated native test launched through the vendor's existing Ports system.
# Extra arguments supplied by EmulationStation are intentionally ignored.
BASE=/sdcard/fmg-test
if [ ! -d "$BASE" ]; then
    exit 1
fi
mkdir -p "$BASE/saves" || exit 1
exec >> "$BASE/teste.log" 2>&1
printf '\n=== FMG Juices Sort isolated Ports test ===\n'
date
uname -a
if [ -r /etc/version ]; then cat /etc/version; fi
export LD_LIBRARY_PATH="/sdcard/simulator/3168/lib_3168:/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export FMG_SAVE_DIR="$BASE/saves"
export FMG_FULLSCREEN=1
if [ -f "$BASE/controls.sh" ]; then . "$BASE/controls.sh"; fi
printf 'Starting native ARM gnueabi build\n'
"$BASE/bin/juices-sort-test" --game juices-sort
result=$?
printf 'FMG test exited with status %s\n' "$result"
exit "$result"
