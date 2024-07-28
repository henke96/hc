#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/.."

fail="$(find "$(cd -- "$root_dir" && pwd)" -name "build.sh" -type f -exec sh -c 'echo "$1"; "$1" || echo x >&3' sh {} \; 3>&1 >&2)"
test -z "$fail"

. "$root_dir/src/shell/hostarch.sh"

case "$(uname)" in
    FreeBSD)
    hostabi=freebsd14
    ;;
    Linux)
    hostabi=linux
    ;;
    *)
    exit 1
    ;;
esac

if test -z "$NO_DEBUG"; then
    "$OUT/debug/$hostarch-${hostabi}_tests" "$@"
fi
"$OUT/$hostarch-${hostabi}_tests" "$@"
