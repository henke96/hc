#!/bin/sh
set -e
script_dir="$(dirname $0)"

# Kernel
flags="-Tkernel/kernel.ld -mno-red-zone -O2 -s"
"$script_dir/../../cc_elf.sh" $flags -S -o "$script_dir/kernel/kernel.bin.s" "$script_dir/kernel/main.c"
"$script_dir/../../cc_elf.sh" $flags -o "$script_dir/kernel/kernel.bin.elf" "$script_dir/kernel/main.c"
llvm-objcopy -O binary "$script_dir/kernel/kernel.bin.elf" "$script_dir/kernel/kernel.bin"

# Bootloader (with kernel binary embedded)
flags="-Wl,-subsystem:efi_application -O2 -s"
"$script_dir/../../cc_pe.sh" $flags -S -o "$script_dir/bootloader/bootloader.efi.s" "$script_dir/bootloader/main.c"
"$script_dir/../../cc_pe.sh" $flags -o "$script_dir/bootloader.efi" "$script_dir/bootloader/main.c"
