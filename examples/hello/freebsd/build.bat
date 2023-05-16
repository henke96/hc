@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\..\"

set LINK_LIBC=1
call "%root_dir%tools\build\freebsdelf.bat" "%~dp0" hello
