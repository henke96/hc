#!/bin/sh
set -e
script_dir="$(dirname $0)"
root_dir="$script_dir/../.."

# Kernel
export ABI=${ABI:-elf} # Can also try gnux32.
flags="-Wl,-T$script_dir/kernel/kernel.ld -mno-red-zone -O2 -s"
"$script_dir/../../cc_elf.sh" $flags -S -o "$script_dir/kernel/kernel.bin.s" "$script_dir/kernel/kernel.c"
"$script_dir/../../cc_elf.sh" $flags -o "$script_dir/kernel/kernel.bin.elf" "$script_dir/kernel/kernel.c"
unset ABI

"${LLVM}llvm-objcopy" -O binary "$script_dir/kernel/kernel.bin.elf" "$script_dir/kernel/kernel.bin"

# Bootloader (with kernel binary embedded)
FLAGS="-I$script_dir/kernel -Os" "$root_dir/tools/build/efi.sh" "$script_dir/bootloader" bootloader
