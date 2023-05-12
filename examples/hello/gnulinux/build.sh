#!/bin/sh
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

FLAGS="-l:libc.so.6 $FLAGS" "$root_dir/tools/build/gnulinuxelf.sh" "$script_dir" hello
