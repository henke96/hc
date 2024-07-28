@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\.."

set "fail="
for /r "%root_dir%" %%f in (build.bat?) do (
    echo %%f
    call "%%f"
    if not errorlevel 0 ( set "fail=1" ) else if errorlevel 1 set "fail=1"
)
if "%fail%" == "1" exit /b 1

call "%root_dir%\src\shell\hostarch.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

set "hostabi=windows-gnu"

if not defined NO_DEBUG (
    "%OUT%\debug\%hostarch%-%hostabi%_tests.exe" %*
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
"%OUT%\%hostarch%-%hostabi%_tests.exe" %*
