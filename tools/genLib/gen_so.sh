#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 INPUT.c OUTPUT.so"
    exit 1
fi

CC="${CC:-clang}$LLVM"
LD="${LD:-ld.lld}$LLVM"
ARCH="${ARCH:-x86_64}"
"$CC" -target $ARCH-unknown-linux-elf --ld-path="$LD" -shared -nostdlib -o "$2" "$1"
