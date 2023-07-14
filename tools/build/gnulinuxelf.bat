@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "prog_path=%~1"

if defined LINK_LIBC set "FLAGS=-l:libc.so.6 %FLAGS%"
if defined LINK_LIBDL set "FLAGS=-l:libdl.so.2 %FLAGS%"

if not defined NO_X86_64 (
    set "ARCH=x86_64" & call :build
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
if not defined NO_AARCH64 (
    set "ARCH=aarch64" & call :build
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
if not defined NO_RISCV64 (
    set "ARCH=riscv64" & call :build
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)

rem Note: -fPIC seems needed for undefined weak symbols to work.
set "FLAGS=-fPIC %FLAGS%"
if not defined STRIP_OPT set STRIP_OPT=--strip-sections
call "%script_dir%\elf.bat" %*
exit /b

:build
setlocal
if not exist "%prog_path%\%ARCH%" (
    mkdir "%prog_path%\%ARCH%"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
if defined LINK_LIBC (
    call "%root_dir%\tools\genLib\gen_so.bat" "%root_dir%\src\hc\linux\gnulinux\libc.so.6.c" "%prog_path%\%ARCH%\libc.so.6"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
if defined LINK_LIBDL (
    call "%root_dir%\tools\genLib\gen_so.bat" "%root_dir%\src\hc\linux\gnulinux\libdl.so.2.c" "%prog_path%\%ARCH%\libdl.so.2"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
exit /b
