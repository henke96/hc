@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "prog_path=%~1"
set "prog_name=%~2"
if "%~3" == "" ( set "ext=exe" ) else set "ext=%~3"

set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
set "common_flags=-L^"%prog_path%^" -Wl,-subsystem,windows"
set "debug_flags=%common_flags% -fsanitize-undefined-trap-on-error -fsanitize=undefined -g3 -gcodeview -Wl,--pdb="
set "release_flags=%common_flags% -fomit-frame-pointer -Ddebug_NDEBUG -s -Os"

if defined LINK_KERNEL32 set "FLAGS=-l:kernel32.lib %FLAGS%"
if defined LINK_USER32 set "FLAGS=-l:user32.lib %FLAGS%"
if defined LINK_GDI32 set "FLAGS=-l:gdi32.lib %FLAGS%"

if not defined NO_X86_64 (
    set "ARCH=x86_64" & call :build "%FLAGS_X86_64%"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if not defined NO_AARCH64 (
    set "ARCH=aarch64" & call :build "%FLAGS_AARCH64%"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
exit /b

:build
setlocal
if not exist "%prog_path%\%ARCH%" (
    mkdir "%prog_path%\%ARCH%"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if defined LINK_KERNEL32 (
    call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%\src\hc\windows\dll\kernel32.def" "%prog_path%\%ARCH%\kernel32.lib"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if defined LINK_USER32 (
    call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%\src\hc\windows\dll\user32.def" "%prog_path%\%ARCH%\user32.lib"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
if defined LINK_GDI32 (
    call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%\src\hc\windows\dll\gdi32.def" "%prog_path%\%ARCH%\gdi32.lib"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)

set "FLAGS=-L^"%prog_path%\%ARCH%^" %FLAGS% %~1"

if defined ASSEMBLY (
    call "%root_dir%\cc_pe.bat" %debug_flags% -S -o "%prog_path%\%ARCH%\debug.%prog_name%.%ext%.s" "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    call "%root_dir%\cc_pe.bat" %release_flags% -S -o "%prog_path%\%ARCH%\%prog_name%.%ext%.s" "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
call "%root_dir%\cc_pe.bat" %debug_flags% -o "%prog_path%\%ARCH%\debug.%prog_name%.%ext%" "%prog_path%\%prog_name%.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%\cc_pe.bat" %release_flags% -o "%prog_path%\%ARCH%\%prog_name%.%ext%" "%prog_path%\%prog_name%.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

if not defined NO_ANALYSIS (
    call "%root_dir%\cc_pe.bat" %debug_flags% %analyse_flags% "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    call "%root_dir%\cc_pe.bat" %release_flags% %analyse_flags% "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
exit /b