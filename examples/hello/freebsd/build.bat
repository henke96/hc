@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\..\"

set "FLAGS=-l:libc.so.7 %FLAGS%"
call "%root_dir%tools\build\freebsdelf.bat" "%~dp0" hello
