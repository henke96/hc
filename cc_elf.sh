#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

flags="$(cat "$script_dir/flags")"
ABI="${ABI:-linux-elf}"
"${llvm_prefix}clang" -I"$script_dir/src" $flags -target $ARCH-unknown-$ABI --ld-path="${llvm_prefix}ld.lld" -Wl,-dynamic-linker="" -Wl,--build-id=none "$@"
