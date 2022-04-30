#!/bin/sh
set -e

if test -z "$1"
then
    echo "Usage: $0 LIB_NAME PROJECT_PATH"
    exit 1
fi

script_dir="$(dirname $0)"
CC="${CC:-clang}"
ARCH="${ARCH:-x86_64}"
"$CC" -shared -target $ARCH-unknown-linux-elf -o "$2/$1" "$script_dir/$1.c"
