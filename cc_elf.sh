#!/bin/sh
set -e
flags="-Wall -Wextra -Wconversion -Wshadow -Wpadded -Werror -Wno-maybe-uninitialized -Wno-unknown-warning-option -Wno-unused-command-line-argument \
-nostdinc -nostdlib -nostartfiles -static \
-fshort-wchar -fno-pie -ffreestanding -fno-asynchronous-unwind-tables -ftls-model=local-exec -fcf-protection=none -fno-stack-check -fno-stack-protector -mno-stack-arg-probe -Qn \
-Wl,--gc-sections -Wl,--build-id=none"
CC="${CC:-clang}"
LD="${LD:-lld}"
ARCH="${ARCH:-x86_64}"
BINFMT="${BINFMT:-linux-elf}"
"$CC" $flags -target $ARCH-unknown-$BINFMT -fuse-ld="$LD" "$@"
