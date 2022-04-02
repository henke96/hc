#!/bin/sh
script_dir="$(dirname $0)"
"$script_dir/build_x86_64.sh" -target aarch64-unknown-windows -o bootaa64.efi
