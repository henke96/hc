@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"

if defined LLVM set "llvm_prefix=%LLVM%\bin\"

set "base_flags=-Weverything -Wno-pedantic -Wnewline-eof -Wno-c99-compat -Wno-unused-command-line-argument -Wno-unknown-warning-option -Wno-unsafe-buffer-usage -Wno-declaration-after-statement -Wno-undef -Wno-reserved-identifier -Wno-missing-prototypes -Wno-missing-variable-declarations -Wno-used-but-marked-unused -Wno-covered-switch-default -Wno-switch-enum -Wno-disabled-macro-expansion -Wno-unused-macros -nostdinc -nostdlib -nostartfiles --rtlib=compiler-rt -funsigned-char -fvisibility=hidden -fno-semantic-interposition -fno-PIC -no-pie -fno-plt -ffreestanding -ffunction-sections -fno-common -fno-asynchronous-unwind-tables -fcf-protection=none -fno-stack-check -fno-stack-protector -fno-strict-aliasing -mno-stack-arg-probe -Qn --sysroot=/path/that/doesnt/exist -Wl,--gc-sections"
if "%ARCH%" == "wasm32" (
    set "extra_flags=-mmultivalue -mbulk-memory -Wl,-zstack-size=0x20000,--export-dynamic"
) else (
    if "%ABI%" == "windows-gnu" (
        set "extra_flags=-Wl,--no-insert-timestamp,--entry=_start -Xlinker --stack=0x100000,0x20000"
    ) else (
        set "extra_flags=-Wl,-dynamic-linker=,--build-id=none,--hash-style=gnu,-znognustack,-znorelro,-znow"
    )
)

if "%ARCH%" == "aarch64" (
    set "extra_flags=%extra_flags% -mno-outline-atomics"
)

"%llvm_prefix%clang" -target "%ARCH%-unknown-%ABI%" -fuse-ld=lld -I "%script_dir%\src" %base_flags% %extra_flags% %*
