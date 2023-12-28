#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"

ARCH="$(uname -m)"

if test -n "$JAVA_HOME"; then java_prefix="$JAVA_HOME/bin/"; fi

"${java_prefix}javac" -d "$root_dir/../hc-out/jni" "$script_dir/jni/Test.java"
"${java_prefix}java" -cp "$root_dir/../hc-out/jni" -Djava.library.path="$root_dir/../hc-out/jni/linux/$ARCH" jni/Test
