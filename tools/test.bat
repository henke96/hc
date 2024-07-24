@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\.."

for /r "%root_dir%" %%f in (*build.bat) do (
    echo %%f
    call "%%f"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)

if "%processor_architecture%" == "AMD64" (
    set "arch=x86_64"
) else if "%processor_architecture%" == "AARCH64" (
    set "arch=aarch64"
) else exit /b 1

set "abi=window-gnu"

if not defined NO_DEBUG (
    "%OUT%\debug_%arch%-%abi%_tests.exe" %*
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
"%OUT%\%arch%-%abi%_tests.exe" %*
