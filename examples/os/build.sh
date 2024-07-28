#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. $root_dir/src/shell/escape.sh

test -n "$OUT" || { echo "Please set OUT"; exit 1; }

# Kernel
export ARCH=x86_64
export ABI=linux
export FLAGS="-Os -s -Wl,-T,$(escape "$script_dir/kernel/kernel.ld") -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
export FLAGS_RELEASE=
export FLAGS_DEBUG=
"$root_dir/tools/builder.sh" "$script_dir/kernel/kernel.elf.c"

"$root_dir/objcopy.sh" -O binary "$OUT/$ARCH-${ABI}_kernel.elf" "$OUT/kernel.bin"
if test -z "$NO_DEBUG"; then
    "$root_dir/objcopy.sh" -O binary "$OUT/debug/$ARCH-${ABI}_kernel.elf" "$OUT/debug/kernel.bin"
fi

# Bootloader (with kernel binary embedded)
export ARCH=x86_64
export ABI=windows-gnu
export FLAGS="-Os -s -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wl,-subsystem,efi_application"
export FLAGS_RELEASE="-I $(escape "$OUT")"
export FLAGS_DEBUG="-I $(escape "$OUT/debug")"
"$root_dir/tools/builder.sh" "$script_dir/bootloader/bootloader.efi.c"
