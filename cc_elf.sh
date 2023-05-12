#!/bin/sh
set -e
script_dir="$(dirname "$0")"
flags="$(cat "$script_dir/flags")"
ARCH="${ARCH:-x86_64}"
ABI="${ABI:-linux-elf}"
"${LLVM}clang" -I"$script_dir/src" $flags -target $ARCH-unknown-$ABI --ld-path="${LLVM}ld.lld" -Wl,-dynamic-linker="" -Wl,--build-id=none "$@"
