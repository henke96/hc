#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 INPUT.c OUTPUT.so"
    exit 1
fi

script_dir="$(dirname "$0")"
"$script_dir/../../cc_elf.sh" -shared -o "$2" "$1"
