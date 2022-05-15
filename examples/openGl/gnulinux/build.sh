#!/bin/sh
script_dir="$(dirname $0)"
"$script_dir/../../../src/linux/gnulinux/dynamic/gen_lib.sh" "libc.so.6" "$script_dir"
"$script_dir/../../../src/linux/gnulinux/dynamic/gen_lib.sh" "libdl.so.2" "$script_dir"
LFLAGS="-L$script_dir -Wl,-Bdynamic -l:libc.so.6 -l:libdl.so.2" "$script_dir/../../../build.sh" "$script_dir"