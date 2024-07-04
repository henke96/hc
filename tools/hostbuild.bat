@echo off
if "%processor_architecture%" == "AMD64" (
    set "HOST_ARCH=x86_64"
) else if "%processor_architecture%" == "AARCH64" (
    set "HOST_ARCH=aarch64"
) else exit /b 1

set "NO_WASM32=1"
set "NO_RISCV64=1"
if not "%HOST_ARCH%" == "x86_64" set "NO_X86_64=1"
if not "%HOST_ARCH%" == "aarch64" set "NO_AARCH64=1"

set "NO_EFI=1"
set "NO_LINUX=1"
set "NO_GNULINUX=1"
set "NO_ANDROID=1"
set "NO_FREEBSD=1"
set "HOST_ABI=windows-gnu"
