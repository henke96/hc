@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."
call "%root_dir%\tools\shell\setnativearch.bat"
if not errorlevel 0 exit /b
if errorlevel 1 exit /b

if defined JAVA_HOME set "java_prefix=%JAVA_HOME%\bin\"

"%java_prefix%javac" -d "%root_dir%\..\hc-out\jni" "%script_dir%\jni\Test.java"
if not errorlevel 0 exit /b
if errorlevel 1 exit /b
"%java_prefix%java" -cp "%root_dir%\..\hc-out\jni" -Djava.library.path="%root_dir%\..\hc-out\jni\windows\%ARCH%" jni/Test
