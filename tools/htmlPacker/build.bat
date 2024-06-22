@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

if not defined OUT (
    echo Please set OUT
    exit /b 1
)

call "%root_dir%\tools\shell\setnativearch.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

call "%root_dir%\genlib.bat" "%OUT%\ntdll.lib" "%root_dir%\src\hc\windows\dll\ntdll.def"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\genlib.bat" "%OUT%\kernel32.lib" "%root_dir%\src\hc\windows\dll\kernel32.def"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\cc.bat" -L "%OUT%" -o "%OUT%\htmlPacker.exe" -l:ntdll.lib -l:kernel32.lib "%script_dir%\windows\htmlPacker.c"
