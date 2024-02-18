#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

input="$1"
output="$2"
if test -n "$3"; then version_script_arg="-Wl,--version-script=$3"; fi

"$root_dir/cc_elf.sh" -shared -fPIC $version_script_arg -o "$output" "$input"
