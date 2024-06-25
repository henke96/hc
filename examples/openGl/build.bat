@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\.."

if not defined OUT (
    echo Please set OUT
    exit /b 1
)
goto start

:build
setlocal
set "FLAGS_RELEASE=-Os"
set "FLAGS_DEBUG=-g"

set "ABI=linux-gnu"
set "FLAGS=-L ^"%OUT%^" -l:libc.so.6 -l:libdl.so.2"
call "%root_dir%\cc.bat" -fPIC -shared -o "%OUT%\libc.so.6" "%root_dir%\src\hc\linux\gnu\libc.so.6.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\cc.bat" -fPIC -shared -o "%OUT%\libdl.so.2" "%root_dir%\src\hc\linux\gnu\libdl.so.2.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\tools\builder.bat" "%script_dir%\gnulinux\openGl.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_openGl"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

set "ABI=freebsd14"
set "FLAGS=-Wl,-dynamic-linker=/libexec/ld-elf.so.1 -L ^"%OUT%^" -l:libc.so.7"
call "%root_dir%\cc.bat" -fPIC -shared -Wl,--version-script="%root_dir%\src\hc\freebsd\libc.so.7.map" -o "%OUT%\libc.so.7" "%root_dir%\src\hc\freebsd\libc.so.7.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\tools\builder.bat" "%script_dir%\freebsd\openGl.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-sections "%OUT%\%ARCH%-%ABI%_openGl"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

if not "%ARCH%" == "riscv64" (
    set "ABI=windows-gnu"
    set "FLAGS=-Wl,-subsystem,console -L ^"%OUT%^" -l:kernel32.lib -l:user32.lib -l:gdi32.lib"
    set "FLAGS_RELEASE=-Os -s"
    set "FLAGS_DEBUG=-g -gcodeview -Wl,--pdb="
    call "%root_dir%\genlib.bat" "%OUT%\kernel32.lib" "%root_dir%\src\hc\windows\dll\kernel32.def"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    call "%root_dir%\genlib.bat" "%OUT%\user32.lib" "%root_dir%\src\hc\windows\dll\user32.def"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    call "%root_dir%\genlib.bat" "%OUT%\gdi32.lib" "%root_dir%\src\hc\windows\dll\gdi32.def"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    call "%root_dir%\tools\builder.bat" "%script_dir%\windows\openGl.exe.c"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)
exit /b

:build_android
setlocal
set "ABI=linux-android26"
set "FLAGS=-fPIC -shared  -L ^"%OUT%^" -l:libc.so -l:liblog.so -l:libdl.so -l:libandroid.so"
call "%root_dir%\cc.bat" -fPIC -shared -o "%OUT%\libc.so" "%root_dir%\src\hc\linux\android\libc.so.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\cc.bat" -fPIC -shared -o "%OUT%\liblog.so" "%root_dir%\src\hc\linux\android\liblog.so.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\cc.bat" -fPIC -shared -o "%OUT%\libdl.so" "%root_dir%\src\hc\linux\android\libdl.so.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\cc.bat" -fPIC -shared -o "%OUT%\libandroid.so" "%root_dir%\src\hc\linux\android\libandroid.so.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\tools\builder.bat" "%script_dir%\android\libopenGl.so.c"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
call "%root_dir%\objcopy.bat" --strip-all "%OUT%\%ARCH%-%ABI%_libopenGl.so"
exit /b

:build_apk
setlocal
rmdir /s /q "%OUT%\apk"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
mkdir "%OUT%\apk\lib\x86_64"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
copy /b "%OUT%\%~1x86_64-linux-android26_libopenGl.so" "%OUT%\apk\lib\x86_64\libopenGl.so" >nul
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
mkdir "%OUT%\apk\lib\arm64-v8a"
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
copy /b "%OUT%\%~1aarch64-linux-android26_libopenGl.so" "%OUT%\apk\lib\arm64-v8a\libopenGl.so" >nul
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

"%ANDROID_SDK%\build-tools\26.0.3\aapt" package %~2 -f -F "%OUT%\%~1openGl.apk" -M "%script_dir%\android\AndroidManifest.xml" -I "%ANDROID_SDK%\platforms\android-26\android.jar" "%OUT%\apk"
exit /b

:sign_apk
setlocal
"%java_prefix%java" -jar "%ANDROID_SDK%\build-tools\26.0.3\lib\apksigner.jar" sign --ks "%KEYSTORE%" --ks-pass "%KEYSTORE_PASS%" "%OUT%/%~1openGl.apk"
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
    call "%root_dir%\tools\builder.bat" "%script_dir%\web\openGl.wasm.c"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

    call "%root_dir%\tools\htmlPacker\build.bat"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    "%OUT%\htmlPacker.exe" "%OUT%\openGl.html" _start.html "%script_dir%\web" "%OUT%"
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
)

set "ARCH=x86_64" & call :build_android
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
set "ARCH=aarch64" & call :build_android
if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
rem TODO riscv64?

if not defined ANDROID_SDK (
    rem Download android command line tools: https://developer.android.com/studio (scroll down)
    rem bin\sdkmanager.bat --sdk_root=. --install "build-tools;26.0.3" "platforms;android-26"
    echo Set ANDROID_SDK to build apks
) else (
    if defined JAVA_HOME set "java_prefix=%JAVA_HOME%\bin\"

    if not defined NO_DEBUG (
        call :build_apk "debug_" "--debug-mode"
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    )
    call :build_apk "" ""
    if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b

    if not defined KEYSTORE (
        rem keytool -genkeypair -keyalg RSA -validity 100000 -keystore my.keystore
        echo Set KEYSTORE ^(and optionally KEYSTORE_PASS^) to sign apks
    ) else (
        if not defined KEYSTORE_PASS set "KEYSTORE_PASS=stdin"
        if not defined NO_DEBUG (
            call :sign_apk "debug_"
            if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
        )
        call :sign_apk ""
        if not errorlevel 0 ( exit /b ) else if errorlevel 1 exit /b
    )
)
