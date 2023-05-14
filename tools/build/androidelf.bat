@echo off
setlocal disabledelayedexpansion
set "root_dir=%~dp0..\..\"

call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\android\liblog.so.c" "%~1liblog.so"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\android\libdl.so.c" "%~1libdl.so"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\android\libandroid.so.c" "%~1libandroid.so"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%tools\genLib\gen_so.bat" "%root_dir%src\hc\linux\android\libc.so.c" "%~1libc.so"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

set "ABI=linux-android26"
set "STRIP_OPT=--strip-all"
set "FLAGS=-fPIC -fpie -pie -Wl,-dynamic-linker=/system/bin/linker64 -L^"%~1\^" %FLAGS%"
call "%~dp0elf.bat" %*
