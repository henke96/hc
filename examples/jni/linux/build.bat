@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\..\"

set "FLAGS=-shared -fPIC %FLAGS%"
call "%root_dir%tools\build\elf.bat" "%~dp0" libtest so
