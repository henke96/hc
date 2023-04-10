@echo off
setlocal
set /p flags=<"%~dp0flags"
"%LLVM%clang" -I"%~dp0src" %flags% -target wasm32-unknown-unknown-unknown --ld-path="%LLVM%wasm-ld" %*
