@echo off
setlocal
set /p flags=<"%~dp0flags"
if not defined ARCH set ARCH=x86_64
"%LLVM%clang" -I"%~dp0src" %flags% -target %ARCH%-unknown-windows-gnu --ld-path="%LLVM%ld.lld" -Wl,--no-insert-timestamp -Wl,-e,_start %*
