#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/src/shell/escape.sh"

test -n "$OUT" || { echo "Please set OUT"; exit 1; }
name=test
opt=-Os

build() {
    export FLAGS_RELEASE="$opt"
    export FLAGS_DEBUG="-g"

    export ABI=linux
    if test -z "$NO_LINUX"; then
        # OpenJDK complains if stack is executable.
        export FLAGS="-fPIC -shared -Wl,-znoexecstack"
        "$root_dir/tools/builder.sh" "$script_dir/linux/lib$name.so.c"
        "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_lib$name.so"
    fi
    export ABI=freebsd14
    if test -z "$NO_FREEBSD"; then
        export FLAGS="-fPIC -shared"
        "$root_dir/tools/builder.sh" "$script_dir/freebsd/lib$name.so.c"
        "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_lib$name.so"
    fi
    export ABI=windows-gnu
    if test -z "$NO_WINDOWS"; then
        export FLAGS="-fPIC -shared -L $(escape "$OUT") -l:kernel32.lib"
        export FLAGS_RELEASE="$opt -s"
        export FLAGS_DEBUG="-g -gcodeview -Wl,--pdb="
        "$root_dir/genlib.sh" "$OUT/kernel32.lib" "$root_dir/src/hc/windows/dll/kernel32.def"
        "$root_dir/tools/builder.sh" "$script_dir/windows/$name.dll.c"
    fi
}

if test -z "$NO_X86_64"; then export ARCH=x86_64; build; fi
if test -z "$NO_AARCH64"; then export ARCH=aarch64; build; fi
if test -z "$NO_RISCV64"; then
    (
        export ARCH=riscv64 NO_WINDOWS=1; build
    )
fi
