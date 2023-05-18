#!/bin/sh
set -e
script_dir="$(dirname "$0")"
cleanup() {
    set +e
    umount "$mnt"
    udisksctl loop-delete -b "$dev"
}
trap cleanup EXIT

prefix="$1"

# Create disk.
dd if=/dev/zero of="$script_dir/${prefix}disk.img" bs=34816 count=1

# Create disk filesystem.
mkfs.fat -F 12 -i 0 -n OS -f 1 -r 16 "$script_dir/${prefix}disk.img"

# Create loop device for disk.
dev=$(udisksctl loop-setup -f "$script_dir/${prefix}disk.img" | sed -E 's/^Mapped file .+ as ([^.]+).*$/\1/')

# Mount disk.
mnt=$(udisksctl mount -b "$dev" 2>&1 | sed -E 's/^Mounted .+ at ([^.]+).*$/\1/' | sed -E 's/.+ already mounted at `(.+)'\''.*$/\1/')

mkdir -p "$mnt/efi/boot"
cp "$script_dir/bootloader/x86_64/${prefix}bootloader.efi" "$mnt/efi/boot/bootx64.efi"
