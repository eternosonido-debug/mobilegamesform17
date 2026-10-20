#!/bin/sh
# Local, bounded boot diagnostics. No UI, network, service or firmware changes.
TASK_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 0
LOG="$TASK_DIR/boot-trace.log"
{
    printf '\n=== FMG BOOT PROBE: %s ===\n' "${1:-unknown}"
    date
    printf 'kernel: '; uname -a
    for file in /etc/version /etc/os-release /etc/issue; do
        if [ -r "$file" ]; then
            printf '\n--- %s ---\n' "$file"
            head -c 4096 "$file"
        fi
    done
    printf '\n--- CPU description ---\n'
    if [ -r /proc/cpuinfo ]; then
        awk '/^(Processor|model name|CPU architecture|Hardware|Revision)[ \t]*:/ { print }' /proc/cpuinfo
    fi
    printf '\n--- SD/storage mounts ---\n'
    if [ -r /proc/mounts ]; then
        awk '$2 == "/" || $2 ~ /(sdcard|storage|userdata)/ { print }' /proc/mounts
    fi
    printf '\n--- Storage paths ---\n'
    ls -ld /storage /sdcard /mnt/sdcard /storage/.emulationstation /storage/.config/emulationstation 2>/dev/null
    printf '\n--- Menu process executables ---\n'
    for proc in /proc/[0-9]*/exe; do
        exe=$(readlink "$proc" 2>/dev/null) || continue
        case "$exe" in
            *emulationstation*|*emuelec*|*/game|*/game_3168)
                printf '%s -> %s\n' "$proc" "$exe"
                ;;
        esac
    done
    printf '\n--- Candidate system-list checksums ---\n'
    for cfg in /storage/.config/emulationstation/es_systems.cfg \
        /storage/.emulationstation/es_systems.cfg \
        /sdcard/.emulationstation/es_systems.cfg \
        /sdcard/.emulationstation/es_systems--.cfg \
        /sdcard/simulator/3168/.emulationstation/es_systems.cfg \
        /etc/emulationstation/es_systems.cfg /tmp/es_systems.cfg; do
        if [ -r "$cfg" ]; then
            cksum "$cfg"
        fi
    done
} >> "$LOG" 2>&1
exit 0
