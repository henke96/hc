@echo off
setlocal
set "root_dir=%~dp0..\..\"

set "common_flags=-O2"
set "debug_flags=%common_flags% -fsanitize-undefined-trap-on-error -fsanitize=undefined -g"
set "release_flags=%common_flags% -Ddebug_NDEBUG -s"
call "%root_dir%cc_wasm.bat" %debug_flags% -S -o "%~1debug.%~2.wasm.s" "%~1%~2.c" %FLAGS%
if %errorlevel% neq 0 exit /b
call "%root_dir%cc_wasm.bat" %debug_flags% -o "%~1debug.%~2.wasm" "%~1%~2.c" %FLAGS%
if %errorlevel% neq 0 exit /b
call "%root_dir%cc_wasm.bat" %release_flags% -S -o "%~1%~2.wasm.s" "%~1%~2.c" %FLAGS%
if %errorlevel% neq 0 exit /b
call "%root_dir%cc_wasm.bat" %release_flags% -o "%~1%~2.wasm" "%~1%~2.c" %FLAGS%
if %errorlevel% neq 0 exit /b

:: Static analysis.
set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
call "%root_dir%cc_wasm.bat" %debug_flags% %analyse_flags% "%~1%~2.c" %FLAGS%
if %errorlevel% neq 0 exit /b
call "%root_dir%cc_wasm.bat" %release_flags% %analyse_flags% "%~1%~2.c" %FLAGS%
