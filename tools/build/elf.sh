#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/tools/shell/escape.sh"

build() {
    out_dir="$root_dir/../hc-out/$out_path/$ARCH"
    mkdir -p "$out_dir"

    eval "set -- $(escape "-L$out_dir") $FLAGS $1"
    if test -n "$ASSEMBLY"; then
        "$root_dir/cc_elf.sh" $debug_flags -S -o "$out_dir/debug-$source_name$ext.s" "$source" "$@"
        "$root_dir/cc_elf.sh" $release_flags -S -o "$out_dir/$source_name$ext.s" "$source" "$@"
    fi
    "$root_dir/cc_elf.sh" $debug_flags -o "$out_dir/debug-$source_name$ext" "$source" "$@"
    "$root_dir/cc_elf.sh" $release_flags -o "$out_dir/$source_name$ext" "$source" "$@"
    if test -n "$STRIP_OPT"; then
        "${llvm_prefix}llvm-objcopy" $STRIP_OPT "$out_dir/$source_name$ext"
    fi

    if test -z "$NO_ANALYSIS"; then
        "$root_dir/cc_elf.sh" $debug_flags $analyse_flags "$source" "$@"
        "$root_dir/cc_elf.sh" $release_flags $analyse_flags "$source" "$@"
    fi
}

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

source="$1"
source_name="${1##*/}"
source_name="${source_name%.*}"
out_path="$2"
ext="$3"

analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
debug_flags="-fsanitize-undefined-trap-on-error -fsanitize=undefined -g -Dhc_DEBUG"
release_flags="-fomit-frame-pointer -s -Os"

if test -z "$NO_X86_64"; then export ARCH="x86_64"; build "$FLAGS_X86_64"; fi
if test -z "$NO_AARCH64"; then export ARCH="aarch64"; build "$FLAGS_AARCH64"; fi
if test -z "$NO_RISCV64"; then export ARCH="riscv64"; build "$FLAGS_RISCV64"; fi
