@echo off
setlocal
set "root_dir=%~dp0..\..\..\"

call "%root_dir%tools\build\wasm.bat" "%~dp0" hello