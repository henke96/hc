#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../../.."

prepare_apk() {
    android_arch="$ARCH"
    if test "$ARCH" = "aarch64"; then android_arch="arm64-v8a"; fi

    out_dir="$out_base_dir/$1dist/lib/$android_arch"
    mkdir -p "$out_dir"
    cp "$out_base_dir/$ARCH/$1libopengl.so" "$out_dir/libopengl.so"
}

build_apk() {
    "$ANDROID_SDK/build-tools/26.0.3/aapt" package $2 -f -F "$out_base_dir/$1openGl.apk" -M "$script_dir/AndroidManifest.xml" -I "$ANDROID_SDK/platforms/android-26/android.jar" "$out_base_dir/$1dist"
}

sign_apk() {
    "${java_prefix}java" -jar "$ANDROID_SDK/build-tools/26.0.3/lib/apksigner.jar" sign --ks "$KEYSTORE" --ks-pass "$KEYSTORE_PASS" "$out_base_dir/$1openGl.apk"
}

if test -n "$JAVA_HOME"; then java_prefix="$JAVA_HOME/bin/"; fi

export LINK_LIBDL=1 LINK_LIBLOG=1 LINK_LIBANDROID=1 LINK_LIBC=1
export FLAGS="-shared $FLAGS"
"$root_dir/tools/build/androidelf.sh" "$script_dir/libopengl.c" openGl/android .so

out_base_dir="$root_dir/../hc-out/openGl/android"

if test -z "$NO_AARCH64"; then
    export ARCH=aarch64
    prepare_apk "debug-"
    prepare_apk ""
fi
if test -z "$NO_X86_64"; then
    export ARCH=x86_64
    prepare_apk "debug-"
    prepare_apk ""
fi

if test -z "$ANDROID_SDK"; then
    # Download android command line tools: https://developer.android.com/studio (scroll down)
    # bin/sdkmanager --sdk_root=. --install "build-tools;26.0.3" "platforms;android-26"
    echo "Set ANDROID_SDK to build apks"
else
    build_apk "debug-" "--debug-mode"
    build_apk "" ""

    if test -z "$KEYSTORE"; then
        # keytool -genkeypair -keyalg RSA -validity 100000 -keystore ./my.keystore
        echo "Set KEYSTORE (and optionally KEYSTORE_PASS) to sign apks"
    else
        KEYSTORE_PASS="${KEYSTORE_PASS:-stdin}"
        sign_apk "debug-"
        sign_apk ""
    fi
fi
