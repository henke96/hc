#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
OUT="$(cd -- "$OUT" && pwd)"

for pkg in bash e2fsprogs nano ncurses util-linux; do
    "$script_dir/recipes/extra/$pkg"
    ! test -d "$OUT/$pkg/bin" || export PATH="$OUT/$pkg/bin:$PATH"
    ! test -d "$OUT/$pkg/sbin" || export PATH="$OUT/$pkg/sbin:$PATH"
done

for pkg in bc bison bzip2 cmake elfutils flex m4 make mtools perl python xorriso xz; do
    ! test -d "$OUT/$pkg/bin" || export PATH="$OUT/$pkg/bin:$PATH"
    ! test -d "$OUT/$pkg/sbin" || export PATH="$OUT/$pkg/sbin:$PATH"
done

exec "$OUT/bash/bin/bash" --rcfile "$OUT/bash/bashrc"
