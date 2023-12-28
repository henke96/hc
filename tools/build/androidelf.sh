#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

build() {
    out_dir="$root_dir/../hc-out/$out_path/$ARCH"
    mkdir -p "$out_dir"
    if test -n "$LINK_LIBLOG"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/liblog.so.c" "$out_dir/liblog.so"; fi
    if test -n "$LINK_LIBDL"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libdl.so.c" "$out_dir/libdl.so"; fi
    if test -n "$LINK_LIBANDROID"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libandroid.so.c" "$out_dir/libandroid.so"; fi
    if test -n "$LINK_LIBC"; then "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/android/libc.so.c" "$out_dir/libc.so"; fi
}

out_path="$2"

if test -n "$LINK_LIBLOG"; then FLAGS="-l:liblog.so $FLAGS"; fi
if test -n "$LINK_LIBDL"; then FLAGS="-l:libdl.so $FLAGS"; fi
if test -n "$LINK_LIBANDROID"; then FLAGS="-l:libandroid.so $FLAGS"; fi
if test -n "$LINK_LIBC"; then FLAGS="-l:libc.so $FLAGS"; fi

export ABI="linux-android26"
if test -z "$NO_X86_64"; then export ARCH="x86_64"; build; fi
if test -z "$NO_AARCH64"; then export ARCH="aarch64"; build; fi

export NO_RISCV64=1
export FLAGS="-fPIC -fpie -pie -Wl,-dynamic-linker=/system/bin/linker64 $FLAGS"
export STRIP_OPT="${STRIP_OPT:---strip-all}"
"$script_dir/elf.sh" "$@"
