#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/.."

fails="$(find "$(cd -- "$root_dir" && pwd)" -name "build.sh" -type f -exec sh -c 'echo "$1"; "$1" || echo x >&3' sh {} \; 3>&1 >&2)"
test -z "$fails"

arch="$(uname -m)"

case "$(uname)" in
    FreeBSD)
    abi=freebsd14
    ;;
    Linux)
    abi=linux
    ;;
    *)
    exit 1
    ;;
esac

if test -z "$NO_DEBUG"; then
    "$OUT/debug_$arch-${abi}_tests" "$@"
fi
"$OUT/$arch-${abi}_tests" "$@"
