@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

set "common_flags=-Wl,-subsystem,efi_application"
set "debug_flags=%common_flags% -fsanitize-undefined-trap-on-error -fsanitize=undefined"
set "release_flags=%common_flags% -Ddebug_NDEBUG -s -Os"
call "%root_dir%cc_pe.bat" %debug_flags% -S -o "%~1debug.%~2.efi.s" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_pe.bat" %debug_flags% -o "%~1debug.%~2.efi" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_pe.bat" %release_flags% -S -o "%~1%~2.efi.s" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_pe.bat" %release_flags% -o "%~1%~2.efi" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

:: Static analysis.
set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
call "%root_dir%cc_pe.bat" %debug_flags% %analyse_flags% "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_pe.bat" %release_flags% %analyse_flags% "%~1%~2.c" %FLAGS%
