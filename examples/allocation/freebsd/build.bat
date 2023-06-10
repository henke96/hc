@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\..\.."

set LINK_LIBC=1
call "%root_dir%\tools\build\freebsdelf.bat" "%script_dir%" allocation
