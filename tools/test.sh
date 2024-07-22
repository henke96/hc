#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/.."

fails="$(find "$(cd -- "$root_dir" && pwd)" -name "build.sh" -type f -exec sh -c 'echo "$1"; "$1" || echo x >&3' sh {} \; 3>&1 >&2)"
test -z "$fails"

. "$root_dir/tools/hostbuild.sh"

if test -z "$NO_DEBUG"; then
    "$OUT/debug_$HOST_ARCH-${HOST_ABI}_tests" "$@"
fi
"$OUT/$HOST_ARCH-${HOST_ABI}_tests" "$@"
