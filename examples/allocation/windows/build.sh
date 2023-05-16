#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

LINK_KERNEL32=1 "$root_dir/tools/build/exe.sh" "$script_dir" allocation
