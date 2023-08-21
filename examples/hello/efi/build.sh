#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
root_dir="$script_dir/../../.."

"$root_dir/tools/build/efi.sh" "$script_dir" hello
