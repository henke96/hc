@echo off
setlocal
set /p flags=<%~dp0flags
if not defined CC set CC=clang
if not defined LD set LD=ld.lld
if not defined ARCH set ARCH=x86_64
"%CC%" -I"%~dp0src" %flags% -target %ARCH%-unknown-windows-gnu --ld-path="%LD%" -Wl,--no-insert-timestamp -Wl,-e,_start %*
endlocal