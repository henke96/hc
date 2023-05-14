@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\..\"

set "FLAGS=-l:libc.so.6 %FLAGS%"
call "%root_dir%tools\build\gnulinuxelf.bat" "%~dp0" hello
