#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

input="$1"
output="$2"

"$root_dir/cc_elf.sh" -shared -o "$output" "$input"
