#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

export NO_DEBUG=1
export NO_ANALYSIS=1
. "$root_dir/tools/hostbuild.sh"

"$script_dir/build.sh"

"$OUT/$HOST_ARCH-${HOST_ABI}_untar" "$@"
