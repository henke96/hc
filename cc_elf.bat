@echo off
setlocal
set /p flags=<"%~dp0flags"
if not defined ARCH set ARCH=x86_64
if not defined ABI set ABI=elf
"%LLVM%clang" -I"%~dp0src" %flags% -target %ARCH%-unknown-linux-%ABI% --ld-path="%LLVM%ld.lld" -Wl,-dynamic-linker="" -Wl,--build-id=none %*
