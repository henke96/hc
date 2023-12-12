#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../../.."

export LINK_LIBC=1
"$root_dir/tools/build/freebsdelf.sh" "$script_dir" allocation
