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

if test -n "$LINK_LIBLOG"; then FLAGS="-l:liblog.so $FLAGS"; fi
if test -n "$LINK_LIBDL"; then FLAGS="-l:libdl.so $FLAGS"; fi
if test -n "$LINK_LIBANDROID"; then FLAGS="-l:libandroid.so $FLAGS"; fi
if test -n "$LINK_LIBC"; then FLAGS="-l:libc.so $FLAGS"; fi

build() {
    mkdir -p "$path/$ARCH"
    if test -n "$LINK_LIBLOG"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/liblog.so.c" "$path/$ARCH/liblog.so"; fi
    if test -n "$LINK_LIBDL"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libdl.so.c" "$path/$ARCH/libdl.so"; fi
    if test -n "$LINK_LIBANDROID"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libandroid.so.c" "$path/$ARCH/libandroid.so"; fi
    if test -n "$LINK_LIBC"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libc.so.c" "$path/$ARCH/libc.so"; fi
}

if test -z "$NO_X86_64"; then ARCH="x86_64" build; fi
if test -z "$NO_AARCH64"; then ARCH="aarch64" build; fi

NO_RISCV64=1 ABI="linux-android26" FLAGS="-fPIC -fpie -pie -Wl,-dynamic-linker=/system/bin/linker64 $FLAGS" STRIP_OPT="--strip-all" "$script_dir/elf.sh" "$@"
