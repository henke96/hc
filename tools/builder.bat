@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\.."

if not defined OUT (
    echo Please set OUT
    exit /b 1
)
if not defined ARCH (
    echo Please set ARCH
    exit /b 1
)
if not defined ABI (
    echo Please set ABI
    exit /b 1
)

set "source=%~1"
set "source_name=%~n1"
set "full_name=%ARCH%-%ABI%_%source_name%"

set "FLAGS_RELEASE=%FLAGS% %FLAGS_RELEASE%"
set "FLAGS_DEBUG=-fsanitize-undefined-trap-on-error -fsanitize=undefined -Dhc_DEBUG=1 %FLAGS% %FLAGS_DEBUG%"

if not defined NO_DEBUG (
    if defined ASSEMBLY (
        call "%root_dir%\cc.bat" -S -o "%OUT%\debug_%full_name%.s" %FLAGS_DEBUG% "%source%"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    )
    call "%root_dir%\cc.bat" -o "%OUT%\debug_%full_name%" %FLAGS_DEBUG% "%source%"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)

if defined ASSEMBLY (
    call "%root_dir%\cc.bat" -S -o "%OUT%\%full_name%.s" %FLAGS_RELEASE% "%source%"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
call "%root_dir%\cc.bat" -o "%OUT%\%full_name%" %FLAGS_RELEASE% "%source%"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

set "analysis_flags=--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
if not defined NO_ANALYSIS (
    call "%root_dir%\cc.bat" %analysis_flags% %FLAGS_RELEASE% "%source%"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    call "%root_dir%\cc.bat" %analysis_flags% %FLAGS_DEBUG% "%source%"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
