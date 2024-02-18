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
    "$root_dir/tools/genLib/gen_so.sh" "$root_dir/src/hc/freebsd/libc.so.7.c" "$out_dir/libc.so.7" "$root_dir/src/hc/freebsd/libc.so.7.map"
    "$root_dir/cc_elf.sh" -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L"$out_dir" "$script_dir/freebsd/tar.c" -o "$out_dir/tar" -l:libc.so.7
    ;;
    *)
    "$root_dir/cc_elf.sh" "$script_dir/linux/tar.c" -o "$out_dir/tar"
    ;;
esac
