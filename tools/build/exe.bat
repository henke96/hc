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

if defined LINK_KERNEL32 (
    call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%\src\hc\windows\dll\kernel32.def" "%out_dir%\kernel32.lib"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
if defined LINK_USER32 (
    call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%\src\hc\windows\dll\user32.def" "%out_dir%\user32.lib"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
if defined LINK_GDI32 (
    call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%\src\hc\windows\dll\gdi32.def" "%out_dir%\gdi32.lib"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
if defined LINK_SYNCHRONIZATION (
    call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%\src\hc\windows\dll\synchronization.def" "%out_dir%\synchronization.lib"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)

set "FLAGS=-L^"%out_dir%^" %FLAGS% %~1"
if defined ASSEMBLY (
    call "%root_dir%\cc_pe.bat" %debug_flags% -S -o "%out_dir%\debug-%source_name%%ext%.s" "%source%" %FLAGS%
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
    call "%root_dir%\cc_pe.bat" %release_flags% -S -o "%out_dir%\%source_name%%ext%.s" "%source%" %FLAGS%
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
call "%root_dir%\cc_pe.bat" %debug_flags% -o "%out_dir%\debug-%source_name%%ext%" "%source%" %FLAGS%
if not errorlevel 0 exit /b
if errorlevel 1 exit /b
call "%root_dir%\cc_pe.bat" %release_flags% -o "%out_dir%\%source_name%%ext%" "%source%" %FLAGS%
if not errorlevel 0 exit /b
if errorlevel 1 exit /b

if not defined NO_ANALYSIS (
    call "%root_dir%\cc_pe.bat" %debug_flags% %analyse_flags% "%source%" %FLAGS%
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
    call "%root_dir%\cc_pe.bat" %release_flags% %analyse_flags% "%source%" %FLAGS%
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
exit /b

:start
set "source=%~1"
set "source_name=%~n1"
set "out_path=%~2"
set "ext=%~3"

set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
set "common_flags=-Wl,-subsystem,windows"
set "debug_flags=%common_flags% -fsanitize-undefined-trap-on-error -fsanitize=undefined -g3 -gcodeview -Wl,--pdb= -Dhc_DEBUG"
set "release_flags=%common_flags% -fomit-frame-pointer -s -Os"

if defined LINK_KERNEL32 set "FLAGS=-l:kernel32.lib %FLAGS%"
if defined LINK_USER32 set "FLAGS=-l:user32.lib %FLAGS%"
if defined LINK_GDI32 set "FLAGS=-l:gdi32.lib %FLAGS%"
if defined LINK_SYNCHRONIZATION set "FLAGS=-l:synchronization.lib %FLAGS%"

if not defined NO_X86_64 (
    set "ARCH=x86_64" & call :build "%FLAGS_X86_64%"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
if not defined NO_AARCH64 (
    set "ARCH=aarch64" & call :build "%FLAGS_AARCH64%"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
