#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/core/shell/escape.sh"

test -n "$OUT" || { echo "Please set OUT"; exit 1; }
name=ssh
opt=-Os

build() {
    export FLAGS_RELEASE="$opt"
    export FLAGS_DEBUG="-g"

    export ABI=linux
    if test -z "$NO_LINUX"; then
        export FLAGS=
        "$root_dir/core/builder.sh" "$script_dir/$name.c"
        "$root_dir/core/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_$name"
    fi
}

if test -z "$NO_X86_64"; then export ARCH=x86_64; build; fi
if test -z "$NO_AARCH64"; then export ARCH=aarch64; build; fi
if test -z "$NO_RISCV64"; then export ARCH=riscv64; build; fi
