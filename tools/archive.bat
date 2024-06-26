@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\.."

if not defined OUT (
    echo Please set OUT
    exit /b 1
)

call "%root_dir%\tools\tar\build.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
set "prefix=hc_%date%"
"%OUT%\tar.exe" -o "%OUT%\%prefix%.tar" -d "%prefix%" -p "%prefix%" -a "%root_dir%"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
del "%OUT%\tar.exe"
