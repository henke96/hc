#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
root_dir="$script_dir/../../.."

export FLAGS="-shared -fPIC $FLAGS"
"$root_dir/tools/build/linuxelf.sh" "$script_dir" libtest so
