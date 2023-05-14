@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

set "FLAGS=-shared -fPIC %FLAGS%"
call "%root_dir%tools\build\elf.bat" "%~dp0linux\" libtest so
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

set "FLAGS=-shared -fPIC -l:kernel32.lib %FLAGS%"
call "%root_dir%tools\build\exe.bat" "%~dp0windows\" test dll
