set -e
flags="-Wall -Wextra -Wconversion -Wshadow -Wpadded -Werror -Wno-maybe-uninitialized -Wno-unknown-warning-option -Wno-unused-command-line-argument \
-fno-pie -nostdlib -ffreestanding -static -fno-asynchronous-unwind-tables -fcf-protection=none -fno-stack-protector -Qn \
-nostartfiles -Wl,--gc-sections -Wl,--build-id=none" # -Wl,--no-rosegment
debug_flags="$flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -g -O2 $FLAGS"
release_flags="$flags -Ddebug_NDEBUG -O2 -s $FLAGS"
CC=${CC:-gcc}
$CC $debug_flags -S -o debug.bin.s main.c
$CC $debug_flags -o debug.bin main.c
$CC $release_flags -S -o release.bin.s main.c
$CC $release_flags -o release.bin main.c
$CC $release_flags -c -o release.bin.o main.c
