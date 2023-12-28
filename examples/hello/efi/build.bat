@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\..\.."

call "%root_dir%\tools\build\efi.bat" "%script_dir%\hello.c" hello\efi .efi
