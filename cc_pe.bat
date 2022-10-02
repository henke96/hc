@echo off
setlocal
set /p flags=<%~dp0flags
if not defined CC set CC=clang
if not defined LD set LD=lld
if not defined ARCH set ARCH=x86_64
%CC% -I%~dp0/src %flags% -target %ARCH%-unknown-windows-gnu -fuse-ld="%LD%" -Wl,--no-insert-timestamp -Wl,-e,_start -L"%~dp0src\hc\windows\lib" %*
endlocal