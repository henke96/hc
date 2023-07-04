#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

export STRIP_OPT="${STRIP_OPT:---strip-sections}"
"$script_dir/elf.sh" "$@"
