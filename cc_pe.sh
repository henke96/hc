#!/bin/sh
set -e
script_dir="$(dirname "$0")"

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

flags="$(cat "$script_dir/flags")"
"${llvm_prefix}clang" -I"$script_dir/src" $flags -target $ARCH-unknown-windows-gnu --ld-path="${llvm_prefix}ld.lld" -Xlinker --no-insert-timestamp -Xlinker --entry=_start -Xlinker --stack=0x100000,0x20000 "$@"
