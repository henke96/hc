#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 PATH PROGRAM_NAME [EXT]"
    exit 1
fi

path="$1"

script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

if test -n "$LINK_LIBC"; then FLAGS="-l:libc.so.7 $FLAGS"; fi

build() {
    mkdir -p "$path/$ARCH"
    if test -n "$LINK_LIBC"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/freebsd/libc.so.7.c" "$path/$ARCH/libc.so.7"; fi
}

if test -z "$NO_X86_64"; then ARCH="x86_64" build; fi
if test -z "$NO_AARCH64"; then ARCH="aarch64" build; fi
if test -z "$NO_RISCV64"; then ARCH="riscv64" build; fi

ABI="freebsd14" FLAGS="-fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic $FLAGS" "$script_dir/elf.sh" "$@"
