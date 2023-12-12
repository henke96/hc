#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
prefix="$1"

# Create disk.
dd if=/dev/zero of="$script_dir/${prefix}disk.img" bs=65536 count=1

mformat -i "$script_dir/${prefix}disk.img" -N 0 -v OS ::
mmd -i "$script_dir/${prefix}disk.img" ::/efi
mmd -i "$script_dir/${prefix}disk.img" ::/efi/boot
mcopy -i "$script_dir/${prefix}disk.img" "$script_dir/bootloader/x86_64/${prefix}bootloader.efi" ::/efi/boot/bootx64.efi
