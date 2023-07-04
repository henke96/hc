@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

if not defined STRIP_OPT set STRIP_OPT=--strip-sections
call "%script_dir%\elf.bat" %*
