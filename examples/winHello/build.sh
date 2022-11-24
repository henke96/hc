#!/bin/sh
set -e
script_dir="$(dirname $0)"
root_dir="$script_dir/../.."

"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/kernel32.def" "$script_dir/kernel32.lib"
"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/user32.def" "$script_dir/user32.lib"

common_flags="-L$script_dir -Wl,-subsystem,console -O2 -s $FLAGS"
"$root_dir/cc_pe.sh" $common_flags -S -o "$script_dir/release.exe.s" "$script_dir/main.c"
"$root_dir/cc_pe.sh" $common_flags -o "$script_dir/release.exe" "$script_dir/main.c" -l:kernel32.lib -l:user32.lib
