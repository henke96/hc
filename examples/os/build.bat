@echo off
setlocal
set "root_dir=%~dp0..\..\"

:: Kernel
if not defined ABI set ABI=elf
set "flags=-Wl,-T^"%~dp0kernel\kernel.ld^" -mno-red-zone -O2 -s"
call "%root_dir%cc_elf.bat" %flags% -S -o "%~dp0kernel\kernel.bin.s" "%~dp0kernel\kernel.c"
if %errorlevel% neq 0 exit /b
call "%root_dir%cc_elf.bat" %flags% -o "%~dp0kernel\kernel.bin.elf" "%~dp0kernel\kernel.c"
if %errorlevel% neq 0 exit /b

:: Static analysis.
set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
call "%root_dir%cc_elf.bat" %flags% %analyse_flags% "%~dp0kernel\kernel.c"
if %errorlevel% neq 0 exit /b

"%LLVM%llvm-objcopy" -O binary "%~dp0kernel\kernel.bin.elf" "%~dp0kernel\kernel.bin"
if %errorlevel% neq 0 exit /b

:: Bootloader (with kernel binary embedded)
set "FLAGS=-I^"%~dp0kernel\\^" -Os"
call "%root_dir%tools\build\efi.bat" "%~dp0bootloader\" bootloader
endlocal
