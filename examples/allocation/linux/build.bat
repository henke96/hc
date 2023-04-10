@echo off
setlocal
set "root_dir=%~dp0..\..\..\"

call "%root_dir%tools\build\elf.bat" "%~dp0" allocation
