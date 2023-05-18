@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"

for /r "%script_dir%" %%f in (*build.bat) do (
    echo "%%f"
    call "%%f"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
