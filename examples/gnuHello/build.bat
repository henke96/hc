@echo off
setlocal
set "root_dir=%~dp0..\..\"

set "FLAGS=-l:libc.so.6"
call "%root_dir%tools\build\gnuelf.bat" "%~dp0" gnuHello
