#!/bin/sh
set -e

if test -z "$1" || test -z "$2"
then
    echo "Usage: $0 INPUT.def OUTPUT.a"
    exit 1
fi

DLLTOOL="${DLLTOOL:-llvm-dlltool}$LLVM"
ARCH="${ARCH:-x86_64}"
if test "$ARCH" = "x86_64"; then
    arch="i386:x86-64"
elif test "$ARCH" = "aarch64"; then
    arch="arm64"
else
    echo "Invalid architecture"
    exit 1
fi

"$DLLTOOL" -m $arch -d "$1" -l "$2"
