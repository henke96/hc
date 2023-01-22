#!/bin/sh
script_dir="$(dirname "$0")"
qemu-system-x86_64 -bios /usr/share/qemu/OVMF.fd -drive format=raw,file="$script_dir/disk.img" -enable-kvm
