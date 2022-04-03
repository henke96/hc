#!/bin/sh
script_dir="$(dirname $0)"
CC="${CC:-clang}" "$script_dir/../../cc.sh" -target x86_64-unknown-windows -Wl,-subsystem:efi_application -Wl,-entry:main -fuse-ld=lld -O2 -s -o "$script_dir/bootx64.efi" $@ "$script_dir/main.c"
