script_dir="$(dirname $0)"
"$script_dir/../../src/linux/gnu/gen_lib.sh" "libdl.so.2" "$script_dir"
LFLAGS="-Wl,-dynamic-linker,/lib64/ld-linux-x86-64.so.2 -L. -Wl,-Bdynamic -l:libdl.so.2" "$script_dir/../../build.sh" "$script_dir"