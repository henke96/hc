#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

FLAGS="-l:kernel32.lib -l:user32.lib -l:gdi32.lib" "$root_dir/tools/build/exe.sh" "$script_dir" winHello
