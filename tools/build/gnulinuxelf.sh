#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 PATH PROGRAM_NAME [EXT]"
    exit 1
fi

path="$1"

script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

if test -n "$LINK_LIBC"; then
    "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/libc.so.6.c" "$path/libc.so.6"
    FLAGS="-l:libc.so.6 $FLAGS"
fi
if test -n "$LINK_LIBDL"; then
    "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/libdl.so.2.c" "$path/libdl.so.2"
    FLAGS="-l:libdl.so.2 $FLAGS"
fi

# Note: -fPIC seems needed for undefined weak symbols to work.
FLAGS="-fPIC $("$root_dir/tools/shellUtil/shellescape.sh" "-L$path") $FLAGS" STRIP_OPT="--strip-sections" "$script_dir/elf.sh" "$@"
