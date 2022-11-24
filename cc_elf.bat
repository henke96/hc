@echo off
setlocal
set /p flags=<%~dp0flags
if not defined CC set CC=clang
if not defined LD set LD=ld.lld
if not defined ARCH set ARCH=x86_64
if not defined ABI set ABI=elf
"%CC%" -I"%~dp0src" %flags% -target %ARCH%-unknown-linux-%ABI% --ld-path="%LD%" -Wl,--build-id=none %*
endlocal
