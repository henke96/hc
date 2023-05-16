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

if test -n "$LINK_LIBLOG"; then
    "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/liblog.so.c" "$path/liblog.so"
    FLAGS="-l:liblog.so $FLAGS"
fi
if test -n "$LINK_LIBDL"; then
    "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libdl.so.c" "$path/libdl.so"
    FLAGS="-l:libdl.so $FLAGS"
fi
if test -n "$LINK_LIBANDROID"; then
    "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libandroid.so.c" "$path/libandroid.so"
    FLAGS="-l:libandroid.so $FLAGS"
fi
if test -n "$LINK_LIBC"; then
    "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libc.so.c" "$path/libc.so"
    FLAGS="-l:libc.so $FLAGS"
fi

ABI="linux-android26" FLAGS="-fPIC -fpie -pie -Wl,-dynamic-linker=/system/bin/linker64 $("$root_dir/tools/shellUtil/shellescape.sh" "-L$path") $FLAGS" STRIP_OPT="--strip-all" "$script_dir/elf.sh" "$@"
