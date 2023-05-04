#!/bin/sh
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."
FLAGS="-l:libc.so.7 $FLAGS" "$root_dir/tools/build/freebsdelf.sh" "$script_dir" freebsdHello
