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

"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/liblog.so.c" "$path/liblog.so"
"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libdl.so.c" "$path/libdl.so"
"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libandroid.so.c" "$path/libandroid.so"
"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libc.so.c" "$path/libc.so"

ABI="android" FLAGS="-fPIC -fpie -pie -Wl,-dynamic-linker=/system/bin/linker64 $("$root_dir/tools/shellUtil/shellescape.sh" "-L$path") $FLAGS" STRIP_OPT="--strip-all" "$script_dir/elf.sh" "$@"
