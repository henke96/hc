@echo off
setlocal disabledelayedexpansion

for /r "%~dp0" %%f in (*build.bat) do (
    echo "%%f"
    call "%%f"
    if not errorlevel 0 exit /b & if errorlevel 1 exit /b
)
