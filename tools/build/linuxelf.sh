#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"

export STRIP_OPT="${STRIP_OPT:---strip-sections}"
"$script_dir/elf.sh" "$@"
