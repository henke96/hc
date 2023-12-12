#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

export NO_AARCH64=1 NO_RISCV64=1

# Kernel
export FLAGS="$("$root_dir/tools/shellUtil/escape.sh" "-Wl,-T$script_dir/kernel/kernel.ld") -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
"$root_dir/tools/build/elf.sh" "$script_dir/kernel" kernel

"${llvm_prefix}llvm-objcopy" -O binary "$script_dir/kernel/x86_64/kernel.elf" "$script_dir/kernel/x86_64/kernel.bin"
"${llvm_prefix}llvm-objcopy" -O binary "$script_dir/kernel/x86_64/debug.kernel.elf" "$script_dir/kernel/x86_64/debug.kernel.bin"

# Bootloader (with kernel binary embedded)
export FLAGS="" FLAGS_X86_64="$("$root_dir/tools/shellUtil/escape.sh" "-I$script_dir/kernel/x86_64")"
"$root_dir/tools/build/efi.sh" "$script_dir/bootloader" bootloader
