#!/bin/sh
set -e
script_dir="$(dirname $0)"
flags="-Wl,-subsystem:console -O2"
debug_flags="$flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -g $FLAGS"
release_flags="$flags -Ddebug_NDEBUG -s $FLAGS"
"$script_dir/../../../cc_pe.sh" $debug_flags -S -o "$script_dir/debug.exe.s" "$script_dir/main.c"
"$script_dir/../../../cc_pe.sh" $debug_flags -o "$script_dir/debug.exe" "$script_dir/main.c" -lkernel32 -luser32 -lgdi32
"$script_dir/../../../cc_pe.sh" $release_flags -S -o "$script_dir/release.exe.s" "$script_dir/main.c"
"$script_dir/../../../cc_pe.sh" $release_flags -o "$script_dir/release.exe" "$script_dir/main.c" -lkernel32 -luser32 -lgdi32
