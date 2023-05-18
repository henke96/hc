@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

if defined LLVM set "llvm_prefix=%LLVM%\bin\"

set NO_AARCH64=1 & set NO_RISCV64=1

rem Kernel
set "FLAGS=-Wl,-T^"%script_dir%\kernel\kernel.ld^" -mno-red-zone -mno-mmx -mno-sse -mno-sse2"
call "%root_dir%\tools\build\elf.bat" "%script_dir%\kernel" kernel
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

"%llvm_prefix%llvm-objcopy" -O binary "%script_dir%\kernel\x86_64\kernel.elf" "%script_dir%\kernel\x86_64\kernel.bin"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
"%llvm_prefix%llvm-objcopy" -O binary "%script_dir%\kernel\x86_64\debug.kernel.elf" "%script_dir%\kernel\x86_64\debug.kernel.bin"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

rem Bootloader (with kernel binary embedded)
set "FLAGS=" & set "FLAGS_X86_64=-I^"%script_dir%\kernel\x86_64^""
call "%root_dir%\tools\build\efi.bat" "%script_dir%\bootloader" bootloader
