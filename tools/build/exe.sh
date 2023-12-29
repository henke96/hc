#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/tools/shell/escape.sh"

build() {
    out_dir="$root_dir/../hc-out/$out_path/$ARCH"
    mkdir -p "$out_dir"

    if test -n "$LINK_KERNEL32"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/kernel32.def" "$out_dir/kernel32.lib"; fi
    if test -n "$LINK_USER32"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/user32.def" "$out_dir/user32.lib"; fi
    if test -n "$LINK_GDI32"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/gdi32.def" "$out_dir/gdi32.lib"; fi
    if test -n "$LINK_SYNCHRONIZATION"; then "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/synchronization.def" "$out_dir/synchronization.lib"; fi

    eval "set -- $(escape "-L$out_dir") $FLAGS $1"
    if test -n "$ASSEMBLY"; then
        "$root_dir/cc_pe.sh" $debug_flags -S -o "$out_dir/debug-$source_name$ext.s" "$source" "$@"
        "$root_dir/cc_pe.sh" $release_flags -S -o "$out_dir/$source_name$ext.s" "$source" "$@"
    fi
    "$root_dir/cc_pe.sh" $debug_flags -o "$out_dir/debug-$source_name$ext" "$source" "$@"
    "$root_dir/cc_pe.sh" $release_flags -o "$out_dir/$source_name$ext" "$source" "$@"

    if test -z "$NO_ANALYSIS"; then
        "$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$source" "$@"
        "$root_dir/cc_pe.sh" $release_flags $analyse_flags "$source" "$@"
    fi
}

source="$1"
source_name="${1##*/}"
source_name="${source_name%.*}"
out_path="$2"
ext="$3"

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
