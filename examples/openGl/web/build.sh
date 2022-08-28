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
(cd "$script_dir" && ../../../tools/htmlPacker/release.bin main.html openGl)
if test "$MINIFY" = "yes"; then
html-minifier --collapse-whitespace --remove-comments --remove-optional-tags --remove-redundant-attributes --remove-script-type-attributes --use-short-doctype --minify-css true --minify-js true openGl.html > openGlMin.html
(cd "$script_dir" && ../../../tools/htmlPacker/release.bin openGlMin.html openGlMin)
fi
