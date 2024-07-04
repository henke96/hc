#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

export NO_ANALYSIS=1
. "$root_dir/tools/hostbuild.sh"

"$script_dir/build.sh"

if test -n "$JAVA_HOME"; then java_prefix="$JAVA_HOME/bin/"; fi

"${java_prefix}javac" -d "$OUT" "$script_dir/jni/Test.java"

if test -z "$NO_DEBUG"; then
    cp "$OUT/debug_$HOST_ARCH-${HOST_ABI}_libtest.so" "$OUT/libtest.so"
    "${java_prefix}java" -cp "$OUT" -Djava.library.path="$OUT" jni/Test
fi
cp "$OUT/$HOST_ARCH-${HOST_ABI}_libtest.so" "$OUT/libtest.so"
"${java_prefix}java" -cp "$OUT" -Djava.library.path="$OUT" jni/Test
