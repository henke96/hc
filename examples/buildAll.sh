#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"

fails="$(find "$script_dir" -name "build.sh" -type f -exec /bin/sh -c 'echo "$1"; "$1" || echo x >&3' sh {} \; 3>&1 >&2)"
test -z "$fails"
