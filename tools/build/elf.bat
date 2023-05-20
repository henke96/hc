@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

if defined LLVM set "llvm_prefix=%LLVM%\bin\"

set "prog_path=%~1"
set "prog_name=%~2"
if "%~3" == "" ( set "ext=elf" ) else set "ext=%~3"

if not defined STRIP_OPT set STRIP_OPT=--strip-sections

set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
set "debug_flags=-fsanitize-undefined-trap-on-error -fsanitize=undefined -g"
set "release_flags=-fomit-frame-pointer -Ddebug_NDEBUG -s -Os"

if not defined NO_X86_64 (
    set "ARCH=x86_64" & call :build "%FLAGS_X86_64%"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if not defined NO_AARCH64 (
    set "ARCH=aarch64" & call :build "%FLAGS_AARCH64%"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if not defined NO_RISCV64 (
    set "ARCH=riscv64" & call :build "%FLAGS_RISCV64%"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
exit /b

:build
setlocal
set "FLAGS=-L^"%prog_path%\%ARCH%^" %FLAGS% %~1"
if not exist "%prog_path%\%ARCH%" (
    mkdir "%prog_path%\%ARCH%"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)

if defined ASSEMBLY (
    call "%root_dir%\cc_elf.bat" %debug_flags% -S -o "%prog_path%\%ARCH%\debug.%prog_name%.%ext%.s" "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    call "%root_dir%\cc_elf.bat" %release_flags% -S -o "%prog_path%\%ARCH%\%prog_name%.%ext%.s" "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
call "%root_dir%\cc_elf.bat" %debug_flags% -o "%prog_path%\%ARCH%\debug.%prog_name%.%ext%" "%prog_path%\%prog_name%.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%\cc_elf.bat" %release_flags% -o "%prog_path%\%ARCH%\%prog_name%.%ext%" "%prog_path%\%prog_name%.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
"%llvm_prefix%llvm-objcopy" %STRIP_OPT% "%prog_path%\%ARCH%\%prog_name%.%ext%"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

if not defined NO_ANALYSIS (
    call "%root_dir%\cc_elf.bat" %debug_flags% %analyse_flags% "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    call "%root_dir%\cc_elf.bat" %release_flags% %analyse_flags% "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
exit /b
