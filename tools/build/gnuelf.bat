@echo off
setlocal
set "root_dir=%~dp0..\..\"

call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\gnulinux\dynamic\libc.so.6.c" "%~1libc.so.6"
if %errorlevel% neq 0 exit /b
call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\gnulinux\dynamic\libdl.so.2.c" "%~1libdl.so.2"
if %errorlevel% neq 0 exit /b

set "FLAGS=-L^"%~1\^" %FLAGS%"
call "%~dp0elf.bat" %*

endlocal
