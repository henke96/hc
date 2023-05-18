@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"

if defined LLVM set "llvm_prefix=%LLVM%\bin\"

set /p flags=<"%script_dir%\flags"
"%llvm_prefix%clang" -I"%script_dir%\src" %flags% -target %ARCH%-unknown-windows-gnu --ld-path="%llvm_prefix%ld.lld" -Wl,--no-insert-timestamp -Wl,-e,_start %*
