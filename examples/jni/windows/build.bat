@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\..\.."

set LINK_KERNEL32=1
set "FLAGS=-shared -fPIC %FLAGS%"
call "%root_dir%\tools\build\exe.bat" "%script_dir%\test.c" jni\windows .dll
