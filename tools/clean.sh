#!/bin/sh
set -e
script_dir="$(dirname "$0")"

git -C "$script_dir" clean -fdqX --exclude="!*.keystore"
