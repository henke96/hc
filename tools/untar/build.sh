#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

test -n "$OUT" || { echo "Please set OUT"; exit 1; }

export ARCH="$(uname -m)"

case "$(uname)" in
    FreeBSD)
    export ABI="freebsd14"
    "$root_dir/cc.sh" -fPIC -shared -Wl,--version-script="$root_dir/src/hc/freebsd/libc.so.7.map" -o "$OUT/libc.so.7" "$root_dir/src/hc/freebsd/libc.so.7.c"
    "$root_dir/cc.sh" -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L "$OUT" -o "$OUT/untar" -l:libc.so.7 "$script_dir/freebsd/untar.c"
    ;;
    *)
    export ABI="linux"
    "$root_dir/cc.sh" -o "$OUT/untar" "$script_dir/linux/untar.c"
    ;;
esac
