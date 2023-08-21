#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"

find "$script_dir" -name "build.sh" -type f -print -exec {} \;
