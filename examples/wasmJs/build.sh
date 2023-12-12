#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

export FLAGS="-Wl,--no-entry"
"$root_dir/tools/build/wasm.sh" "$script_dir" wasmJs
