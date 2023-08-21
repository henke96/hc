#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
root_dir="$script_dir/../../.."

export FLAGS="-Wl,--no-entry $FLAGS"
"$root_dir/tools/build/wasm.sh" "$script_dir" allocation

"$root_dir/tools/htmlPacker/htmlPacker.sh" "$script_dir" _start.html allocation
