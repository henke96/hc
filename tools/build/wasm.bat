@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "prog_path=%~1"
set "prog_name=%~2"

set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
set "debug_flags=-fsanitize-undefined-trap-on-error -fsanitize=undefined -g"
set "release_flags=-Ddebug_NDEBUG -fomit-frame-pointer -s -Os"

if defined ASSEMBLY (
    call "%root_dir%\cc_wasm.bat" %debug_flags% -S -o "%prog_path%\debug.%prog_name%.wasm.s" "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
    call "%root_dir%\cc_wasm.bat" %release_flags% -S -o "%prog_path%\%prog_name%.wasm.s" "%prog_path%\%prog_name%.c" %FLAGS%
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
call "%root_dir%\cc_wasm.bat" %debug_flags% -o "%prog_path%\debug.%prog_name%.wasm" "%prog_path%\%prog_name%.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%\cc_wasm.bat" %release_flags% -o "%prog_path%\%prog_name%.wasm" "%prog_path%\%prog_name%.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

call "%root_dir%\cc_wasm.bat" %debug_flags% %analyse_flags% "%prog_path%\%prog_name%.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%\cc_wasm.bat" %release_flags% %analyse_flags% "%prog_path%\%prog_name%.c" %FLAGS%
