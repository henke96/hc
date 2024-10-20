#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/core/shell/escape.sh"

test -n "$OUT" || { echo "Please set OUT"; exit 1; }
name=webPacker
opt=-Os

build() {
    export FLAGS_RELEASE="$opt"
    export FLAGS_DEBUG="-g"

    export ABI=linux
    if test -z "$NO_LINUX"; then
        export FLAGS=
        "$root_dir/core/builder.sh" "$script_dir/linux/$name.c"
        "$root_dir/core/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_$name"
    fi
    export ABI=freebsd14
    if test -z "$NO_FREEBSD"; then
        export FLAGS="-Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L $(escape "$OUT") -l:libc.so.7"
        "$root_dir/core/cc.sh" -fPIC -shared -Wl,--version-script="$root_dir/core/hc/freebsd/libc.so.7.map" -o "$OUT/libc.so.7" "$root_dir/core/hc/freebsd/libc.so.7.c"
        "$root_dir/core/builder.sh" "$script_dir/freebsd/$name.c"
        "$root_dir/core/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_$name"
    fi
    export ABI=windows-gnu
    if test -z "$NO_WINDOWS"; then
        export FLAGS="-Wl,-subsystem,console -L $(escape "$OUT") -l:ntdll.lib -l:kernel32.lib"
        export FLAGS_RELEASE="$opt -s"
        export FLAGS_DEBUG="-g -gcodeview -Wl,--pdb="
        "$root_dir/core/genlib.sh" "$OUT/ntdll.lib" "$root_dir/core/hc/windows/dll/ntdll.def"
        "$root_dir/core/genlib.sh" "$OUT/kernel32.lib" "$root_dir/core/hc/windows/dll/kernel32.def"
        "$root_dir/core/builder.sh" "$script_dir/windows/$name.exe.c"
    fi
}

if test -z "$NO_X86_64"; then export ARCH=x86_64; build; fi
if test -z "$NO_AARCH64"; then export ARCH=aarch64; build; fi
if test -z "$NO_RISCV64"; then
    (
        export ARCH=riscv64 NO_WINDOWS=1; build
    )
fi
