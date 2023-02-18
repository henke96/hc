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

"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/libc.so.6.c" "$path/libc.so.6"
"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/libdl.so.2.c" "$path/libdl.so.2"

# Note: -fPIC seems needed for undefined weak symbols to work.
FLAGS="-fPIC $("$root_dir/tools/shellUtil/shellescape.sh" "-L$path") $FLAGS" "$script_dir/elf.sh" "$@"
