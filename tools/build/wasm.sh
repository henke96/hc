#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

source="$1"
source_name="${1##*/}"
source_name="${source_name%.*}"
out_path="$2"
ext="$3"

analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
debug_flags="-fsanitize-undefined-trap-on-error -fsanitize=undefined -g -Dhc_DEBUG"
release_flags="-fomit-frame-pointer -s -Os"

out_dir="$root_dir/../hc-out/$out_path"
mkdir -p "$out_dir"

eval "set -- $FLAGS"
if test -n "$ASSEMBLY"; then
    "$root_dir/cc_wasm.sh" $debug_flags -S -o "$out_dir/debug-$source_name.wasm.s" "$source" "$@"
    "$root_dir/cc_wasm.sh" $release_flags -S -o "$out_dir/$source_name.wasm.s" "$source" "$@"
fi
"$root_dir/cc_wasm.sh" $debug_flags -o "$out_dir/debug-$source_name.wasm" "$source" "$@"
"$root_dir/cc_wasm.sh" $release_flags -o "$out_dir/$source_name.wasm" "$source" "$@"

if test -z "$NO_ANALYSIS"; then
    "$root_dir/cc_wasm.sh" $debug_flags $analyse_flags "$source" "$@"
    "$root_dir/cc_wasm.sh" $release_flags $analyse_flags "$source" "$@"
fi
