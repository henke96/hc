#!/bin/sh --
set -e

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

input="$1"
output="$2"

if test "$ARCH" = "x86_64"; then
    machine="x64"
elif test "$ARCH" = "aarch64"; then
    machine="arm64"
else
    echo "Invalid architecture"
    exit 1
fi

"${llvm_prefix}lld-link" -machine:"$machine" -def:"$input" -out:"$output"
