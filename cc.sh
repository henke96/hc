#!/bin/sh
set -e
flags="-Wall -Wextra -Wconversion -Wshadow -Wpadded -Werror -Wno-maybe-uninitialized -Wno-unknown-warning-option -Wno-unused-command-line-argument \
-fshort-wchar -fno-pie -nostdlib -ffreestanding -static -fno-asynchronous-unwind-tables -fcf-protection=none -fno-stack-protector -Qn \
-nostartfiles -Wl,--gc-sections -Wl,--build-id=none"
CC=${CC:-gcc}
$CC $flags "$@"
