@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"
set "root_dir=%script_dir%\..\..\.."

if defined JAVA_HOME set "java_prefix=%JAVA_HOME%\bin\"

set LINK_LIBDL=1 & set LINK_LIBLOG=1 & set LINK_LIBANDROID=1 & set LINK_LIBC=1
set "FLAGS=-shared %FLAGS%"
call "%root_dir%\tools\build\androidelf.bat" "%script_dir%" libopengl so
if not errorlevel 0 exit /b
if errorlevel 1 exit /b

if not defined NO_AARCH64 (
    set "ARCH=aarch64"
    call :prepare_apk "debug."
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
    call :prepare_apk ""
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
if not defined NO_X86_64 (
    set "ARCH=x86_64"
    call :prepare_apk "debug."
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
    call :prepare_apk ""
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)

if not defined ANDROID_SDK (
    rem Download android command line tools: https://developer.android.com/studio (scroll down)
    rem bin\sdkmanager.bat --sdk_root=. --install "build-tools;26.0.3" "platforms;android-26"
    echo.Set ANDROID_SDK to build apks
) else (
    call :build_apk "debug." "--debug-mode"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
    call :build_apk "" ""
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b

    if not defined KEYSTORE (
        rem keytool -genkeypair -keyalg RSA -validity 100000 -keystore my.keystore
        echo.Set KEYSTORE ^(and optionally KEYSTORE_PASS^) to sign apks
    ) else (
        if not defined KEYSTORE_PASS set "KEYSTORE_PASS=stdin"
        call :sign_apk "debug."
        if not errorlevel 0 exit /b
        if errorlevel 1 exit /b
        call :sign_apk ""
        if not errorlevel 0 exit /b
        if errorlevel 1 exit /b
    )
)
exit /b

:prepare_apk
setlocal
set "android_arch=%ARCH%"
if "%ARCH%" == "aarch64" set "android_arch=arm64-v8a"

if not exist "%script_dir%\%~1dist\lib\%android_arch%" (
    mkdir "%script_dir%\%~1dist\lib\%android_arch%"
    if not errorlevel 0 exit /b
    if errorlevel 1 exit /b
)
copy /b /v /y "%script_dir%\%ARCH%\%~1libopengl.so" "%script_dir%\%~1dist\lib\%android_arch%\libopengl.so" >nul
exit /b

:build_apk
setlocal
"%ANDROID_SDK%\build-tools\26.0.3\aapt.exe" package %~2 -f -F "%script_dir%\%~1openGl.apk" -M "%script_dir%\AndroidManifest.xml" -I "%ANDROID_SDK%\platforms\android-26\android.jar" "%script_dir%\%~1dist"
exit /b

:sign_apk
setlocal
"%java_prefix%java" -jar "%ANDROID_SDK%\build-tools\26.0.3\lib\apksigner.jar" sign --ks "%KEYSTORE%" --ks-pass "%KEYSTORE_PASS%" "%script_dir%\%~1openGl.apk"
exit /b
