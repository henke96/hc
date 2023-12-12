#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"

git -C "$script_dir" clean -fdqX
