#!/bin/sh
set -e
script_dir="$(dirname "$0")"

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

flags="$(cat "$script_dir/flags")"
"${llvm_prefix}clang" -I"$script_dir/src" $flags -target wasm32-unknown-unknown-unknown --ld-path="${llvm_prefix}wasm-ld" "$@"
