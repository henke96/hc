#!/bin/sh
set -e
flags="-Wall -Wextra -Wconversion -Wshadow -Wpadded -Werror -Wno-maybe-uninitialized -Wno-unknown-warning-option -Wno-unused-command-line-argument \
-fshort-wchar -fno-pie -nostdinc -nostdlib -ffreestanding -static -fno-asynchronous-unwind-tables -fcf-protection=none -fno-stack-check -fno-stack-protector -mno-stack-arg-probe -Qn \
-nostartfiles -Wl,--gc-sections -Wl,--build-id=none"
CC="${CC:-clang}"
LD="${LD:-lld}"
ARCH="${ARCH:-x86_64}"
BINFMT="${BINFMT:-linux-elf}"
"$CC" $flags -target $ARCH-unknown-$BINFMT -fuse-ld="$LD" "$@"
