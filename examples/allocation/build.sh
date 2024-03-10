#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/tools/shell/escape.sh"

build() {
    export FLAGS_RELEASE="-Os"
    export FLAGS_DEBUG="-g"

    export ABI=linux
    export FLAGS=
    "$root_dir/tools/builder.sh" "$script_dir/linux/allocation.c"
    "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_allocation"

    export ABI=freebsd14
    export FLAGS="-Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L $(escape "$OUT") -l:libc.so.7"
    "$root_dir/cc.sh" -fPIC -shared -Wl,--version-script="$root_dir/src/hc/freebsd/libc.so.7.map" -o "$OUT/libc.so.7" "$root_dir/src/hc/freebsd/libc.so.7.c"
    "$root_dir/tools/builder.sh" "$script_dir/freebsd/allocation.c"
    "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_allocation"

    if test "$ARCH" != "riscv64"; then
        export ABI=windows-gnu
        export FLAGS="-Wl,-subsystem,console -L $(escape "$OUT") -l:kernel32.lib"
        export FLAGS_RELEASE="-Os -s"
        export FLAGS_DEBUG="-g -gcodeview -Wl,--pdb="
        "$root_dir/genlib.sh" "$OUT/kernel32.lib" "$root_dir/src/hc/windows/dll/kernel32.def"
        "$root_dir/tools/builder.sh" "$script_dir/windows/allocation.exe.c"
    fi
}

if test -z "$NO_X86_64"; then export ARCH=x86_64; build; fi
if test -z "$NO_AARCH64"; then export ARCH=aarch64; build; fi
if test -z "$NO_RISCV64"; then export ARCH=riscv64; build; fi
if test -z "$NO_WASM32"; then
    export ARCH=wasm32
    export ABI=unknown
    export FLAGS="-Wl,--no-entry"
    export FLAGS_RELEASE="-Os -s"
    export FLAGS_DEBUG="-g"
    "$root_dir/tools/builder.sh" "$script_dir/web/allocation.wasm.c"

    "$root_dir/tools/htmlPacker/build.sh"
    "$OUT/htmlPacker" "$OUT/allocation.html" "$script_dir/web/_start.html" "$OUT"
fi
