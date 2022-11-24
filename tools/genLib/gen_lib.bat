@echo off
setlocal

if not defined LLD_LINK set LLD_LINK=lld-link
if not defined ARCH set ARCH=x86_64

if "%ARCH%" == "x86_64" (
    set machine=x64
) else (
    if "%ARCH%" == "aarch64" (
        set machine=arm64
    ) else (
        echo "Invalid architecture"
        exit 1
    )
)
"%LLD_LINK%" -machine:"%machine%" -def:"%1" -out:"%2"
endlocal