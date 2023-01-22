#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../../.."

FLAGS="-Wl,--no-entry" "$root_dir/tools/build/wasm.sh" "$script_dir" openGl

(
    cd "$script_dir"
    "../../../tools/htmlPacker/htmlPacker.sh" _start.html openGl
    if test "$MINIFY" = "yes"; then
        html-minifier --collapse-whitespace --remove-comments --remove-optional-tags --remove-redundant-attributes --remove-script-type-attributes --use-short-doctype --minify-css true --minify-js true openGl.html > openGlMin.html
        "../../../tools/htmlPacker/htmlPacker.sh" _start.html openGl
    fi
)

