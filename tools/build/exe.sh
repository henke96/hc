#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
root_dir="$script_dir/../.."

build() {
    mkdir -p "$prog_path/$ARCH"
    if test -n "$LINK_KERNEL32"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/kernel32.def" "$prog_path/$ARCH/kernel32.lib"; fi
    if test -n "$LINK_USER32"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/user32.def" "$prog_path/$ARCH/user32.lib"; fi
    if test -n "$LINK_GDI32"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/gdi32.def" "$prog_path/$ARCH/gdi32.lib"; fi
    if test -n "$LINK_SYNCHRONIZATION"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/synchronization.def" "$prog_path/$ARCH/synchronization.lib"; fi

    eval "set -- $("$script_dir/../shellUtil/escape.sh" "-L$prog_path/$ARCH") $FLAGS $1"

    if test -n "$ASSEMBLY"; then
        "$root_dir/cc_pe.sh" $debug_flags -S -o "$prog_path/$ARCH/debug.$prog_name.$ext.s" "$prog_path/$prog_name.c" "$@"
        "$root_dir/cc_pe.sh" $release_flags -S -o "$prog_path/$ARCH/$prog_name.$ext.s" "$prog_path/$prog_name.c" "$@"
    fi
    "$root_dir/cc_pe.sh" $debug_flags -o "$prog_path/$ARCH/debug.$prog_name.$ext" "$prog_path/$prog_name.c" "$@"
    "$root_dir/cc_pe.sh" $release_flags -o "$prog_path/$ARCH/$prog_name.$ext" "$prog_path/$prog_name.c" "$@"

    if test -z "$NO_ANALYSIS"; then
        "$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$prog_path/$prog_name.c" "$@"
        "$root_dir/cc_pe.sh" $release_flags $analyse_flags "$prog_path/$prog_name.c" "$@"
    fi
}

prog_path="$1"
prog_name="$2"
ext="${3:-exe}"

analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
common_flags="-Wl,-subsystem,windows"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -g3 -gcodeview -Wl,--pdb= -Dhc_DEBUG"
release_flags="$common_flags -fomit-frame-pointer -s -Os"

if test -n "$LINK_KERNEL32"; then FLAGS="-l:kernel32.lib $FLAGS"; fi
if test -n "$LINK_USER32"; then FLAGS="-l:user32.lib $FLAGS"; fi
if test -n "$LINK_GDI32"; then FLAGS="-l:gdi32.lib $FLAGS"; fi
if test -n "$LINK_SYNCHRONIZATION"; then FLAGS="-l:synchronization.lib $FLAGS"; fi

if test -z "$NO_X86_64"; then export ARCH="x86_64"; build "$FLAGS_X86_64"; fi
if test -z "$NO_AARCH64"; then export ARCH="aarch64"; build "$FLAGS_AARCH64"; fi
