#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

FLAGS="-shared -fPIC" "$root_dir/tools/build/elf.sh" "$script_dir/linux" libtest so
FLAGS="-shared -fPIC -l:kernel32.lib" "$root_dir/tools/build/exe.sh" "$script_dir/windows" test dll
