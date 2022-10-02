#!/bin/sh
set -e
script_dir="$(dirname $0)"
flags="$(cat $script_dir/flags)"
CC="${CC:-clang}"
LD="${LD:-lld}"
ARCH="${ARCH:-x86_64}"
ABI="${ABI:-elf}"
"$CC" -I$script_dir/src $flags -target $ARCH-unknown-linux-$ABI -fuse-ld="$LD" -Wl,--build-id=none "$@"
