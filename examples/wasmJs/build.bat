@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "FLAGS=-Wl,--no-entry"
call "%root_dir%\tools\build\wasm.bat" "%script_dir%" wasmJs
