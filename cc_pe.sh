#!/bin/sh
set -e
script_dir="$(dirname $0)"
BINFMT="windows-coff" "$script_dir/cc_elf.sh" -Wl,-entry:main -L"$script_dir/src/windows/lib" "$@"
