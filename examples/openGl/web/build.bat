@echo off
setlocal
set "root_dir=%~dp0..\..\..\"

set "FLAGS=-Wl,--no-entry %FLAGS%"
call "%root_dir%tools\build\wasm.bat" "%~dp0" openGl
if %errorlevel% neq 0 exit /b

call "%root_dir%tools\htmlPacker\htmlPacker.bat" "%~dp0" _start.html openGl
