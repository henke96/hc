@echo off
setlocal
set "root_dir=%~dp0..\..\"

set "FLAGS=-l:libdl.so -l:liblog.so"
call "%root_dir%tools\build\androidelf.bat" "%~dp0" android elf
