@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\..\.."

set "FLAGS=-Wl,--no-entry %FLAGS%"
call "%root_dir%\tools\build\wasm.bat" "%script_dir%" allocation
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

call "%root_dir%\tools\htmlPacker\htmlPacker.bat" "%script_dir%" _start.html allocation
