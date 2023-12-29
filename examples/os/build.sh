#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. $root_dir/tools/shell/escape.sh

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

export NO_AARCH64=1 NO_RISCV64=1

# Kernel
export FLAGS="$(escape "-Wl,-T$script_dir/kernel/kernel.ld") -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
"$root_dir/tools/build/elf.sh" "$script_dir/kernel/kernel.c" os/kernel .elf

out_base_dir="$root_dir/../hc-out/os"

"${llvm_prefix}llvm-objcopy" -O binary "$out_base_dir/kernel/x86_64/kernel.elf" "$out_base_dir/kernel/x86_64/kernel.bin"
"${llvm_prefix}llvm-objcopy" -O binary "$out_base_dir/kernel/x86_64/debug-kernel.elf" "$out_base_dir/kernel/x86_64/debug-kernel.bin"

# Bootloader (with kernel binary embedded)
export FLAGS="" FLAGS_X86_64="$(escape "-I$out_base_dir/kernel/x86_64")"
"$root_dir/tools/build/efi.sh" "$script_dir/bootloader/bootloader.c" os .efi
