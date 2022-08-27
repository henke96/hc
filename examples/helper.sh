#!/bin/sh
set -e

if test -z "$1"
then
    echo "Usage: $0 PROJECT_PATH"
    exit 1
fi

OBJCOPY="${OBJCOPY:-llvm-objcopy}"

script_dir="$(dirname $0)"
debug_flags="-fsanitize-undefined-trap-on-error -fsanitize=undefined -g -O2 $FLAGS"
release_flags="-Ddebug_NDEBUG -O2 -s $FLAGS"
"$script_dir/../cc_elf.sh" $debug_flags -S -o "$1/debug.bin.s" "$1/main.c" $LFLAGS
"$script_dir/../cc_elf.sh" $debug_flags -o "$1/debug.bin" "$1/main.c" $LFLAGS
"$script_dir/../cc_elf.sh" $release_flags -S -o "$1/release.bin.s" "$1/main.c" $LFLAGS
"$script_dir/../cc_elf.sh" $release_flags -o "$1/release.bin" "$1/main.c" $LFLAGS
$OBJCOPY --strip-sections "$1/release.bin" || true
