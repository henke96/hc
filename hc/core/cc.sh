#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"

if test -n "$LLVM"; then llvm_prefix="$LLVM/bin/"; fi

base_flags="-Weverything -Wno-pedantic -Wnewline-eof -Wno-c99-compat -Wno-pre-c11-compat -Wno-unused-function -Wno-unused-command-line-argument -Wno-unknown-warning-option -Wno-unsafe-buffer-usage -Wno-declaration-after-statement -Wno-undef -Wno-reserved-identifier -Wno-missing-prototypes -Wno-missing-variable-declarations -Wno-used-but-marked-unused -Wno-disabled-macro-expansion -Wno-unused-macros -Wno-switch -Wno-switch-default -Wno-covered-switch-default -nostdinc -nostdlib -nostartfiles --rtlib=compiler-rt -funsigned-char -fvisibility=hidden -fno-semantic-interposition -fno-PIC -no-pie -ffreestanding -ffunction-sections -fno-common -fno-asynchronous-unwind-tables -fcf-protection=none -fno-stack-check -fno-stack-protector -fno-strict-aliasing -mno-stack-arg-probe -Qn --sysroot=/path/that/doesnt/exist -Wl,--gc-sections"
if test "$ARCH" = wasm32; then
    extra_flags="-mmultivalue -mbulk-memory -Wl,-zstack-size=0x20000,--export-dynamic"
elif test "$ABI" = windows-gnu; then
    extra_flags="-Wl,--no-insert-timestamp,--entry=_start -Xlinker --stack=0x100000,0x20000"
else
    extra_flags="-fno-plt -Wl,-dynamic-linker=,--build-id=none,--hash-style=gnu,-znognustack,-znorelro,-znow"
fi

if test "$ARCH" = aarch64; then
    extra_flags="$extra_flags -mno-outline-atomics"
fi

"${llvm_prefix}clang" -target "$ARCH-unknown-$ABI" -fuse-ld=lld -I "$script_dir" $base_flags $extra_flags "$@"
