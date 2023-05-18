#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

export FLAGS="-shared -fPIC $FLAGS"
"$root_dir/tools/build/elf.sh" "$script_dir" libtest so
