#!/bin/sh
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

export LINK_LIBC=1
"$root_dir/tools/build/gnulinuxelf.sh" "$script_dir" hello
