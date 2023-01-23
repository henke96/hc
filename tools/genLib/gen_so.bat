@echo off
setlocal
set "root_dir=%~dp0..\.."

"%root_dir%\cc_elf.bat" -shared -o "%~2" "%~1"
endlocal
