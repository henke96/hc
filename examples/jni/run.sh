#!/bin/sh
script_dir="$(dirname "$0")"

ARCH="$(uname -m)"

javac "$script_dir/jni/Test.java"
java -cp "$script_dir" -Djava.library.path="$script_dir/linux/$ARCH" jni/Test
