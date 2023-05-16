#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

FLAGS="-shared -fPIC -l:kernel32.lib $FLAGS" "$root_dir/tools/build/exe.sh" "$script_dir" test dll
