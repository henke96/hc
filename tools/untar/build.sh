#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

test -n "$OUT" || { echo "Please set OUT"; exit 1; }

export ARCH="$(uname -m)"

case "$(uname)" in
    *)
    export ABI="linux"
    "$root_dir/cc.sh" -o "$OUT/untar" "$script_dir/linux/untar.c"
    ;;
esac
