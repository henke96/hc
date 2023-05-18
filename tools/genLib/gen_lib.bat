@echo off
setlocal disabledelayedexpansion

if defined LLVM set "llvm_prefix=%LLVM%\bin\"

set "input=%~1"
set "output=%~2"

if "%ARCH%" == "x86_64" (
    set "machine=x64"
) else (
    if "%ARCH%" == "aarch64" (
        set "machine=arm64"
    ) else (
        echo "Invalid architecture"
        exit /b 1
    )
)

"%llvm_prefix%lld-link" -machine:"%machine%" -def:"%input%" -out:"%output%"
