#!/bin/sh
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."
"$root_dir/tools/build/elf.sh" "$script_dir" allocation