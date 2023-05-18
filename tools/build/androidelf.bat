@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "prog_path=%~1"

if defined LINK_LIBLOG set "FLAGS=-l:liblog.so %FLAGS%"
if defined LINK_LIBDL set "FLAGS=-l:libdl.so %FLAGS%"
if defined LINK_LIBANDROID set "FLAGS=-l:libandroid.so %FLAGS%"
if defined LINK_LIBC set "FLAGS=-l:libc.so %FLAGS%"

if not defined NO_X86_64 (
    set "ARCH=x86_64" & call :build
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if not defined NO_AARCH64 (
    set "ARCH=aarch64" & call :build
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)

set NO_RISCV64=1
set "ABI=linux-android26" & set "STRIP_OPT=--strip-all"
set "FLAGS=-fPIC -fpie -pie -Wl,-dynamic-linker=/system/bin/linker64 %FLAGS%"
call "%script_dir%\elf.bat" %*
exit /b

:build
setlocal
if not exist "%prog_path%\%ARCH%" (
    mkdir "%prog_path%\%ARCH%"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if defined LINK_LIBLOG (
    call "%root_dir%\tools\genLib\gen_so.bat" "%root_dir%\src\hc\linux\android\liblog.so.c" "%prog_path%\%ARCH%\liblog.so"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if defined LINK_LIBDL (
    call "%root_dir%\tools\genLib\gen_so.bat" "%root_dir%\src\hc\linux\android\libdl.so.c" "%prog_path%\%ARCH%\libdl.so"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if defined LINK_LIBANDROID (
    call "%root_dir%\tools\genLib\gen_so.bat" "%root_dir%\src\hc\linux\android\libandroid.so.c" "%prog_path%\%ARCH%\libandroid.so"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if defined LINK_LIBC (
    call "%root_dir%\tools\genLib\gen_so.bat" "%root_dir%\src\hc\linux\android\libc.so.c" "%prog_path%\%ARCH%\libc.so"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
exit /b
