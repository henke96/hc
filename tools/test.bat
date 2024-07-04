@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\.."

for /r "%root_dir%" %%f in (*build.bat) do (
    echo %%f
    call "%%f"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)

call "%root_dir%\tools\hostbuild.bat"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if not defined NO_DEBUG (
    "%OUT%\debug_%HOST_ARCH%-%HOST_ABI%_tests.exe" %*
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
"%OUT%\%HOST_ARCH%-%HOST_ABI%_tests.exe" %*
