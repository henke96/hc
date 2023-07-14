@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."
call "%root_dir%\tools\shellUtil\setnativearch.bat"
if not errorlevel 0 exit /b
if errorlevel 1 exit /b

if defined JAVA_HOME set "java_prefix=%JAVA_HOME%\bin\"

"%java_prefix%javac" "%script_dir%\jni\Test.java"
"%java_prefix%java" -cp "%script_dir%" -Djava.library.path="%script_dir%\windows\%ARCH%" jni/Test
