#!/bin/sh
# This doesn't seem to work yet, lld says "unknown file type"..
script_dir="$(dirname $0)"
"$script_dir/build_x86_64.sh" -target riscv64-unknown-windows -fuse-ld=lld-link-14 -o bootriscv64.efi
