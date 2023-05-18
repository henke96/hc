#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 PATH PROGRAM_NAME [EXT]"
    exit 1
fi

path="$1"
prog_name="$2"
ext="${3:-exe}"

script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

common_flags="-Wl,-subsystem,windows"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -g3 -gcodeview -Wl,--pdb="
release_flags="$common_flags -fomit-frame-pointer -Ddebug_NDEBUG -s -Os"

if test -n "$LINK_KERNEL32"; then FLAGS="-l:kernel32.lib $FLAGS"; fi
if test -n "$LINK_USER32"; then FLAGS="-l:user32.lib $FLAGS"; fi
if test -n "$LINK_GDI32"; then FLAGS="-l:gdi32.lib $FLAGS"; fi

build() {
    mkdir -p "$path/$ARCH"
    if test -n "$LINK_KERNEL32"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/kernel32.def" "$path/$ARCH/kernel32.lib"; fi
    if test -n "$LINK_USER32"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/user32.def" "$path/$ARCH/user32.lib"; fi
    if test -n "$LINK_GDI32"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/gdi32.def" "$path/$ARCH/gdi32.lib"; fi

    eval "set -- $("$script_dir/../shellUtil/shellescape.sh" "-L$path/$ARCH") $FLAGS $1"

    if test -n "$ASSEMBLY"; then
        "$root_dir/cc_pe.sh" $debug_flags -S -o "$path/$ARCH/debug.$prog_name.$ext.s" "$path/$prog_name.c" "$@"
        "$root_dir/cc_pe.sh" $release_flags -S -o "$path/$ARCH/$prog_name.$ext.s" "$path/$prog_name.c" "$@"
    fi
    "$root_dir/cc_pe.sh" $debug_flags -o "$path/$ARCH/debug.$prog_name.$ext" "$path/$prog_name.c" "$@"
    "$root_dir/cc_pe.sh" $release_flags -o "$path/$ARCH/$prog_name.$ext" "$path/$prog_name.c" "$@"

    if test -z "$NO_ANALYSIS"; then
        analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
        "$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$path/$prog_name.c" "$@"
        "$root_dir/cc_pe.sh" $release_flags $analyse_flags "$path/$prog_name.c" "$@"
    fi
}

if test -z "$NO_X86_64"; then ARCH="x86_64" build "$FLAGS_X86_64"; fi
if test -z "$NO_AARCH64"; then ARCH="aarch64" build "$FLAGS_AARCH64"; fi
