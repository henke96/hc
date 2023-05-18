@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\..\.."

set LINK_KERNEL32=1 & set LINK_USER32=1 & set LINK_GDI32=1
call "%root_dir%\tools\build\exe.bat" "%script_dir%" openGl
