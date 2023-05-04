@echo off
setlocal
set "root_dir=%~dp0..\..\"

call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\freebsd\libc.so.7.c" "%~1libc.so.7"
if %errorlevel% neq 0 exit /b

set "ABI=freebsd14"
set "FLAGS=-fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic -L^"%~1\^" %FLAGS%"
call "%~dp0elf.bat" %*
