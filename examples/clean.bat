@echo off
setlocal
cd "%~dp0"
if %errorlevel% neq 0 exit /b
git clean -fdqX --exclude="!*.keystore"
endlocal

