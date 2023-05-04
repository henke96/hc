@echo off
setlocal
set "root_dir=%~dp0..\..\"

set "FLAGS=-l:libc.so.7 %FLAGS%"
call "%root_dir%tools\build\freebsdelf.bat" "%~dp0" freebsdHello
