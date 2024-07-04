@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

if not defined OUT (
    echo Please set OUT
    exit /b 1
)

rem Kernel
set "ARCH=x86_64"
set "ABI=linux"
set "FLAGS=-Os -s -Wl,-T,^"%script_dir%\kernel\kernel.ld^" -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
set "FLAGS_RELEASE="
set "FLAGS_DEBUG="
call "%root_dir%\tools\builder.bat" "%script_dir%\kernel\kernel.elf.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

call "%root_dir%\objcopy.bat" -O binary "%OUT%\%ARCH%-%ABI%_kernel.elf" "%OUT%\kernel.bin"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
if not defined NO_DEBUG (
    call "%root_dir%\objcopy.bat" -O binary "%OUT%\debug_%ARCH%-%ABI%_kernel.elf" "%OUT%\debug_kernel.bin"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)

rem Bootloader (with kernel binary embedded)
set "ARCH=x86_64"
set "ABI=windows-gnu"
set "FLAGS=-Os -s -I ^"%OUT%^" -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wl,-subsystem,efi_application"
set "FLAGS_RELEASE="
set "FLAGS_DEBUG="
call "%root_dir%\tools\builder.bat" "%script_dir%\bootloader\bootloader.efi.c"
