#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
root_dir="$script_dir/../../.."

export LINK_LIBC=1 LINK_LIBDL=1
"$root_dir/tools/build/gnulinuxelf.sh" "$script_dir" openGl
