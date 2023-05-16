@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

if "%~3" == "" (
    set "ext=exe"
) else (
    set "ext=%~3"
)

call "%root_dir%tools\genLib\gen_lib.bat" "%root_dir%src\hc\windows\dll\kernel32.def" "%~1kernel32.lib"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%tools\genLib\gen_lib.bat" "%root_dir%src\hc\windows\dll\user32.def" "%~1user32.lib"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%tools\genLib\gen_lib.bat" "%root_dir%src\hc\windows\dll\gdi32.def" "%~1gdi32.lib"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

set "common_flags=-L^"%~1\^" -Wl,-subsystem,windows"
set "debug_flags=%common_flags% -fsanitize-undefined-trap-on-error -fsanitize=undefined -g3 -gcodeview -Wl,--pdb="
set "release_flags=%common_flags% -Ddebug_NDEBUG -s -Os"
call "%root_dir%cc_pe.bat" %debug_flags% -S -o "%~1debug.%~2.%ext%.s" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_pe.bat" %debug_flags% -o "%~1debug.%~2.%ext%" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_pe.bat" %release_flags% -S -o "%~1%~2.%ext%.s" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_pe.bat" %release_flags% -o "%~1%~2.%ext%" "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

:: Static analysis.
set "analyse_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
call "%root_dir%cc_pe.bat" %debug_flags% %analyse_flags% "%~1%~2.c" %FLAGS%
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%cc_pe.bat" %release_flags% %analyse_flags% "%~1%~2.c" %FLAGS%
