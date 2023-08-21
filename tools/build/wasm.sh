#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
root_dir="$script_dir/../.."

prog_path="$1"
prog_name="$2"

analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
debug_flags="-fsanitize-undefined-trap-on-error -fsanitize=undefined -g -Dhc_DEBUG"
release_flags="-fomit-frame-pointer -s -Os"
eval "set -- $FLAGS"

if test -n "$ASSEMBLY"; then
    "$root_dir/cc_wasm.sh" $debug_flags -S -o "$prog_path/debug.$prog_name.wasm.s" "$prog_path/$prog_name.c" "$@"
    "$root_dir/cc_wasm.sh" $release_flags -S -o "$prog_path/$prog_name.wasm.s" "$prog_path/$prog_name.c" "$@"
fi
"$root_dir/cc_wasm.sh" $debug_flags -o "$prog_path/debug.$prog_name.wasm" "$prog_path/$prog_name.c" "$@"
"$root_dir/cc_wasm.sh" $release_flags -o "$prog_path/$prog_name.wasm" "$prog_path/$prog_name.c" "$@"

if test -z "$NO_ANALYSIS"; then
    "$root_dir/cc_wasm.sh" $debug_flags $analyse_flags "$prog_path/$prog_name.c" "$@"
    "$root_dir/cc_wasm.sh" $release_flags $analyse_flags "$prog_path/$prog_name.c" "$@"
fi
