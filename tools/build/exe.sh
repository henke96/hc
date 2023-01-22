#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 PATH PROGRAM_NAME"
    exit 1
fi

script_dir="$(dirname $0)"
root_dir="$script_dir/../.."

"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/kernel32.def" "$1/kernel32.lib"
"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/user32.def" "$1/user32.lib"
"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/gdi32.def" "$1/gdi32.lib"

common_flags="-Wl,-subsystem,windows -L$1 -O2"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -g $FLAGS"
release_flags="$common_flags -Ddebug_NDEBUG -s $FLAGS"
"$root_dir/cc_pe.sh" $debug_flags -S -o "$1/$2.debug.exe.s" "$1/$2.c" $LFLAGS
"$root_dir/cc_pe.sh" $debug_flags -o "$1/$2.debug.exe" "$1/$2.c" $LFLAGS
"$root_dir/cc_pe.sh" $release_flags -S -o "$1/$2.exe.s" "$1/$2.c" $LFLAGS
"$root_dir/cc_pe.sh" $release_flags -o "$1/$2.exe" "$1/$2.c" $LFLAGS

# Static analysis.
analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
"$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$1/$2.c" $LFLAGS
"$root_dir/cc_pe.sh" $release_flags $analyse_flags "$1/$2.c" $LFLAGS
