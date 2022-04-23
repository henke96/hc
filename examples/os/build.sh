#!/bin/sh
set -e
script_dir="$(dirname $0)"

# Kernel
export FORMAT=elf # Can also try gnux32, but clang seems to have bugs if optimizations are enabled.
flags="-Wl,-T$script_dir/kernel/kernel.ld -Wl,--print-map -mno-red-zone -O2 -s"
"$script_dir/../../cc_elf.sh" $flags -S -o "$script_dir/kernel/kernel.bin.s" "$script_dir/kernel/main.c"
"$script_dir/../../cc_elf.sh" $flags -o "$script_dir/kernel/kernel.bin.elf" "$script_dir/kernel/main.c"
unset FORMAT

llvm-objcopy -O binary "$script_dir/kernel/kernel.bin.elf" "$script_dir/kernel/kernel.bin"

# Bootloader (with kernel binary embedded)
flags="-Wl,-subsystem:efi_application -I$script_dir/kernel -Os -s"
"$script_dir/../../cc_pe.sh" $flags -S -o "$script_dir/bootloader/bootloader.efi.s" "$script_dir/bootloader/main.c"
"$script_dir/../../cc_pe.sh" $flags -o "$script_dir/bootloader.efi" "$script_dir/bootloader/main.c"
