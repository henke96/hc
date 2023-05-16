@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\..\"

set LINK_LIBC=1 & set LINK_LIBDL=1
call "%root_dir%tools\build\gnulinuxelf.bat" "%~dp0" openGl
