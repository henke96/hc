#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 PATH PROGRAM_NAME"
    exit 1
fi

path="$1"
prog_name="$2"

script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

debug_flags="-fsanitize-undefined-trap-on-error -fsanitize=undefined -g -O2"
release_flags="-Ddebug_NDEBUG -O2 -s"
eval "set -- $FLAGS"

"$root_dir/cc_elf.sh" $debug_flags -S -o "$path/debug.$prog_name.elf.s" "$path/$prog_name.c" "$@"
"$root_dir/cc_elf.sh" $debug_flags -o "$path/debug.$prog_name.elf" "$path/$prog_name.c" "$@"
"$root_dir/cc_elf.sh" $release_flags -S -o "$path/$prog_name.elf.s" "$path/$prog_name.c" "$@"
"$root_dir/cc_elf.sh" $release_flags -o "$path/$prog_name.elf" "$path/$prog_name.c" "$@"
"${LLVM}llvm-objcopy" --strip-sections "$path/$prog_name.elf"

# Static analysis.
analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
"$root_dir/cc_elf.sh" $debug_flags $analyse_flags "$path/$prog_name.c" "$@"
"$root_dir/cc_elf.sh" $release_flags $analyse_flags "$path/$prog_name.c" "$@"
