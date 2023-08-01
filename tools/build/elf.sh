#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

build() {
    mkdir -p "$prog_path/$ARCH"

    eval "set -- $("$script_dir/../shellUtil/escape.sh" "-L$prog_path/$ARCH") $FLAGS $1"
    if test -n "$ASSEMBLY"; then
        "$root_dir/cc_elf.sh" $debug_flags -S -o "$prog_path/$ARCH/debug.$prog_name.$ext.s" "$prog_path/$prog_name.c" "$@"
        "$root_dir/cc_elf.sh" $release_flags -S -o "$prog_path/$ARCH/$prog_name.$ext.s" "$prog_path/$prog_name.c" "$@"
    fi
    "$root_dir/cc_elf.sh" $debug_flags -o "$prog_path/$ARCH/debug.$prog_name.$ext" "$prog_path/$prog_name.c" "$@"
    "$root_dir/cc_elf.sh" $release_flags -o "$prog_path/$ARCH/$prog_name.$ext" "$prog_path/$prog_name.c" "$@"
    if test -n "$STRIP_OPT"; then
        "${llvm_prefix}llvm-objcopy" $STRIP_OPT "$prog_path/$ARCH/$prog_name.$ext"
    fi

    if test -z "$NO_ANALYSIS"; then
        "$root_dir/cc_elf.sh" $debug_flags $analyse_flags "$prog_path/$prog_name.c" "$@"
        "$root_dir/cc_elf.sh" $release_flags $analyse_flags "$prog_path/$prog_name.c" "$@"
    fi
}

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

prog_path="$1"
prog_name="$2"
ext="${3:-elf}"

analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
debug_flags="-fsanitize-undefined-trap-on-error -fsanitize=undefined -g -Dhc_DEBUG"
release_flags="-fomit-frame-pointer -s -Os"

if test -z "$NO_X86_64"; then export ARCH="x86_64"; build "$FLAGS_X86_64"; fi
if test -z "$NO_AARCH64"; then export ARCH="aarch64"; build "$FLAGS_AARCH64"; fi
if test -z "$NO_RISCV64"; then export ARCH="riscv64"; build "$FLAGS_RISCV64"; fi
