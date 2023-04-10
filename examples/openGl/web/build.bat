@echo off
setlocal
set "root_dir=%~dp0..\..\..\"

set "FLAGS=-Wl,--no-entry"
call "%root_dir%tools\build\wasm.bat" "%~dp0" openGl
if %errorlevel% neq 0 exit /b

cd "%~dp0"
if %errorlevel% neq 0 exit /b
call "..\..\..\tools\htmlPacker\htmlPacker.bat" _start.html openGl
