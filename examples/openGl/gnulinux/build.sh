#!/bin/sh
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

FLAGS="-l:libc.so.6 -l:libdl.so.2 $FLAGS" "$root_dir/tools/build/gnuelf.sh" "$script_dir" openGl
