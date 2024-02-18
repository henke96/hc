#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../../.."

# OpenJDK complains if stack is executable.
export FLAGS="-shared -fPIC -Wl,-znoexecstack $FLAGS"
"$root_dir/tools/build/linuxelf.sh" "$script_dir/libtest.c" jni/linux .so
