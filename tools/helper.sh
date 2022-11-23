#!/bin/sh
set -e

if test -z "$1"
then
    echo "Usage: $0 PROJECT_PATH"
    exit 1
fi

CC="${CC:-clang}$LLVM"

script_dir="$(dirname $0)"
flags="-fsanitize=undefined -fsanitize=address -Wall -Wextra -Wconversion -Wshadow -Wpadded -Werror $FLAGS"
$CC $flags -o "$1/release.bin" "$1/main.c"
