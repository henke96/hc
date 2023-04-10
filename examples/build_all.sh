#!/bin/sh
script_dir="$(dirname "$0")"

# Run all build.sh scripts.
find "$script_dir" -name "build.sh" -type f -print -exec {} \;
