#!/bin/sh
set -e
script_dir="$(dirname $0)"
flags="-Wl,--no-entry -O2 -s $FLAGS"
"$script_dir/../../../cc_wasm.sh" $flags -S -o "$script_dir/release.wasm.s" "$script_dir/main.c"
"$script_dir/../../../cc_wasm.sh" $flags -o "$script_dir/release.wasm" "$script_dir/main.c"

# Build html packer if needed.
if test ! -f "$script_dir/../../../tools/htmlPacker/release.bin"; then
    "$script_dir/../../../tools/htmlPacker/build.sh"
fi

# Generate html.
(cd "$script_dir" && ../../../tools/htmlPacker/release.bin main.html allocation)
