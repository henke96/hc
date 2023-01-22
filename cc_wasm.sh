#!/bin/sh
set -e
script_dir="$(dirname "$0")"
flags="$(cat "$script_dir/flags")"
"${LLVM}clang" -I"$script_dir/src" $flags -target wasm32-unknown-unknown-unknown --ld-path="${LLVM}wasm-ld" "$@"
