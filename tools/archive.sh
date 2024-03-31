#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/.."

test -n "$OUT" || { echo "Please set OUT"; exit 1; }

"$script_dir/tar/build.sh"
prefix="hc_$(date +%Y-%m-%d)"
"$OUT/tar" -o "$OUT/$prefix.tar" -r "$root_dir" -p "$prefix" -a .
rm "$OUT/tar"
