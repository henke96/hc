@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

call "%root_dir%\tools\shellUtil\setnativearch.bat"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%\src\hc\windows\dll\kernel32.def" "%script_dir%\windows\kernel32.lib"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
call "%root_dir%\cc_pe.bat" -L"%script_dir%\windows" "%script_dir%\windows\htmlPacker.c" -o "%script_dir%\windows\htmlPacker.exe" -l:kernel32.lib
if not errorlevel 0 exit /b & if errorlevel 1 exit /b

"%script_dir%\windows\htmlPacker.exe" %*
