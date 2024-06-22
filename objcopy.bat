@echo off
setlocal disabledelayedexpansion

if defined LLVM set "llvm_prefix=%LLVM%\bin\"

"%llvm_prefix%llvm-objcopy" %*
