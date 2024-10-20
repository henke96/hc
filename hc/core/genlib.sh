#!/bin/sh --
set -e

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

if test "$ARCH" = "x86_64"; then
    machine="x64"
elif test "$ARCH" = "aarch64"; then
    machine="arm64"
else
    exit 1
fi

"${llvm_prefix}lld-link" -machine:"$machine" -out:"$1" -def:"$2"
