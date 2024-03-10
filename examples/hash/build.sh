#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/tools/shell/escape.sh"

build() {
    export ABI=linux
    export FLAGS=
    export FLAGS_RELEASE="$opt"
    export FLAGS_DEBUG="-g"
    "$root_dir/tools/builder.sh" "$script_dir/linux/hash.c"
    "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_hash"

    if test "$ARCH" != "riscv64"; then
        export ABI=windows-gnu
        export FLAGS="-Wl,-subsystem,console -L $(escape "$OUT") -l:kernel32.lib"
        export FLAGS_RELEASE="$opt -s"
        export FLAGS_DEBUG="-g -gcodeview -Wl,--pdb="
        "$root_dir/genlib.sh" "$OUT/kernel32.lib" "$root_dir/src/hc/windows/dll/kernel32.def"
        "$root_dir/tools/builder.sh" "$script_dir/windows/hash.exe.c"
    fi
}

opt="-Os"

if test -z "$NO_X86_64"; then export ARCH=x86_64; build; fi
if test -z "$NO_AARCH64"; then export ARCH=aarch64; build; fi
if test -z "$NO_RISCV64"; then export ARCH=riscv64; build; fi
