@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "source=%~1"
set "source_name=%~n1"
set "out_path=%~2"
set "ext=%~3"

set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
set "debug_flags=-fsanitize-undefined-trap-on-error -fsanitize=undefined -g -Dhc_DEBUG"
set "release_flags=-fomit-frame-pointer -s -Os"

set "out_dir=%root_dir%\..\hc-out\%out_path%"
if not exist "%out_dir%" (
    mkdir "%out_dir%"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)

if defined ASSEMBLY (
    call "%root_dir%\cc_wasm.bat" %debug_flags% -S -o "%out_dir%\debug-%source_name%%ext%.s" "%source%" %FLAGS%
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
    call "%root_dir%\cc_wasm.bat" %release_flags% -S -o "%out_dir%\%source_name%%ext%.s" "%source%" %FLAGS%
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
call "%root_dir%\cc_wasm.bat" %debug_flags% -o "%out_dir%\debug-%source_name%%ext%" "%source%" %FLAGS%
if not errorlevel 0 exit /b
if errorlevel 1 exit /b
call "%root_dir%\cc_wasm.bat" %release_flags% -o "%out_dir%\%source_name%%ext%" "%source%" %FLAGS%
if not errorlevel 0 exit /b
if errorlevel 1 exit /b

call "%root_dir%\cc_wasm.bat" %debug_flags% %analyse_flags% "%source%" %FLAGS%
if not errorlevel 0 exit /b
if errorlevel 1 exit /b
call "%root_dir%\cc_wasm.bat" %release_flags% %analyse_flags% "%source%" %FLAGS%
