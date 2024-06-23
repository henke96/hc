@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."
goto start

:build
setlocal
set "ABI=linux"
set "FLAGS="
set "FLAGS_RELEASE=%opt%"
set "FLAGS_DEBUG=-g"
call "%root_dir%\tools\builder.bat" "%script_dir%\linux\hash.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_hash"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

set "ABI=freebsd14"
set "FLAGS=-Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L ^"%OUT%^" -l:libc.so.7"
call "%root_dir%\cc.bat" -fPIC -shared -Wl,--version-script="%root_dir%\src\hc\freebsd\libc.so.7.map" -o "%OUT%\libc.so.7" "%root_dir%\src\hc\freebsd\libc.so.7.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\tools\builder.bat" "%script_dir%\freebsd\hash.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_hash"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if not "%ARCH%" == "riscv64" (
    set "ABI=windows-gnu"
    set "FLAGS=-Wl,-subsystem,console -L ^"%OUT%^" -l:kernel32.lib"
    set "FLAGS_RELEASE=%opt% -s"
    set "FLAGS_DEBUG=-g -gcodeview -Wl,--pdb="
    call "%root_dir%\genlib.bat" "%OUT%\kernel32.lib" "%root_dir%\src\hc\windows\dll\kernel32.def"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    call "%root_dir%\tools\builder.bat" "%script_dir%\windows\hash.exe.c"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
exit /b

:start
set "opt=-Os"

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
