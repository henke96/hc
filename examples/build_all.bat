@echo off
setlocal
cd "%~dp0"
if %errorlevel% neq 0 exit /b

for /R %%f in (*build.bat) do (
    endlocal

    echo "%%f"
    call "%%f"

    setlocal
    cd "%~dp0"
    if %errorlevel% neq 0 exit /b
)
