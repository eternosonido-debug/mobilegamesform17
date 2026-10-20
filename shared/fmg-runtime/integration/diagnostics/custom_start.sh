#!/bin/sh
# Additive EmuELEC hook, based on the custom_start hook referenced on this SD.
for root in /storage /sdcard /mnt/sdcard; do
    probe="$root/fake-mobile-games/diagnostics/boot-probe.sh"
    if [ -f "$probe" ]; then
        /bin/sh "$probe" "emuelec-custom-${1:-unknown}" || :
        if [ "${1:-}" != before ]; then
            (sleep 12; /bin/sh "$probe" "emuelec-custom-after-menu") </dev/null >/dev/null 2>&1 &
        fi
        break
    fi
done
exit 0
