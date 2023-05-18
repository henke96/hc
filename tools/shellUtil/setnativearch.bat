@echo off
if "%processor_architecture%" == "AMD64" (
    set "ARCH=x86_64"
    exit /b 0
)
if "%processor_architecture%" == "ARM64" (
    set "ARCH=aarch64"
    exit /b 0
)

echo "Invalid architecture"
exit /b 1
