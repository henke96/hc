#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../../.."

export LINK_KERNEL32=1
export FLAGS="-shared -fPIC $FLAGS"
"$root_dir/tools/build/exe.sh" "$script_dir" test dll
