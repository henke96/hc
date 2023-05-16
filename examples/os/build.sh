#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

# Kernel
flags="-Wl,-T$script_dir/kernel/kernel.ld -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
ARCH="x86_64" FLAGS="$flags" "$root_dir/tools/build/elf.sh" "$script_dir/kernel" kernel
"${LLVM}llvm-objcopy" -O binary "$script_dir/kernel/kernel.elf" "$script_dir/kernel/kernel.bin"
"${LLVM}llvm-objcopy" -O binary "$script_dir/kernel/debug.kernel.elf" "$script_dir/kernel/debug.kernel.bin"

# Bootloader (with kernel binary embedded)
ARCH="x86_64" FLAGS="-I$script_dir/kernel" "$root_dir/tools/build/efi.sh" "$script_dir/bootloader" bootloader
