@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "NO_DEBUG=1"
set "NO_ANALYSIS=1"
call "%root_dir%\tools\hostbuild.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

call "%script_dir%\build.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

"%OUT%\%HOST_ARCH%-%HOST_ABI%_tar.exe" %*
