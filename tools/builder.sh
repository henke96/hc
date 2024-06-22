#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/.."

test -n "$OUT" || { echo "Please set OUT"; exit 1; }
test -n "$ARCH" || { echo "Please set ARCH"; exit 1; }
test -n "$ABI" || { echo "Please set ABI"; exit 1; }

source="$1"
source_name="${1##*/}"
source_name="${source_name%.*}"
full_name="$ARCH-${ABI}_$source_name"

FLAGS_RELEASE="$FLAGS $FLAGS_RELEASE"
FLAGS_DEBUG="-fsanitize-undefined-trap-on-error -fsanitize=undefined -Dhc_DEBUG=1 $FLAGS $FLAGS_DEBUG"

if test -z "$NO_DEBUG"; then
    eval "set -- $FLAGS_DEBUG"
    if test -n "$ASSEMBLY"; then
        "$root_dir/cc.sh" -S -o "$OUT/debug_$full_name.s" "$@" "$source"
    fi
    "$root_dir/cc.sh" -o "$OUT/debug_$full_name" "$@" "$source"
fi

eval "set -- $FLAGS_RELEASE"
if test -n "$ASSEMBLY"; then
    "$root_dir/cc.sh" -S -o "$OUT/$full_name.s" "$@" "$source"
fi
"$root_dir/cc.sh" -o "$OUT/$full_name" "$@" "$source"

analysis_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
if test -z "$NO_ANALYSIS"; then
    "$root_dir/cc.sh" $analysis_flags "$@" "$source"
    eval "set -- $FLAGS_DEBUG"
    "$root_dir/cc.sh" $analysis_flags "$@" "$source"
fi
