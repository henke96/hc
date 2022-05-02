script_dir="$(dirname $0)"
"$script_dir/../../src/linux/gnu/gen_lib.sh" "libdl.so.2" "$script_dir"
LFLAGS="-L$script_dir -Wl,-Bdynamic -l:libdl.so.2" "$script_dir/../../build.sh" "$script_dir"