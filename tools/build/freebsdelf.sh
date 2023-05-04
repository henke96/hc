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

"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/freebsd/libc.so.7.c" "$path/libc.so.7"

ABI="freebsd14" FLAGS="-fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic $("$root_dir/tools/shellUtil/shellescape.sh" "-L$path") $FLAGS" "$script_dir/elf.sh" "$@"
