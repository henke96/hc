@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\.."

set LINK_KERNEL32=1
call "%root_dir%\tools\build\exe.bat" "%~dp0" hello
