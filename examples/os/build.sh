#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

# Kernel
export FLAGS="-Wl,-T$script_dir/kernel/kernel.ld -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
NO_AARCH64=1 NO_RISCV64=1 "$root_dir/tools/build/elf.sh" "$script_dir/kernel" kernel
"${LLVM}llvm-objcopy" -O binary "$script_dir/kernel/x86_64/kernel.elf" "$script_dir/kernel/x86_64/kernel.bin"
"${LLVM}llvm-objcopy" -O binary "$script_dir/kernel/x86_64/debug.kernel.elf" "$script_dir/kernel/x86_64/debug.kernel.bin"

# Bootloader (with kernel binary embedded)
export FLAGS="" FLAGS_X86_64="-I$script_dir/kernel/x86_64"
NO_AARCH64=1 "$root_dir/tools/build/efi.sh" "$script_dir/bootloader" bootloader
