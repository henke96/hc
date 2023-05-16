@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\..\"

set LINK_KERNEL32=1
set "FLAGS=-shared -fPIC %FLAGS%"
call "%root_dir%tools\build\exe.bat" "%~dp0" test dll
