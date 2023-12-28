#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

build() {
    out_dir="$root_dir/../hc-out/$out_path/$ARCH"
    mkdir -p "$out_dir"

    if test -n "$LINK_LIBC"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/freebsd/libc.so.7.c" "$out_dir/libc.so.7"; fi
}

out_path="$2"

if test -n "$LINK_LIBC"; then FLAGS="-l:libc.so.7 $FLAGS"; fi

export ABI="freebsd14"
if test -z "$NO_X86_64"; then export ARCH="x86_64"; build; fi
if test -z "$NO_AARCH64"; then export ARCH="aarch64"; build; fi
if test -z "$NO_RISCV64"; then export ARCH="riscv64"; build; fi

export FLAGS="-fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic $FLAGS"
export STRIP_OPT="${STRIP_OPT:---strip-sections}"
"$script_dir/elf.sh" "$@"
