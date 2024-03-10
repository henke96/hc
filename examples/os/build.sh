#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. $root_dir/tools/shell/escape.sh

# Kernel
export ARCH=x86_64
export ABI=linux
export FLAGS="-Os -s -Wl,-T,$(escape "$script_dir/kernel/kernel.ld") -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
export FLAGS_RELEASE=
export FLAGS_DEBUG=
"$root_dir/tools/builder.sh" "$script_dir/kernel/kernel.elf.c"

"$root_dir/objcopy.sh" -O binary "$OUT/x86_64-linux_kernel.elf" "$OUT/kernel.bin"
"$root_dir/objcopy.sh" -O binary "$OUT/debug_x86_64-linux_kernel.elf" "$OUT/debug_kernel.bin"

# Bootloader (with kernel binary embedded)
export ARCH=x86_64
export ABI=windows-gnu
export FLAGS="-Os -s -I $(escape "$OUT") -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wl,-subsystem,efi_application "
export FLAGS_RELEASE=
export FLAGS_DEBUG=
"$root_dir/tools/builder.sh" "$script_dir/bootloader/bootloader.efi.c"
