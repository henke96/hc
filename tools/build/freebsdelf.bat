@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "prog_path=%~1"

if defined LINK_LIBC set "FLAGS=-l:libc.so.7 %FLAGS%"

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

set "ABI=freebsd14"
set "FLAGS=-fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic %FLAGS%"
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
    call "%root_dir%\tools\genLib\gen_so.bat" "%root_dir%\src\hc\freebsd\libc.so.7.c" "%prog_path%\%ARCH%\libc.so.7"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
exit /b
