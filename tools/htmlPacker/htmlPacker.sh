#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

export ARCH="$(uname -m)"

case "$(uname)" in
    MINGW*|MSYS*|CYGWIN*)
        "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/kernel32.def" "$script_dir/windows/kernel32.lib"
        "$root_dir/cc_pe.sh" -L"$script_dir/windows/" "$script_dir/windows/htmlPacker.c" -o "$script_dir/windows/htmlPacker.exe" -l:kernel32.lib
        "$script_dir/windows/htmlPacker.exe" "$@"
        ;;
    FreeBSD)
        "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/freebsd/libc.so.7.c" "$script_dir/freebsd/libc.so.7"
        ABI="freebsd14" FLAGS="-fPIC" "$root_dir/cc_elf.sh" -fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic -L"$script_dir/freebsd/" "$script_dir/freebsd/htmlPacker.c" -o "$script_dir/freebsd/htmlPacker.elf" -l:libc.so.7
        "$script_dir/freebsd/htmlPacker.elf" "$@"
        ;;
    *)
        "$root_dir/cc_elf.sh" "$script_dir/linux/htmlPacker.c" -o "$script_dir/linux/htmlPacker.elf"
        "$script_dir/linux/htmlPacker.elf" "$@"
        ;;
esac
