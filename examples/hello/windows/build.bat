@echo off
setlocal
set "root_dir=%~dp0..\..\.."

set "FLAGS=-l:kernel32.lib -l:user32.lib %FLAGS%"
call "%root_dir%\tools\build\exe.bat" "%~dp0" hello
