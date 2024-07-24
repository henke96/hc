@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

if not defined OUT (
    echo Please set OUT
    exit /b 1
)
set "name=test"
set "opt=-Os"
goto start

:build
setlocal
    set "FLAGS_RELEASE=%opt%"
    set "FLAGS_DEBUG=-g"

    set "ABI=linux"
    if not defined NO_LINUX (
        rem OpenJDK complains if stack is executable.
        set "FLAGS=-fPIC -shared -Wl,-znoexecstack"
        call "%root_dir%\tools\builder.bat" "%script_dir%\linux\lib%name%.so.c"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
        call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_lib%name%.so"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    )
    rem TODO freebsd
    set "ABI=windows-gnu"
    if not defined NO_WINDOWS (
        set "FLAGS=-fPIC -shared -L "%OUT%" -l:kernel32.lib"
        set "FLAGS_RELEASE=%opt% -s"
        set "FLAGS_DEBUG=-g -gcodeview -Wl,--pdb="
        call "%root_dir%\genlib.bat" "%OUT%\kernel32.lib" "%root_dir%\src\hc\windows\dll\kernel32.def"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
        call "%root_dir%\tools\builder.bat" "%script_dir%\windows\%name%.dll.c"
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
