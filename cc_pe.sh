#!/bin/sh
set -e
script_dir="$(dirname $0)"
flags="$(cat $script_dir/flags)"
CC="${CC:-clang}"
LD="${LD:-lld}"
ARCH="${ARCH:-x86_64}"
"$CC" -I$script_dir/src $flags -target $ARCH-unknown-windows-gnu -fuse-ld="$LD" -Wl,--no-insert-timestamp -Wl,-e,_start -L"$script_dir/src/hc/windows/lib" "$@"
