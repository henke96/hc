@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

call "%root_dir%tools\build\elf.bat" "%~dp0" tty
