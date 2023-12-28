#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../../.."

export FLAGS="-Wl,--no-entry $FLAGS"
"$root_dir/tools/build/wasm.sh" "$script_dir/openGl.c" openGl/web .wasm

"$root_dir/tools/htmlPacker/build.sh" openGl/web

"$root_dir/../hc-out/openGl/web/htmlPacker" "$script_dir/_start.html" "$root_dir/../hc-out/openGl/web" openGl
