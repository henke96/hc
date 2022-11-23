#!/bin/sh
set -e
script_dir="$(dirname $0)"
root_dir="$script_dir/../../.."
"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/kernel32.def" "$script_dir/libkernel32.a"
"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/user32.def" "$script_dir/libuser32.a"
"$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/gdi32.def" "$script_dir/libgdi32.a"

flags="-L$script_dir -Wl,-subsystem,console -O2"
debug_flags="$flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -g $FLAGS"
release_flags="$flags -Ddebug_NDEBUG -s $FLAGS"
"$root_dir/cc_pe.sh" $debug_flags -S -o "$script_dir/debug.exe.s" "$script_dir/main.c"
"$root_dir/cc_pe.sh" $debug_flags -o "$script_dir/debug.exe" "$script_dir/main.c" -lkernel32 -luser32 -lgdi32
"$root_dir/cc_pe.sh" $release_flags -S -o "$script_dir/release.exe.s" "$script_dir/main.c"
"$root_dir/cc_pe.sh" $release_flags -o "$script_dir/release.exe" "$script_dir/main.c" -lkernel32 -luser32 -lgdi32
