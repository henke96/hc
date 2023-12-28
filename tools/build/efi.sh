#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

build() {
    out_dir="$root_dir/../hc-out/$out_path/$ARCH"
    mkdir -p "$out_dir"

    eval "set -- $FLAGS $1"
    if test -n "$ASSEMBLY"; then
        "$root_dir/cc_pe.sh" $debug_flags -S -o "$out_dir/debug-$source_name.efi.s" "$source" "$@"
        "$root_dir/cc_pe.sh" $release_flags -S -o "$out_dir/$source_name.efi.s" "$source" "$@"
    fi
    "$root_dir/cc_pe.sh" $debug_flags -o "$out_dir/debug-$source_name.efi" "$source" "$@"
    "$root_dir/cc_pe.sh" $release_flags -o "$out_dir/$source_name.efi" "$source" "$@"

    if test -z "$NO_ANALYSIS"; then
        "$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$source" "$@"
        "$root_dir/cc_pe.sh" $release_flags $analyse_flags "$source" "$@"
    fi
}

source="$1"
source_name="${1##*/}"
source_name="${source_name%.*}"
out_path="$2"
ext="$3"

analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
common_flags="-Wl,-subsystem,efi_application"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -Dhc_DEBUG"
release_flags="$common_flags -fomit-frame-pointer -s -Os"

if test -z "$NO_X86_64"; then export ARCH="x86_64"; build "$FLAGS_X86_64"; fi
if test -z "$NO_AARCH64"; then export ARCH="aarch64"; build "$FLAGS_AARCH64"; fi
