#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."

"$root_dir/tools/build/linuxelf.sh" "$script_dir" udp
