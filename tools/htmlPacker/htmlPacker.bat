@echo off
setlocal
set "root_dir=%~dp0..\..\"

if "%processor_architecture%" == "AMD64" (
    set "ARCH=x86_64"
) else (
    if "%processor_architecture%" == "ARM64" (
        set "ARCH=aarch64"
    ) else (
        echo "Invalid architecture"
        exit /b
    )
)

call "%root_dir%tools\genLib\gen_lib.bat" "%root_dir%src\hc\windows\dll\kernel32.def" "%~dp0windows\kernel32.lib"
if %errorlevel% neq 0 exit /b
call "%root_dir%cc_pe.bat" -L"%~dp0windows\\" "%~dp0windows\htmlPacker.c" -o "%~dp0windows\htmlPacker.exe" -l:kernel32.lib
if %errorlevel% neq 0 exit /b

"%~dp0windows\htmlPacker.exe" %*
