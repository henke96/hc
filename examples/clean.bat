@echo off
setlocal disabledelayedexpansion
cd "%~dp0"
if not errorlevel 0 exit /b & if errorlevel 1 exit /b
git clean -fdqX --exclude="!*.keystore"
