#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 PATH PROGRAM_NAME [EXT]"
    exit 1
fi

path="$1"
prog_name="$2"
ext="${3:-elf}"

STRIP_OPT="${STRIP_OPT:---strip-sections}"

script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

common_flags="-O2"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -g"
release_flags="$common_flags -Ddebug_NDEBUG -s"
eval "set -- $FLAGS"

"$root_dir/cc_elf.sh" $debug_flags -S -o "$path/debug.$prog_name.$ext.s" "$path/$prog_name.c" "$@"
"$root_dir/cc_elf.sh" $debug_flags -o "$path/debug.$prog_name.$ext" "$path/$prog_name.c" "$@"
"$root_dir/cc_elf.sh" $release_flags -S -o "$path/$prog_name.$ext.s" "$path/$prog_name.c" "$@"
"$root_dir/cc_elf.sh" $release_flags -o "$path/$prog_name.$ext" "$path/$prog_name.c" "$@"
"${LLVM}llvm-objcopy" $STRIP_OPT "$path/$prog_name.$ext"

# Static analysis.
analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
"$root_dir/cc_elf.sh" $debug_flags $analyse_flags "$path/$prog_name.c" "$@"
"$root_dir/cc_elf.sh" $release_flags $analyse_flags "$path/$prog_name.c" "$@"
