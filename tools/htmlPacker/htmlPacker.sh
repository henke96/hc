#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

export ARCH="$(uname -m)"

case "$(uname)" in
    MINGW*|MSYS*|CYGWIN*)
        "$root_dir/tools/genLib/gen_lib.sh" "$root_dir/src/hc/windows/dll/kernel32.def" "$script_dir/windows/kernel32.lib"
        "$root_dir/cc_pe.sh" -L"$script_dir/windows/" "$script_dir/windows/htmlPacker.c" -o "$script_dir/windows/htmlPacker.exe" -l:kernel32.lib
        "$script_dir/windows/htmlPacker.exe" "$@"
        ;;
    *)
        "$root_dir/cc_elf.sh" "$script_dir/linux/htmlPacker.c" -o "$script_dir/linux/htmlPacker.elf"
        "$script_dir/linux/htmlPacker.elf" "$@"
        ;;
esac
