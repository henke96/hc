@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

if defined LINK_LIBC (
    call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\freebsd\libc.so.7.c" "%~1libc.so.7"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    set "FLAGS=-l:libc.so.7 %FLAGS%"
)

set "ABI=freebsd14"
set "FLAGS=-fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic -L^"%~1\^" %FLAGS%"
set "STRIP_OPT=--strip-sections"
call "%~dp0elf.bat" %*
