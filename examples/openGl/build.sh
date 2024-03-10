#!/bin/sh --
set -e
script_dir="$(cd -- "${0%/*}/" && pwd)"
root_dir="$script_dir/../.."
. "$root_dir/tools/shell/escape.sh"

test -n "$OUT" || { echo "Please set OUT"; exit 1; }

build() {
    export ABI=linux-gnu
    export FLAGS="-L $(escape "$OUT") -l:libc.so.6 -l:libdl.so.2"
    export FLAGS_RELEASE="-Os"
    export FLAGS_DEBUG="-g"
    "$root_dir/cc.sh" -fPIC -shared -o "$OUT/libc.so.6" "$root_dir/src/hc/linux/gnu/libc.so.6.c"
    "$root_dir/cc.sh" -fPIC -shared -o "$OUT/libdl.so.2" "$root_dir/src/hc/linux/gnu/libdl.so.2.c"
    "$root_dir/tools/builder.sh" "$script_dir/gnulinux/openGl.c"
    "$root_dir/objcopy.sh" --strip-sections "$OUT/$ARCH-${ABI}_openGl"

    if test "$ARCH" != "riscv64"; then
        export ABI=windows-gnu
        export FLAGS="-Wl,-subsystem,console -L $(escape "$OUT") -l:kernel32.lib -l:user32.lib -l:gdi32.lib"
        export FLAGS_RELEASE="-Os -s"
        export FLAGS_DEBUG="-g -gcodeview -Wl,--pdb="
        "$root_dir/genlib.sh" "$OUT/kernel32.lib" "$root_dir/src/hc/windows/dll/kernel32.def"
        "$root_dir/genlib.sh" "$OUT/user32.lib" "$root_dir/src/hc/windows/dll/user32.def"
        "$root_dir/genlib.sh" "$OUT/gdi32.lib" "$root_dir/src/hc/windows/dll/gdi32.def"
        "$root_dir/tools/builder.sh" "$script_dir/windows/openGl.exe.c"
    fi
}

build_android() {
    export ABI=linux-android26
    export FLAGS="-fPIC -shared -L $(escape "$OUT") -l:libc.so -l:liblog.so -l:libdl.so -l:libandroid.so"
    export FLAGS_RELEASE="-Os"
    export FLAGS_DEBUG="-g"
    "$root_dir/cc.sh" -fPIC -shared -o "$OUT/libc.so" "$root_dir/src/hc/linux/android/libc.so.c"
    "$root_dir/cc.sh" -fPIC -shared -o "$OUT/liblog.so" "$root_dir/src/hc/linux/android/liblog.so.c"
    "$root_dir/cc.sh" -fPIC -shared -o "$OUT/libdl.so" "$root_dir/src/hc/linux/android/libdl.so.c"
    "$root_dir/cc.sh" -fPIC -shared -o "$OUT/libandroid.so" "$root_dir/src/hc/linux/android/libandroid.so.c"
    "$root_dir/tools/builder.sh" "$script_dir/android/libopenGl.so.c"
    "$root_dir/objcopy.sh" --strip-all "$OUT/$ARCH-${ABI}_libopenGl.so"
}

build_apk() {
    rm -rf "$OUT/apk"
    mkdir -p "$OUT/apk/lib/x86_64"
    cp "$OUT/${1}x86_64-linux-android26_libopenGl.so" "$OUT/apk/lib/x86_64/libopenGl.so"
    mkdir "$OUT/apk/lib/arm64-v8a"
    cp "$OUT/${1}aarch64-linux-android26_libopenGl.so" "$OUT/apk/lib/arm64-v8a/libopenGl.so"

    "$ANDROID_SDK/build-tools/26.0.3/aapt" package $2 -f -F "$OUT/${1}openGl.apk" -M "$script_dir/android/AndroidManifest.xml" -I "$ANDROID_SDK/platforms/android-26/android.jar" "$OUT/apk"
}

sign_apk() {
    "${java_prefix}java" -jar "$ANDROID_SDK/build-tools/26.0.3/lib/apksigner.jar" sign --ks "$KEYSTORE" --ks-pass "$KEYSTORE_PASS" "$OUT/${1}openGl.apk"
}

if test -z "$NO_X86_64"; then export ARCH=x86_64; build; fi
if test -z "$NO_AARCH64"; then export ARCH=aarch64; build; fi
if test -z "$NO_RISCV64"; then export ARCH=riscv64; build; fi
if test -z "$NO_WASM32"; then
    export ARCH=wasm32
    export ABI=unknown
    export FLAGS="-Wl,--no-entry"
    export FLAGS_RELEASE="-Os -s"
    export FLAGS_DEBUG="-g"
    "$root_dir/tools/builder.sh" "$script_dir/web/openGl.wasm.c"

    "$root_dir/tools/htmlPacker/build.sh"
    "$OUT/htmlPacker" "$OUT/openGl.html" "$script_dir/web/_start.html" "$OUT"
fi

export ARCH=x86_64; build_android;
export ARCH=aarch64; build_android;
# TODO: riscv64?

if test -z "$ANDROID_SDK"; then
    # Download android command line tools: https://developer.android.com/studio (scroll down)
    # Run: bin/sdkmanager --sdk_root=. --install "build-tools;26.0.3" "platforms;android-26"
    echo "Set ANDROID_SDK to build apks"
else
    if test -n "$JAVA_HOME"; then java_prefix="$JAVA_HOME/bin/"; fi

    if test -z "$NO_DEBUG"; then build_apk "debug_" "--debug-mode"; fi
    build_apk "" ""

    if test -z "$KEYSTORE"; then
        # keytool -genkeypair -keyalg RSA -validity 100000 -keystore my.keystore
        echo "Set KEYSTORE (and optionally KEYSTORE_PASS) to sign apks"
    else
        KEYSTORE_PASS="${KEYSTORE_PASS:-stdin}"
        if test -z "$NO_DEBUG"; then sign_apk "debug_"; fi
        sign_apk ""
    fi
fi

