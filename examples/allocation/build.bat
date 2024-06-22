@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."
goto start

:build
setlocal
set "FLAGS_RELEASE=-Os"
set "FLAGS_DEBUG=-g"

set "ABI=linux"
set "FLAGS="
call "%root_dir%\tools\builder.bat" "%script_dir%\linux\allocation.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_allocation"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

set "ABI=freebsd14"
set "FLAGS=-Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L ^"%OUT%^" -l:libc.so.7"
call "%root_dir%\cc.bat" -fPIC -shared -Wl,--version-script="%root_dir%\src\hc\freebsd\libc.so.7.map" -o "%OUT%\libc.so.7" "%root_dir%\src\hc\freebsd\libc.so.7.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\tools\builder.bat" "%script_dir%\freebsd\allocation.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_allocation"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if not "%ARCH%" == "riscv64" (
    set "ABI=windows-gnu"
    set "FLAGS=-Wl,-subsystem,console -L ^"%OUT%^" -l:kernel32.lib"
    set "FLAGS_RELEASE=-Os -s"
    set "FLAGS_DEBUG=-g -gcodeview -Wl,--pdb="
    call "%root_dir%\genlib.bat" "%OUT%\kernel32.lib" "%root_dir%\src\hc\windows\dll\kernel32.def"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    call "%root_dir%\tools\builder.bat" "%script_dir%\windows\allocation.exe.c"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
exit /b

:start
if not defined NO_X86_64 (
    set "ARCH=x86_64" & call :build
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
if not defined NO_AARCH64 (
    set "ARCH=aarch64" & call :build
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
if not defined NO_RISCV64 (
    set "ARCH=riscv64" & call :build
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
if not defined NO_WASM32 (
    set "ARCH=wasm32"
    set "ABI=unknown"
    set "FLAGS=-Wl,--no-entry"
    set "FLAGS_RELEASE=-Os -s"
    set "FLAGS_DEBUG=-g"
    call "%root_dir%\tools\builder.bat" "%script_dir%\web\allocation.wasm.c"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

    call "%root_dir%\tools\htmlPacker\build.bat"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    "%OUT%\htmlPacker.exe" "%OUT%\allocation.html" _start.html "%script_dir%\web" "%OUT%"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
