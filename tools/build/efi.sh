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

common_flags="-Wl,-subsystem,efi_application"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined"
release_flags="$common_flags -fomit-frame-pointer -Ddebug_NDEBUG -s -Os"

build() {
    mkdir -p "$path/$ARCH"
    eval "set -- $FLAGS $1"

    if test -n "$ASSEMBLY"; then
        "$root_dir/cc_pe.sh" $debug_flags -S -o "$path/$ARCH/debug.$prog_name.efi.s" "$path/$prog_name.c" "$@"
        "$root_dir/cc_pe.sh" $release_flags -S -o "$path/$ARCH/$prog_name.efi.s" "$path/$prog_name.c" "$@"
    fi
    "$root_dir/cc_pe.sh" $debug_flags -o "$path/$ARCH/debug.$prog_name.efi" "$path/$prog_name.c" "$@"
    "$root_dir/cc_pe.sh" $release_flags -o "$path/$ARCH/$prog_name.efi" "$path/$prog_name.c" "$@"

    if test -z "$NO_ANALYSIS"; then
        analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
        "$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$path/$prog_name.c" "$@"
        "$root_dir/cc_pe.sh" $release_flags $analyse_flags "$path/$prog_name.c" "$@"
    fi
}

if test -z "$NO_X86_64"; then ARCH="x86_64" build "$FLAGS_X86_64"; fi
if test -z "$NO_AARCH64"; then ARCH="aarch64" build "$FLAGS_AARCH64"; fi
