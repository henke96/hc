#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"

export OUT="$script_dir/../../hc-out/bootstrap"
mkdir -p "$OUT"
"$script_dir/llvm"
