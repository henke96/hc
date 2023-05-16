@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

if defined LINK_LIBC (
    call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\gnulinux\libc.so.6.c" "%~1libc.so.6"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    set "FLAGS=-l:libc.so.6 %FLAGS%"
)
if defined LINK_LIBDL (
    call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\gnulinux\libdl.so.2.c" "%~1libdl.so.2"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    set "FLAGS=-l:libdl.so.2 %FLAGS%"
)

set "FLAGS=-fPIC -L^"%~1\^" %FLAGS%"
set "STRIP_OPT=--strip-sections"
call "%~dp0elf.bat" %*
