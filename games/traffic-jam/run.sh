#!/bin/sh
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
exec "$ROOT/build/traffic-jam/traffic-jam" "$@"
