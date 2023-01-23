#!/bin/sh
script_dir="$(dirname "$0")"

# Run all build.sh scripts.
(cd "$script_dir" && find -name "build.sh" -type f -print -exec {} \;)
