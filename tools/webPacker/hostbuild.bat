@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "name=webPacker"
set "NO_X86_64=1"
set "NO_AARCH64=1"
set "NO_RISCV64=1"
if "%processor_architecture%" == "AMD64" (
    set "NO_X86_64="
    set "arch=x86_64"
) else if "%processor_architecture%" == "AARCH64" (
    set "NO_AARCH64="
    set "arch=aarch64"
) else exit /b 1

set "NO_LINUX=1"
set "NO_FREEBSD=1"
set "NO_WINDOWS="
set "abi=windows-gnu"

set "NO_DEBUG=1"
set "NO_ANALYSIS=1"
call "%script_dir%\build.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

move /y "%OUT%\%arch%-%abi%_%name%.exe" "%OUT%\%name%.exe" >nul
