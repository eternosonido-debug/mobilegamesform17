# Isolated test through the vendor's existing Ports category

This test adds no system configuration and does not modify startup scripts.
The active vendor list already defines Ports at `/storage/.config/emuelec/ports`,
with `.sh` scripts executed by `/usr/bin/bash`. The inspected filesystem contains
that interpreter, and the boot trace confirmed `/storage` resolves to the SD.

Installed files:
- SD `.config/emuelec/ports/Juices-Sort-Test.sh`
- SD `fmg-test/bin/juices-sort-test` (the ARM gnueabi executable)
- SD `fmg-test/controls.sh`, `saves/`, LICENSE and C++ runtime notices

The script opens Juices Sort directly and records start, system version, SDL
errors and exit status to `fmg-test/teste.log`. It ignores additional arguments
from the vendor's EmulationStation launch template. Controls come from the SD's
input maps; physical behavior is not confirmed. SELECT/C, Escape or Right Shift
requests save and exit. The software remains a candidate pending hardware test.

Remove this single script and `fmg-test/` to undo. Preserve `saves/` first if useful.
