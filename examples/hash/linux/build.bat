@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\..\.."

set "FLAGS=-O2 %FLAGS%"
call "%root_dir%\tools\build\linuxelf.bat" "%script_dir%" hash
