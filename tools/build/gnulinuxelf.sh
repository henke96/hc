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

if test -n "$LINK_LIBC"; then FLAGS="-l:libc.so.6 $FLAGS"; fi
if test -n "$LINK_LIBDL"; then FLAGS="-l:libdl.so.2 $FLAGS"; fi

build() {
    mkdir -p "$path/$ARCH"
    if test -n "$LINK_LIBC"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/libc.so.6.c" "$path/$ARCH/libc.so.6"; fi
    if test -n "$LINK_LIBDL"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/libdl.so.2.c" "$path/$ARCH/libdl.so.2"; fi
}

if test -z "$NO_X86_64"; then ARCH="x86_64" build; fi
if test -z "$NO_AARCH64"; then ARCH="aarch64" build; fi
if test -z "$NO_RISCV64"; then ARCH="riscv64" build; fi

# Note: -fPIC seems needed for undefined weak symbols to work.
FLAGS="-fPIC $FLAGS" "$script_dir/elf.sh" "$@"
