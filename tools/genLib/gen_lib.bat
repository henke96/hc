@echo off
setlocal

if not defined ARCH set ARCH=x86_64
if "%ARCH%" == "x86_64" (
    set machine=x64
) else (
    if "%ARCH%" == "aarch64" (
        set machine=arm64
    ) else (
        echo "Invalid architecture"
        exit /b 1
    )
)
"%LLVM%lld-link" -machine:"%machine%" -def:"%~1" -out:"%~2"
endlocal
