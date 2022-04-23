#!/bin/sh
set -e
script_dir="$(dirname $0)"
flags="$(cat $script_dir/flags)"
CC="${CC:-clang}"
LD="${LD:-lld}"
ARCH="${ARCH:-x86_64}"
FORMAT="${FORMAT:-elf}"
"$CC" $flags -target $ARCH-unknown-linux-$FORMAT -fuse-ld="$LD" -Wl,--gc-sections -Wl,--build-id=none "$@"
