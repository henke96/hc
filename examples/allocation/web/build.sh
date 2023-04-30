#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

FLAGS="-Wl,--no-entry" "$root_dir/tools/build/wasm.sh" "$script_dir" allocation

(cd "$script_dir"; "../../../tools/htmlPacker/htmlPacker.sh" _start.html allocation)
