@echo off
if "%processor_architecture%" == "AMD64" (
    set "hostarch=x86_64"
) else if "%processor_architecture%" == "AARCH64" (
    set "hostarch=aarch64"
) else exit /b 1
