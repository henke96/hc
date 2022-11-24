@echo off
setlocal
set /p flags=<%~dp0flags
if not defined CC set CC=clang
if not defined LD set LD=ld.lld
"%CC%" -I"%~dp0src" %flags% -target wasm32-unknown-unknown-unknown --ld-path="%LD%" %*
endlocal