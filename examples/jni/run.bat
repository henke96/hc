@echo off
javac "%~dp0jni/Test.java"
java -cp "%~dp0\" -Djava.library.path="%~dp0windows" jni/Test
