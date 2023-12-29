@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "out_path=%~1"
set "out_dir=%root_dir%\..\hc-out\%out_path%"
if not exist "%out_dir%" (
    mkdir "%out_dir%"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)

call "%root_dir%\tools\shell\setnativearch.bat"
if not errorlevel 0 exit /b
if errorlevel 1 exit /b

call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%\src\hc\windows\dll\kernel32.def" "%out_dir%\kernel32.lib"
if not errorlevel 0 exit /b
if errorlevel 1 exit /b
call "%root_dir%\cc_pe.bat" -L"%out_dir%" -o "%out_dir%\htmlPacker.exe" "%script_dir%\windows\htmlPacker.c" -l:kernel32.lib
