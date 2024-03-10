#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

test -n "$OUT" || { echo "Please set OUT"; exit 1; }

ARCH="$(uname -m)"
ABI=linux

if test -n "$JAVA_HOME"; then java_prefix="$JAVA_HOME/bin/"; fi

cp "$OUT/debug_$ARCH-${ABI}_libtest.so" "$OUT/libtest.so"
"${java_prefix}javac" -d "$OUT" "$script_dir/jni/Test.java"
"${java_prefix}java" -cp "$OUT" -Djava.library.path="$OUT" jni/Test
