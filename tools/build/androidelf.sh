#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
root_dir="$script_dir/../.."

build() {
    mkdir -p "$prog_path/$ARCH"
    if test -n "$LINK_LIBLOG"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/liblog.so.c" "$prog_path/$ARCH/liblog.so"; fi
    if test -n "$LINK_LIBDL"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libdl.so.c" "$prog_path/$ARCH/libdl.so"; fi
    if test -n "$LINK_LIBANDROID"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libandroid.so.c" "$prog_path/$ARCH/libandroid.so"; fi
    if test -n "$LINK_LIBC"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libc.so.c" "$prog_path/$ARCH/libc.so"; fi
}

prog_path="$1"

if test -n "$LINK_LIBLOG"; then FLAGS="-l:liblog.so $FLAGS"; fi
if test -n "$LINK_LIBDL"; then FLAGS="-l:libdl.so $FLAGS"; fi
if test -n "$LINK_LIBANDROID"; then FLAGS="-l:libandroid.so $FLAGS"; fi
if test -n "$LINK_LIBC"; then FLAGS="-l:libc.so $FLAGS"; fi

if test -z "$NO_X86_64"; then export ARCH="x86_64"; build; fi
if test -z "$NO_AARCH64"; then export ARCH="aarch64"; build; fi

export NO_RISCV64=1
export ABI="linux-android26"
export FLAGS="-fPIC -fpie -pie -Wl,-dynamic-linker=/system/bin/linker64 $FLAGS"
export STRIP_OPT="${STRIP_OPT:---strip-all}"
"$script_dir/elf.sh" "$@"
