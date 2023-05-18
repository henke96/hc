#!/bin/sh
set -e
script_dir="$(dirname "$0")"

find "$script_dir" -name "build.sh" -type f -print -exec {} \;
