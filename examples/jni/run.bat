@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

if not defined OUT (
    echo Please set OUT
    exit /b 1
)

call "%root_dir%\tools\shell\setnativearch.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if defined JAVA_HOME (
    set "java_prefix=%JAVA_HOME%\bin\"
)

copy /b /y "%OUT%\debug_%ARCH%-%ABI%_test.dll" "%OUT%\test.dll" >nul
"%java_prefix%javac" -d "%OUT%" "%script_dir%\jni\Test.java"
"%java_prefix%java" -cp "%OUT%" -Djava.library.path="%OUT%" jni/Test
