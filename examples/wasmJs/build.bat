@echo off
setlocal
set "root_dir=%~dp0..\..\"

set "FLAGS=-Wl,--no-entry"
call "%root_dir%tools\build\wasm.bat" "%~dp0" wasmJs
