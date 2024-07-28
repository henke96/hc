@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

call "%root_dir%\src\shell\hostarch.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
if not "%hostarch%" == "x86_64" set "NO_X86_64=1"
if not "%hostarch%" == "aarch64" set "NO_AARCH64=1"
if not "%hostarch%" == "riscv64" set "NO_RISCV64=1"

set "NO_LINUX=1"
set "NO_FREEBSD=1"
set "NO_WINDOWS="
set "hostabi=windows-gnu"

set "NO_ANALYSIS=1"
call "%script_dir%\build.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if defined JAVA_HOME set "java_prefix=%JAVA_HOME%\bin\"

"%java_prefix%javac" -d "%OUT%" "%script_dir%\jni\Test.java"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if not defined NO_DEBUG (
    copy /b /y "%OUT%\debug\%hostarch%-%hostabi%_test.dll" "%OUT%\test.dll" >nul
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    "%java_prefix%java" -cp "%OUT%" -Djava.library.path="%OUT%" jni/Test
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
copy /b /y "%OUT%\%hostarch%-%hostabi%_test.dll" "%OUT%\test.dll" >nul
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
"%java_prefix%java" -cp "%OUT%" -Djava.library.path="%OUT%" jni/Test
