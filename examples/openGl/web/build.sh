#!/bin/sh
set -e
script_dir="$(dirname $0)"
flags="-Wl,--no-entry -O2 -s $FLAGS"
"$script_dir/../../../cc_wasm.sh" $flags -S -o "$script_dir/release.wasm.s" "$script_dir/main.c"
"$script_dir/../../../cc_wasm.sh" $flags -o "$script_dir/release.wasm" "$script_dir/main.c"

# Build html packer if needed.
html_packer_path="$script_dir/../../../tools/htmlPacker"
if test ! -f "$html_packer_path/release.bin"; then
"$html_packer_path/build.sh"
fi

# Generate html.
(cd "$script_dir" && "$html_packer_path/release.bin" main.html openGl)
if test "$MINIFY" = "yes"; then
html-minifier --collapse-whitespace --remove-comments --remove-optional-tags --remove-redundant-attributes --remove-script-type-attributes --use-short-doctype --minify-css true --minify-js true openGl.html > openGlMin.html
(cd "$script_dir" && "$html_packer_path/release.bin" openGlMin.html openGlMin)
fi
