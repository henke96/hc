@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\.."

set "prefix=hc_%date%"
call "%root_dir%\tools\tar\tar.bat" -o "%OUT%\%prefix%.tar" -d "%prefix%" -p "%prefix%" -a "%root_dir%"
