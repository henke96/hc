@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "input=%~1"
set "output=%~2"

"%root_dir%\cc_elf.bat" -shared -o "%output%" "%input%"
