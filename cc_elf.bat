@echo off
setlocal
set /p flags=<%~dp0flags
if not defined CC set CC=clang
if not defined LD set LD=lld
if not defined ARCH set ARCH=x86_64
if not defined FORMAT set FORMAT=elf
%CC% %flags% -target %ARCH%-unknown-linux-%FORMAT% -fuse-ld="%LD%" -Wl,--gc-sections -Wl,--build-id=none %*
endlocal
