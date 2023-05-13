@echo off
setlocal
set "root_dir=%~dp0..\..\"

set "FLAGS=-shared -fPIC %FLAGS%"
call "%root_dir%tools\build\elf.bat" "%~dp0linux\" libtest so
if %errorlevel% neq 0 exit /b

set "FLAGS=-shared -fPIC -l:kernel32.lib %FLAGS%"
call "%root_dir%tools\build\exe.bat" "%~dp0windows\" test dll
