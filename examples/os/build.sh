#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

# Kernel
abi="${ABI:-elf}" # Can also try gnux32.
flags="-Wl,-T$script_dir/kernel/kernel.ld -mno-red-zone -O2 -s"
ARCH="x86_64" ABI="$abi" "$root_dir/cc_elf.sh" $flags -S -o "$script_dir/kernel/kernel.bin.s" "$script_dir/kernel/kernel.c"
ARCH="x86_64" ABI="$abi" "$root_dir/cc_elf.sh" $flags -o "$script_dir/kernel/kernel.bin.elf" "$script_dir/kernel/kernel.c"

# Static analysis.
analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
ARCH="x86_64" "$root_dir/cc_elf.sh" $flags $analyse_flags "$script_dir/kernel/kernel.c"

"${LLVM}llvm-objcopy" -O binary "$script_dir/kernel/kernel.bin.elf" "$script_dir/kernel/kernel.bin"

# Bootloader (with kernel binary embedded)
ARCH="x86_64" FLAGS="-I$script_dir/kernel -Os" "$root_dir/tools/build/efi.sh" "$script_dir/bootloader" bootloader
