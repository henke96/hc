#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../../.."

"$root_dir/tools/build/wasm.sh" "$script_dir" tests
