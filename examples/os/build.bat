@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

set "ARCH=x86_64"

:: Kernel
set "FLAGS=-Wl,-T^"%~dp0kernel\kernel.ld^" -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
call "%root_dir%tools\build\elf.bat" "%~dp0kernel\" kernel
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

"%LLVM%llvm-objcopy" -O binary "%~dp0kernel\kernel.elf" "%~dp0kernel\kernel.bin"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
"%LLVM%llvm-objcopy" -O binary "%~dp0kernel\debug.kernel.elf" "%~dp0kernel\debug.kernel.bin"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

:: Bootloader (with kernel binary embedded)
set "FLAGS=-I^"%~dp0kernel\\^""
call "%root_dir%tools\build\efi.bat" "%~dp0bootloader\" bootloader
