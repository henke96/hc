@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

if defined LINK_LIBLOG (
    call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\android\liblog.so.c" "%~1liblog.so"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    set "FLAGS=-l:liblog.so %FLAGS%"
)
if defined LINK_LIBDL (
    call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\android\libdl.so.c" "%~1libdl.so"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    set "FLAGS=-l:libdl.so %FLAGS%"
)
if defined LINK_LIBANDROID (
    call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\android\libandroid.so.c" "%~1libandroid.so"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    set "FLAGS=-l:libandroid.so %FLAGS%"
)
if defined LINK_LIBC (
    call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\android\libc.so.c" "%~1libc.so"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    set "FLAGS=-l:libc.so %FLAGS%"
)

set "ABI=linux-android26"
set "STRIP_OPT=--strip-all"
set "FLAGS=-fPIC -fpie -pie -Wl,-dynamic-linker=/system/bin/linker64 -L^"%~1\^" %FLAGS%"
call "%~dp0elf.bat" %*
