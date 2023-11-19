#!/bin/sh --
set -ex
cd -- "$(dirname -- "$0")"
. ./recipe.sh
recipe_init "./make.sh ./xz.sh ./cmake.sh ./python.sh" ""

pkg="llvm-project-16.0.6.src"
URL="https://github.com/llvm/llvm-project/releases/download/llvmorg-16.0.6/$pkg.tar.xz"
SHA512="89a67ebfbbc764cc456e8825ecfa90707741f8835b1b2adffae0b227ab1fe5ca9cce75b0efaffc9ca8431cae528dc54fd838867a56a2b645344d9e82d19ab1b7"

recipe_start
rm -rf "./$pkg"; xz -d -c "$DOWNLOAD" | tar xf -; cd "./$pkg"

cmake -S llvm -B build -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="clang;lld;lldb" -DLLVM_TARGETS_TO_BUILD="AArch64;RISCV;WebAssembly;X86" -DLLVM_ENABLE_LIBXML2=OFF -DLLVM_ENABLE_LIBEDIT=OFF -DLLVM_ENABLE_LIBPFM=OFF -DLLVM_ENABLE_ZLIB=OFF -DLLVM_ENABLE_BINDINGS=OFF -DLLVM_ENABLE_UNWIND_TABLES=OFF -DCLANG_ENABLE_ARCMT=OFF -DCMAKE_INSTALL_RPATH="$SCRIPT_DIR/$RECIPE_NAME/lib" -DCMAKE_INSTALL_PREFIX="$SCRIPT_DIR/$RECIPE_NAME" -DCMAKE_BUILD_TYPE=Release -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_TESTS=OFF -DLLVM_APPEND_VC_REV=OFF -DCMAKE_MAKE_PROGRAM=make
make -C build -j "$NUM_CPUS" install

cd ..; rm -rf "./$pkg"
recipe_finish
