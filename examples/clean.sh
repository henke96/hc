#!/bin/sh
script_dir="$(dirname "$0")"

# Clean up all files covered by .gitignore.
(cd "$script_dir" && git clean -fdqX --exclude="!*.keystore")
