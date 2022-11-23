#!/bin/sh
set -e
script_dir="$(dirname $0)"
flags="$(cat $script_dir/flags)"
CC="${CC:-clang}$LLVM"
LD="${LD:-ld.lld}$LLVM"
ARCH="${ARCH:-x86_64}"
ABI="${ABI:-elf}"
"$CC" -I$script_dir/src $flags -target $ARCH-unknown-linux-$ABI --ld-path="$LD" -Wl,--build-id=none "$@"
