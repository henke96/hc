@echo off
setlocal
set "root_dir=%~dp0..\..\..\"

set "FLAGS=-shared -l:libdl.so -l:libandroid.so -l:liblog.so -l:libc.so %FLAGS%"
set "ARCH=aarch64"
call "%root_dir%tools\build\androidelf.bat" "%~dp0" libopengl aarch64.so
if %errorlevel% neq 0 exit /b
set "ARCH=x86_64"
call "%root_dir%tools\build\androidelf.bat" "%~dp0" libopengl x86_64.so
if %errorlevel% neq 0 exit /b

call :prepare_apk "debug."
if %errorlevel% neq 0 exit /b
call :prepare_apk ""
if %errorlevel% neq 0 exit /b

if defined JAVA_HOME (
    set "JAVA=%JAVA_HOME%\bin\java.exe"
) else (
    set "JAVA=java.exe"
)

:: Download android command line tools: https://developer.android.com/studio (scroll down)
:: bin\sdkmanager.bat --sdk_root=. --install "build-tools;26.0.3" "platforms;android-26"
if not defined ANDROID_SDK (
    echo Set ANDROID_SDK to build apks
) else (
    call :build_apk "debug." "--debug-mode"
    if %errorlevel% neq 0 exit /b
    call :build_apk "" ""
    if %errorlevel% neq 0 exit /b

    if not defined KEYSTORE (
        :: keytool -genkeypair -keyalg RSA -validity 100000 -keystore my.keystore
        echo Set KEYSTORE (and optionally KEYSTORE_PASS^) to sign apks
    ) else (
        if not defined KEYSTORE_PASS set "KEYSTORE_PASS=stdin"
        call :sign_apk "debug."
        if %errorlevel% neq 0 exit /b
        call :sign_apk ""
        if %errorlevel% neq 0 exit /b
    )
)
exit /b

:prepare_apk
if not exist "%~dp0%~1dist\lib\arm64-v8a\" (
    mkdir "%~dp0%~1dist\lib\arm64-v8a\"
    if %errorlevel% neq 0 exit /b
)
if not exist "%~dp0%~1dist\lib\x86_64\" (
    mkdir "%~dp0%~1dist\lib\x86_64\"
    if %errorlevel% neq 0 exit /b
)
move "%~dp0%~1libopengl.aarch64.so" "%~dp0%~1dist\lib\arm64-v8a\libopengl.so" >nul
if %errorlevel% neq 0 exit /b
move "%~dp0%~1libopengl.x86_64.so" "%~dp0%~1dist\lib\x86_64\libopengl.so" >nul
exit /b

:build_apk
call "%ANDROID_SDK%\build-tools\26.0.3\aapt.exe" package %~2 -f -F "%~dp0%~1openGl.apk" -M "%~dp0\AndroidManifest.xml" -I "%ANDROID_SDK%\platforms\android-26\android.jar" "%~dp0%~1dist"
exit /b

:sign_apk
call "%JAVA%" -jar "%ANDROID_SDK%\build-tools\26.0.3\lib\apksigner.jar" sign --ks "%KEYSTORE%" --ks-pass "%KEYSTORE_PASS%" "%~dp0%~1openGl.apk"
exit /b
