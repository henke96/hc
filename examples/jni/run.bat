@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

set "NO_ANALYSIS=1"
call "%root_dir%\tools\hostbuild.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

call "%script_dir%\build.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if defined JAVA_HOME set "java_prefix=%JAVA_HOME%\bin\"

"%java_prefix%javac" -d "%OUT%" "%script_dir%\jni\Test.java"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if not defined NO_DEBUG (
    copy /b /y "%OUT%\debug_%HOST_ARCH%-%HOST_ABI%_test.dll" "%OUT%\test.dll" >nul
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    "%java_prefix%java" -cp "%OUT%" -Djava.library.path="%OUT%" jni/Test
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
copy /b /y "%OUT%\%HOST_ARCH%-%HOST_ABI%_test.dll" "%OUT%\test.dll" >nul
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
"%java_prefix%java" -cp "%OUT%" -Djava.library.path="%OUT%" jni/Test
