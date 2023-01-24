#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 PATH PROGRAM_NAME"
    exit 1
fi

path="$1"

script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/dynamic/libc.so.6.c" "$path/libc.so.6"
"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/dynamic/libdl.so.2.c" "$path/libdl.so.2"

FLAGS="$("$script_dir/../shellUtil/shellescape.sh" "-L$path") $FLAGS" "$script_dir/elf.sh" "$@"
