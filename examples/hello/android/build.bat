@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\..\"

set LINK_LIBDL=1 & set LINK_LIBLOG=1
call "%root_dir%tools\build\androidelf.bat" "%~dp0" hello elf
