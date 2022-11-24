#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 INPUT.def OUTPUT.lib"
    exit 1
fi

LLD_LINK="${LLD_LINK:-lld-link}$LLVM"
ARCH="${ARCH:-x86_64}"
if test "$ARCH" = "x86_64"; then
    machine="x64"
elif test "$ARCH" = "aarch64"; then
    machine="arm64"
else
    echo "Invalid architecture"
    exit 1
fi

"$LLD_LINK" -machine:$machine -def:"$1" -out:"$2"