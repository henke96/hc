#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

FLAGS="-l:libdl.so -l:liblog.so" "$root_dir/tools/build/androidelf.sh" "$script_dir" android elf
