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

common_flags="-Wl,-subsystem,efi_application -O2"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -g"
release_flags="$common_flags -Ddebug_NDEBUG -s"
eval "set -- $FLAGS"

"$root_dir/cc_pe.sh" $debug_flags -S -o "$path/$prog_name.debug.efi.s" "$path/$prog_name.c" "$@"
"$root_dir/cc_pe.sh" $debug_flags -o "$path/$prog_name.debug.efi" "$path/$prog_name.c" "$@"
"$root_dir/cc_pe.sh" $release_flags -S -o "$path/$prog_name.efi.s" "$path/$prog_name.c" "$@"
"$root_dir/cc_pe.sh" $release_flags -o "$path/$prog_name.efi" "$path/$prog_name.c" "$@"

# Static analysis.
analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
"$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$path/$prog_name.c" "$@"
"$root_dir/cc_pe.sh" $release_flags $analyse_flags "$path/$prog_name.c" "$@"
