#!/bin/sh
script_dir="$(dirname "$0")"

javac jni/Test.java
java -Djava.library.path="$script_dir/linux" jni/Test
