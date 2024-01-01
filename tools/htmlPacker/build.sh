#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

out_path="$1"
out_dir="$root_dir/../hc-out/$out_path"
mkdir -p "$out_dir"

export ARCH="$(uname -m)"

case "$(uname)" in
    FreeBSD)
    export ABI="freebsd14"
    "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/freebsd/libc.so.7.c" "$out_dir/libc.so.7"
    FLAGS="-fPIC" "$root_dir/cc_elf.sh" -fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic -L"$out_dir" "$script_dir/freebsd/htmlPacker.c" -o "$out_dir/htmlPacker" -l:libc.so.7
    ;;
    *)
    "$root_dir/cc_elf.sh" "$script_dir/linux/htmlPacker.c" -o "$out_dir/htmlPacker"
    ;;
esac
