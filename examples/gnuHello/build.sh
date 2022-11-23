#!/bin/sh
script_dir="$(dirname $0)"
root_dir="$script_dir/../.."
"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/dynamic/libc.so.6.c" "$script_dir/libc.so.6"
"$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/linux/gnulinux/dynamic/libdl.so.2.c" "$script_dir/libdl.so.2"
LFLAGS="-L$script_dir -Wl,-Bdynamic -l:libc.so.6 -l:libdl.so.2" "$root_dir/examples/helper.sh" "$script_dir"