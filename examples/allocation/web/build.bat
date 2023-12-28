@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\..\.."

set "FLAGS=-Wl,--no-entry %FLAGS%"
call "%root_dir%\tools\build\wasm.bat" "%script_dir%\allocation.c" allocation\web .wasm
if not errorlevel 0 exit /b
if errorlevel 1 exit /b

call "%root_dir%\tools\htmlPacker\build.bat" allocation\web
if not errorlevel 0 exit /b
if errorlevel 1 exit /b

"%root_dir%\..\hc-out\allocation\web\htmlPacker.exe" "%script_dir%\_start.html" "%root_dir%\..\hc-out\allocation\web" allocation
