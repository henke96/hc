@echo off
setlocal disabledelayedexpansion

if defined LLVM set "llvm_prefix=%LLVM%\bin\"

if "%ARCH%" == "x86_64" (
    set "machine=x64"
) else (
    if "%ARCH%" == "aarch64" (
        set "machine=arm64"
    ) else (
        exit /b 1
    )
)

"%llvm_prefix%lld-link" -machine:"%machine%" -out:"%~1" -def:"%~2"
