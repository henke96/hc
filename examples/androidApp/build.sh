#!/bin/sh
set -e
script_dir="$(dirname "$0")"
root_dir="$script_dir/../.."

flags="-shared -l:libdl.so -l:libandroid.so -l:liblog.so -l:libc.so $FLAGS"
ARCH="aarch64" FLAGS="$flags" "$root_dir/tools/build/androidelf.sh" "$script_dir" libandroidapp aarch64.so
ARCH="x86_64" FLAGS="$flags" "$root_dir/tools/build/androidelf.sh" "$script_dir" libandroidapp x86_64.so

prepare_apk() {
    mkdir -p "$script_dir/$1dist/lib/arm64-v8a/"
    mkdir -p "$script_dir/$1dist/lib/x86_64/"
    mv "$script_dir/$1libandroidapp.aarch64.so" "$script_dir/$1dist/lib/arm64-v8a/libandroidapp.so"
    mv "$script_dir/$1libandroidapp.x86_64.so" "$script_dir/$1dist/lib/x86_64/libandroidapp.so"
}

build_apk() {
    "$ANDROID_SDK/build-tools/26.0.3/aapt" package $2 -f -F "$script_dir/$1androidApp.apk" -M "$script_dir/AndroidManifest.xml" -I "$ANDROID_SDK/platforms/android-26/android.jar" "$script_dir/$1dist"
}
sign_apk() {
    "$ANDROID_SDK/build-tools/26.0.3/apksigner" sign --ks "$KEYSTORE" --ks-pass "$KEYSTORE_PASS" "$script_dir/$1androidApp.apk"
}

prepare_apk "debug."
prepare_apk ""

# Download android command line tools: https://developer.android.com/studio (scroll down)
# bin/sdkmanager --sdk_root=. --install "build-tools;26.0.3" "platforms;android-26"
if test -z "$ANDROID_SDK"; then
    echo "Set ANDROID_SDK to build apks"
else
    build_apk "debug." "--debug-mode"
    build_apk "" ""

    if test -z "$KEYSTORE"; then
        # keytool -genkeypair -keyalg RSA -validity 100000 -keystore ./my.keystore
        echo "Set KEYSTORE (and optionally KEYSTORE_PASS) to sign apks"
    else
        KEYSTORE_PASS="${KEYSTORE_PASS:-stdin}"
        sign_apk "debug."
        sign_apk ""
    fi
fi
