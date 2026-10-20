#!/bin/sh
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
exec "$ROOT/build/new-blocks/new-blocks" "$@"
