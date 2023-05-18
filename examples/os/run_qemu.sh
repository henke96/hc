#!/bin/sh
set -e
script_dir="$(dirname "$0")"
prefix="$1"
qemu-system-x86_64 -bios /usr/share/qemu/OVMF.fd -drive format=raw,file="$script_dir/${prefix}disk.img" -enable-kvm
