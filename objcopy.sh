#!/bin/sh --
set -e
if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi
"${llvm_prefix}llvm-objcopy" "$@"
