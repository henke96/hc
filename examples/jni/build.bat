@echo off
setlocal
set "root_dir=%~dp0..\..\"

set "FLAGS=-shared -fPIC"
call "%root_dir%tools\build\elf.bat" "%~dp0linux\" libtest so

set "FLAGS=-shared -fPIC -l:kernel32.lib"
call "%root_dir%tools\build\exe.bat" "%~dp0windows\" test dll
