#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

name=webPacker

. "$root_dir/core/shell/hostarch.sh"
export NO_X86_64=1
export NO_AARCH64=1
export NO_RISCV64=1
case "$hostarch" in
    x86_64) export NO_X86_64= ;;
    aarch64) export NO_AARCH64= ;;
    riscv64) export NO_RISCV64= ;;
    *) exit 1 ;;
esac

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
