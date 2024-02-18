@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\..\.."

rem OpenJDK complains if stack is executable.
set "FLAGS=-shared -fPIC -Wl,-znoexecstack %FLAGS%"
call "%root_dir%\tools\build\linuxelf.bat" "%script_dir%\libtest.c" jni\linux .so
