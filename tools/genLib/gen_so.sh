#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
root_dir="$script_dir/../.."

input="$1"
output="$2"

"$root_dir/cc_elf.sh" -shared -o "$output" "$input"
