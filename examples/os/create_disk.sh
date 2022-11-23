#!/bin/sh
set -e
script_dir="$(dirname $0)"
cleanup() {
    set +e
    umount "$mnt"
    udisksctl loop-delete -b "$dev"
}
trap cleanup EXIT

# Create disk.
dd if=/dev/zero of="$script_dir/disk.img" bs=34816 count=1

# Create disk filesystem.
mkfs.fat -F 12 -i 0 -n OS -f 1 -r 16 "$script_dir/disk.img"

# Create loop device for disk.
dev=$(udisksctl loop-setup -f "$script_dir/disk.img" | sed -E 's/^Mapped file .+ as ([^.]+).*$/\1/')

# Mount disk.
mnt=$(udisksctl mount -b "$dev" | sed -E 's/^Mounted .+ at ([^.]+).*$/\1/')

mkdir -p "$mnt/EFI/BOOT"
cp "$script_dir/bootloader.efi" "$mnt/EFI/BOOT/BOOTX64.EFI"
