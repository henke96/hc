#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

build() {
    mkdir -p "$prog_path/$ARCH"
    if test -n "$LINK_LIBC"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/freebsd/libc.so.7.c" "$prog_path/$ARCH/libc.so.7"; fi
}

prog_path="$1"

if test -n "$LINK_LIBC"; then FLAGS="-l:libc.so.7 $FLAGS"; fi

if test -z "$NO_X86_64"; then ARCH="x86_64" build; fi
if test -z "$NO_AARCH64"; then ARCH="aarch64" build; fi
if test -z "$NO_RISCV64"; then ARCH="riscv64" build; fi

export ABI="freebsd14"
export FLAGS="-fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic $FLAGS"
"$script_dir/elf.sh" "$@"
