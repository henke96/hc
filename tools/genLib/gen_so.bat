@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "input=%~1"
set "output=%~2"
if not "%~3" == "" set "version_script_arg=-Wl,--version-script=%~3"

"%root_dir%\cc_elf.bat" -shared %version_script_arg% -o "%output%" "%input%"
