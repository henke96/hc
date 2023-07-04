#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

"$root_dir/tools/build/linuxelf.sh" "$script_dir" allocation
