#!/bin/sh
script_dir="$(dirname "$0")"

javac "$script_dir/jni/Test.java"
java -cp "$script_dir" -Djava.library.path="$script_dir/linux" jni/Test
