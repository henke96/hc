@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\..\"

set "FLAGS=-l:libdl.so -l:liblog.so %FLAGS%"
call "%root_dir%tools\build\androidelf.bat" "%~dp0" hello elf
