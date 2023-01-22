#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 PATH PROGRAM_NAME"
    exit 1
fi

script_dir="$(dirname $0)"
root_dir="$script_dir/../.."

"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/dynamic/libc.so.6.c" "$1/libc.so.6"
"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/dynamic/libdl.so.2.c" "$1/libdl.so.2"

LFLAGS="-L$1 $LFLAGS" "$script_dir/elf.sh" "$@"
