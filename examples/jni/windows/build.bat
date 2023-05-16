@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

set "FLAGS=-shared -fPIC -l:kernel32.lib %FLAGS%"
call "%root_dir%tools\build\exe.bat" "%~dp0windows\" test dll
