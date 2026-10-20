#!/bin/sh
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
exec "$ROOT/build/color-loop/color-loop" "$@"
