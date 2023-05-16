@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

if "%~3" == "" (
    set "ext=elf"
) else (
    set "ext=%~3"
)

if not defined STRIP_OPT set STRIP_OPT=--strip-sections

set "debug_flags=-fsanitize-undefined-trap-on-error -fsanitize=undefined -g"
set "release_flags=-Ddebug_NDEBUG -s -Os"

if defined ASSEMBLY (
    call "%root_dir%cc_elf.bat" %debug_flags% -S -o "%~1debug.%~2.%ext%.s" "%~1%~2.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    call "%root_dir%cc_elf.bat" %release_flags% -S -o "%~1%~2.%ext%.s" "%~1%~2.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
call "%root_dir%cc_elf.bat" %debug_flags% -o "%~1debug.%~2.%ext%" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_elf.bat" %release_flags% -o "%~1%~2.%ext%" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
if defined STRIP_OPT (
    "%LLVM%llvm-objcopy" %STRIP_OPT% "%~1%~2.%ext%"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)

set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
call "%root_dir%cc_elf.bat" %debug_flags% %analyse_flags% "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_elf.bat" %release_flags% %analyse_flags% "%~1%~2.c" %FLAGS%
