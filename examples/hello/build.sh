#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/src/shell/escape.sh"

test -n "$OUT" || { echo "Please set OUT"; exit 1; }
name=hello
opt=-Os

build() {
    export FLAGS_RELEASE="$opt"
    export FLAGS_DEBUG="-g"

    export ABI=linux
    if test -z "$NO_LINUX"; then
        export FLAGS=
        "$root_dir/tools/builder.sh" "$script_dir/linux/$name.c"
        "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_$name"
    fi
    export ABI=linux-gnu
    if test -z "$NO_GNULINUX"; then
        export FLAGS="-L $(escape "$OUT") -l:libc.so.6"
        "$root_dir/cc.sh" -fPIC -shared -o "$OUT/libc.so.6" "$root_dir/src/hc/linux/gnu/libc.so.6.c"
        "$root_dir/tools/builder.sh" "$script_dir/gnulinux/$name.c"
        "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_$name"
    fi
    export ABI=linux-android26
    if test -z "$NO_ANDROID"; then
        # Android's dynamic linker demands PIE.
        export FLAGS="-fPIE -pie -Wl,-dynamic-linker=/system/bin/linker64 -L $(escape "$OUT") -l:libc.so -l:liblog.so"
        "$root_dir/cc.sh" -fPIC -shared -o "$OUT/libc.so" "$root_dir/src/hc/linux/android/libc.so.c"
        "$root_dir/cc.sh" -fPIC -shared -o "$OUT/liblog.so" "$root_dir/src/hc/linux/android/liblog.so.c"
        "$root_dir/tools/builder.sh" "$script_dir/android/$name.c"
        "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_$name"
    fi
    export ABI=freebsd14
    if test -z "$NO_FREEBSD"; then
        export FLAGS="-Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L $(escape "$OUT") -l:libc.so.7"
        "$root_dir/cc.sh" -fPIC -shared -Wl,--version-script="$root_dir/src/hc/freebsd/libc.so.7.map" -o "$OUT/libc.so.7" "$root_dir/src/hc/freebsd/libc.so.7.c"
        "$root_dir/tools/builder.sh" "$script_dir/freebsd/$name.c"
        "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_$name"
    fi
    export ABI=windows-gnu
    if test -z "$NO_WINDOWS"; then
        export FLAGS="-Wl,-subsystem,console -L $(escape "$OUT") -l:kernel32.lib"
        export FLAGS_RELEASE="$opt -s"
        export FLAGS_DEBUG="-g -gcodeview -Wl,--pdb="
        "$root_dir/genlib.sh" "$OUT/kernel32.lib" "$root_dir/src/hc/windows/dll/kernel32.def"
        "$root_dir/tools/builder.sh" "$script_dir/windows/$name.exe.c"
    fi
    if test -z "$NO_EFI"; then
        export FLAGS="-Wl,-subsystem,efi_application $opt -s"
        export FLAGS_RELEASE=
        export FLAGS_DEBUG=
        "$root_dir/tools/builder.sh" "$script_dir/efi/$name.efi.c"
    fi
}

if test -z "$NO_X86_64"; then export ARCH=x86_64; build; fi
if test -z "$NO_AARCH64"; then export ARCH=aarch64; build; fi
if test -z "$NO_RISCV64"; then
    (
        export ARCH=riscv64 NO_WINDOWS=1 NO_EFI=1; build
    )
fi
if test -z "$NO_WASM32"; then
    export ARCH=wasm32
    export ABI=wasi
    export FLAGS=
    export FLAGS_RELEASE="$opt -s"
    export FLAGS_DEBUG="-g"
    "$root_dir/tools/builder.sh" "$script_dir/wasi/$name.c"
fi
