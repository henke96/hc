@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

if not defined OUT (
    echo Please set OUT
    exit /b 1
)
set "name=tests"
set "opt=-Os"
set "flags_common=-I "%script_dir%\common\files""
goto start

:build
setlocal
    set "FLAGS_RELEASE=%opt%"
    set "FLAGS_DEBUG=-g"

    set "ABI=linux"
    if not defined NO_LINUX (
        set "FLAGS=%flags_common%"
        call "%root_dir%\core\builder.bat" "%script_dir%\linux\%name%.c"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
        call "%root_dir%\core\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_%name%"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    )
    set "ABI=freebsd14"
    if not defined NO_FREEBSD (
        set "FLAGS=%flags_common% -Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L "%OUT%" -l:libc.so.7"
        call "%root_dir%\core\cc.bat" -fPIC -shared -Wl,--version-script="%root_dir%\core\hc\freebsd\libc.so.7.map" -o "%OUT%\libc.so.7" "%root_dir%\core\hc\freebsd\libc.so.7.c"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
        call "%root_dir%\core\builder.bat" "%script_dir%\freebsd\%name%.c"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
        call "%root_dir%\core\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_%name%"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    )
    set "ABI=windows-gnu"
    if not defined NO_WINDOWS (
        set "FLAGS=%flags_common% -Wl,-subsystem,console -L "%OUT%" -l:kernel32.lib"
        set "FLAGS_RELEASE=%opt% -s"
        set "FLAGS_DEBUG=-g -gcodeview -Wl,--pdb="
        call "%root_dir%\core\genlib.bat" "%OUT%\kernel32.lib" "%root_dir%\core\hc\windows\dll\kernel32.def"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
        call "%root_dir%\core\builder.bat" "%script_dir%\windows\%name%.exe.c"
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
    setlocal
        set "ARCH=riscv64" & set "NO_WINDOWS=1" & call :build
    endlocal
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
if not defined NO_WASM32 (
    set "ARCH=wasm32"
    set "ABI=wasi"
    set "FLAGS=%flags_common%"
    set "FLAGS_RELEASE=%opt% -s"
    set "FLAGS_DEBUG=-g"
    call "%root_dir%\core\builder.bat" "%script_dir%\wasi\%name%.c"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
