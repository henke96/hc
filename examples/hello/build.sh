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
    "$root_dir/tools/builder.sh" "$script_dir/linux/hello.c"
    "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_hello"

    export ABI=linux-gnu
    export FLAGS="-L $(escape "$OUT") -l:libc.so.6"
    "$root_dir/cc.sh" -fPIC -shared -o "$OUT/libc.so.6" "$root_dir/src/hc/linux/gnu/libc.so.6.c"
    "$root_dir/tools/builder.sh" "$script_dir/gnulinux/hello.c"
    "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_hello"

    export ABI=linux-android26
    # Android's dynamic linker demands PIE.
    export FLAGS="-fPIE -pie -Wl,-dynamic-linker=/system/bin/linker64 -L $(escape "$OUT") -l:libc.so -l:liblog.so"
    "$root_dir/cc.sh" -fPIC -shared -o "$OUT/libc.so" "$root_dir/src/hc/linux/android/libc.so.c"
    "$root_dir/cc.sh" -fPIC -shared -o "$OUT/liblog.so" "$root_dir/src/hc/linux/android/liblog.so.c"
    "$root_dir/tools/builder.sh" "$script_dir/android/hello.c"
    "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_hello"

    export ABI=freebsd14
    export FLAGS="-Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L $(escape "$OUT") -l:libc.so.7"
    "$root_dir/cc.sh" -fPIC -shared -Wl,--version-script="$root_dir/src/hc/freebsd/libc.so.7.map" -o "$OUT/libc.so.7" "$root_dir/src/hc/freebsd/libc.so.7.c"
    "$root_dir/tools/builder.sh" "$script_dir/freebsd/hello.c"
    "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_hello"

    if test "$ARCH" != "riscv64"; then
        export ABI=windows-gnu
        export FLAGS="-Wl,-subsystem,console -L $(escape "$OUT") -l:kernel32.lib"
        export FLAGS_RELEASE="-Os -s"
        export FLAGS_DEBUG="-g -gcodeview -Wl,--pdb="
        "$root_dir/genlib.sh" "$OUT/kernel32.lib" "$root_dir/src/hc/windows/dll/kernel32.def"
        "$root_dir/tools/builder.sh" "$script_dir/windows/hello.exe.c"

        export ABI=windows-gnu
        export FLAGS="-Wl,-subsystem,efi_application -Os -s"
        export FLAGS_RELEASE=
        export FLAGS_DEBUG=
        "$root_dir/tools/builder.sh" "$script_dir/efi/hello.efi.c"
    fi
}

if test -z "$NO_X86_64"; then export ARCH=x86_64; build; fi
if test -z "$NO_AARCH64"; then export ARCH=aarch64; build; fi
if test -z "$NO_RISCV64"; then export ARCH=riscv64; build; fi
if test -z "$NO_WASM32"; then
    export ARCH=wasm32
    export ABI=wasi
    export FLAGS=
    export FLAGS_RELEASE="-Os -s"
    export FLAGS_DEBUG="-g"
    "$root_dir/tools/builder.sh" "$script_dir/wasi/hello.c"
fi
