@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\..\"

set "FLAGS=-Wl,--no-entry %FLAGS%"
call "%root_dir%tools\build\wasm.bat" "%~dp0" allocation
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

call "%root_dir%tools\htmlPacker\htmlPacker.bat" "%~dp0" _start.html allocation
