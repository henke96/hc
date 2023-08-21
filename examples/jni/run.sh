#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"

ARCH="$(uname -m)"

if test -n "$JAVA_HOME"; then java_prefix="$JAVA_HOME/bin/"; fi

"${java_prefix}javac" "$script_dir/jni/Test.java"
"${java_prefix}java" -cp "$script_dir" -Djava.library.path="$script_dir/linux/$ARCH" jni/Test
