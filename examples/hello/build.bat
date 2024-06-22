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
call "%root_dir%\tools\builder.bat" "%script_dir%\linux\hello.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_hello"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

set "ABI=linux-gnu"
set "FLAGS=-L ^"%OUT%^" -l:libc.so.6"
call "%root_dir%\cc.bat" -fPIC -shared -o "%OUT%\libc.so.6" "%root_dir%\src\hc\linux\gnu\libc.so.6.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\tools\builder.bat" "%script_dir%\gnulinux\hello.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_hello"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

set "ABI=linux-android26"
rem Android's dynamic linker demands PIE.
set "FLAGS=-fPIE -pie -Wl,-dynamic-linker=/system/bin/linker64 -L ^"%OUT%^" -l:libc.so -l:liblog.so"
call "%root_dir%\cc.bat" -fPIC -shared -o "%OUT%\libc.so" "%root_dir%\src\hc\linux\android\libc.so.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\cc.bat" -fPIC -shared -o "%OUT%\liblog.so" "%root_dir%\src\hc\linux\android\liblog.so.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\tools\builder.bat" "%script_dir%\android\hello.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_hello"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

set "ABI=freebsd14"
set "FLAGS=-Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L ^"%OUT%^" -l:libc.so.7"
call "%root_dir%\cc.bat" -fPIC -shared -Wl,--version-script="%root_dir%\src\hc\freebsd\libc.so.7.map" -o "%OUT%\libc.so.7" "%root_dir%\src\hc\freebsd\libc.so.7.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\tools\builder.bat" "%script_dir%\freebsd\hello.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_hello"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if not "%ARCH%" == "riscv64" (
    set "ABI=windows-gnu"
    set "FLAGS=-Wl,-subsystem,console -L ^"%OUT%^" -l:kernel32.lib"
    set "FLAGS_RELEASE=-Os -s"
    set "FLAGS_DEBUG=-g -gcodeview -Wl,--pdb="
    call "%root_dir%\genlib.bat" "%OUT%\kernel32.lib" "%root_dir%\src\hc\windows\dll\kernel32.def"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    call "%root_dir%\tools\builder.bat" "%script_dir%\windows\hello.exe.c"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

    set "ABI=windows-gnu"
    set "FLAGS=-Wl,-subsystem,efi_application -Os -s"
    set "FLAGS_RELEASE="
    set "FLAGS_DEBUG="
    call "%root_dir%\tools\builder.bat" "%script_dir%\efi\hello.efi.c"
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
    set "ABI=wasi"
    set "FLAGS="
    set "FLAGS_RELEASE=-Os -s"
    set "FLAGS_DEBUG=-g"
    call "%root_dir%\tools\builder.bat" "%script_dir%\wasi\hello.c"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
