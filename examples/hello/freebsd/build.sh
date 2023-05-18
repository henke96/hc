#!/bin/sh
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

export LINK_LIBC=1
"$root_dir/tools/build/freebsdelf.sh" "$script_dir" hello
