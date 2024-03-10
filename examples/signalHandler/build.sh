#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/tools/shell/escape.sh"

build() {
    export ABI=linux
    export FLAGS=
    export FLAGS_RELEASE="-Os"
    export FLAGS_DEBUG="-g"
    "$root_dir/tools/builder.sh" "$script_dir/signalHandler.c"
    "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_signalHandler"
}

if test -z "$NO_X86_64"; then export ARCH=x86_64; build; fi
if test -z "$NO_AARCH64"; then export ARCH=aarch64; build; fi
if test -z "$NO_RISCV64"; then export ARCH=riscv64; build; fi
