#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

name=untar

. "$root_dir/src/shell/hostarch.sh"
if test "$hostarch" != "x86_64"; then export NO_X86_64=1; fi
if test "$hostarch" != "aarch64"; then export NO_AARCH64=1; fi
if test "$hostarch" != "riscv64"; then export NO_RISCV64=1; fi

export NO_LINUX=1
export NO_FREEBSD=1
export NO_WINDOWS=1
case "$(uname)" in
    FreeBSD)
    export NO_FREEBSD=
    hostabi=freebsd14
    ;;
    Linux)
    export NO_LINUX=
    hostabi=linux
    ;;
    *)
    exit 1
    ;;
esac

export NO_DEBUG=1
export NO_ANALYSIS=1
"$script_dir/build.sh"

mv "$OUT/$hostarch-${hostabi}_$name" "$OUT/$name"
