#!/bin/sh
set -e
script_dir="$(dirname "$0")"
flags="$(cat "$script_dir/flags")"
ARCH="${ARCH:-x86_64}"
"${LLVM}clang" -I"$script_dir/src" $flags -target $ARCH-unknown-windows-gnu --ld-path="${LLVM}ld.lld" -Wl,--no-insert-timestamp -Wl,-e,_start "$@"
