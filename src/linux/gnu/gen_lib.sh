#!/bin/sh
set -e

if test -z "$1"
then
    echo "Usage: $0 LIB_NAME PROJECT_PATH"
    exit 1
fi

script_dir="$(dirname $0)"
CC="${CC:-clang}"
LD="${LD:-lld}"
ARCH="${ARCH:-x86_64}"
"$CC" -target $ARCH-unknown-linux-elf -fuse-ld="$LD" -shared -nostdlib -o "$2/$1" "$script_dir/$1.c"
