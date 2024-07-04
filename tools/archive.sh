#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/.."

prefix="hc_$(date +%Y-%m-%d)"
"$root_dir/tools/tar/tar.sh" -o "$OUT/$prefix.tar" -d "$prefix" -p "$prefix" -a "$root_dir"
