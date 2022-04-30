@echo off
setlocal
if not defined CC set CC=clang
if not defined LD set LD=lld
if not defined ARCH set ARCH=x86_64
%CC% -target %ARCH%-unknown-linux-elf -fuse-ld="%LD%" -shared -nostdlib -o %2%1 %~dp0%1.c
endlocal
