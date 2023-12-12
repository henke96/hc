#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

build() {
    mkdir -p "$prog_path/$ARCH"
    eval "set -- $FLAGS $1"

    if test -n "$ASSEMBLY"; then
        "$root_dir/cc_pe.sh" $debug_flags -S -o "$prog_path/$ARCH/debug.$prog_name.efi.s" "$prog_path/$prog_name.c" "$@"
        "$root_dir/cc_pe.sh" $release_flags -S -o "$prog_path/$ARCH/$prog_name.efi.s" "$prog_path/$prog_name.c" "$@"
    fi
    "$root_dir/cc_pe.sh" $debug_flags -o "$prog_path/$ARCH/debug.$prog_name.efi" "$prog_path/$prog_name.c" "$@"
    "$root_dir/cc_pe.sh" $release_flags -o "$prog_path/$ARCH/$prog_name.efi" "$prog_path/$prog_name.c" "$@"

    if test -z "$NO_ANALYSIS"; then
        "$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$prog_path/$prog_name.c" "$@"
        "$root_dir/cc_pe.sh" $release_flags $analyse_flags "$prog_path/$prog_name.c" "$@"
    fi
}

prog_path="$1"
prog_name="$2"

analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
common_flags="-Wl,-subsystem,efi_application"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -Dhc_DEBUG"
release_flags="$common_flags -fomit-frame-pointer -s -Os"

if test -z "$NO_X86_64"; then export ARCH="x86_64"; build "$FLAGS_X86_64"; fi
if test -z "$NO_AARCH64"; then export ARCH="aarch64"; build "$FLAGS_AARCH64"; fi
