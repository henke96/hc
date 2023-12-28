@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."
goto start

:build
setlocal
set "out_dir=%root_dir%\..\hc-out\%out_path%\%ARCH%"
if not exist "%out_dir%" (
    mkdir "%out_dir%"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)

if defined LINK_LIBC (
    call "%root_dir%\tools\genLib\gen_so.bat" "%root_dir%\src\hc\freebsd\libc.so.7.c" "%out_dir%\libc.so.7"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
exit /b

:start
set "out_path=%~2"

if defined LINK_LIBC set "FLAGS=-l:libc.so.7 %FLAGS%"

set "ABI=freebsd14"
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

set "FLAGS=-fPIC -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -Wl,--export-dynamic %FLAGS%"
if not defined STRIP_OPT set STRIP_OPT=--strip-sections
call "%script_dir%\elf.bat" %*
