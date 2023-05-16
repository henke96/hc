#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

export LINK_LIBDL=1 LINK_LIBLOG=1
"$root_dir/tools/build/androidelf.sh" "$script_dir" hello elf
