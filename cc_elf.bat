@echo off
setlocal disabledelayedexpansion
set /p flags=<"%~dp0flags"
if not defined ARCH set ARCH=x86_64
if not defined ABI set ABI=linux-elf
"%LLVM%clang" -I"%~dp0src" %flags% -target %ARCH%-unknown-%ABI% --ld-path="%LLVM%ld.lld" -Wl,-dynamic-linker="" -Wl,--build-id=none %*
