#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

arch="$(uname -m)"

if test "$arch" != "x86_64"; then export NO_X86_64=1; fi
if test "$arch" != "aarch64"; then export NO_AARCH64=1; fi
if test "$arch" != "riscv64"; then export NO_RISCV64=1; fi

export NO_LINUX=1
export NO_FREEBSD=1
export NO_WINDOWS=1
case "$(uname)" in
    FreeBSD)
    export NO_FREEBSD=
    abi=freebsd14
    ;;
    Linux)
    export NO_LINUX=
    abi=linux
    ;;
    *)
    exit 1
    ;;
esac

export NO_ANALYSIS=1
"$script_dir/build.sh"

if test -n "$JAVA_HOME"; then java_prefix="$JAVA_HOME/bin/"; fi

"${java_prefix}javac" -d "$OUT" "$script_dir/jni/Test.java"

if test -z "$NO_DEBUG"; then
    cp "$OUT/debug_$arch-${abi}_libtest.so" "$OUT/libtest.so"
    "${java_prefix}java" -cp "$OUT" -Djava.library.path="$OUT" jni/Test
fi
cp "$OUT/$arch-${abi}_libtest.so" "$OUT/libtest.so"
"${java_prefix}java" -cp "$OUT" -Djava.library.path="$OUT" jni/Test
