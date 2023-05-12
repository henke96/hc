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

"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/kernel32.def" "$path/kernel32.lib"
"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/user32.def" "$path/user32.lib"
"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/gdi32.def" "$path/gdi32.lib"

common_flags="-Wl,-subsystem,windows -O2"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -gcodeview"
release_flags="$common_flags -Ddebug_NDEBUG -s"
eval "set -- $("$script_dir/../shellUtil/shellescape.sh" "-L$path") $FLAGS"

"$root_dir/cc_pe.sh" $debug_flags -S -o "$path/debug.$prog_name.$ext.s" "$path/$prog_name.c" "$@"
"$root_dir/cc_pe.sh" $debug_flags -o "$path/debug.$prog_name.$ext" "$path/$prog_name.c" "$@"
"$root_dir/cc_pe.sh" $release_flags -S -o "$path/$prog_name.$ext.s" "$path/$prog_name.c" "$@"
"$root_dir/cc_pe.sh" $release_flags -o "$path/$prog_name.$ext" "$path/$prog_name.c" "$@"

# Static analysis.
analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
"$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$path/$prog_name.c" "$@"
"$root_dir/cc_pe.sh" $release_flags $analyse_flags "$path/$prog_name.c" "$@"
