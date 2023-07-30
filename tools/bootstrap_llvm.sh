#!/bin/sh
# Bootstrap LLVM from minimal dependencies.
set -e
script_dir="$(dirname "$(realpath "$0")")"

llvm_dir="$script_dir/llvm"
temp_dir="$script_dir/temp"
mkdir "$llvm_dir"
mkdir "$temp_dir"

numcpus=${1:-1}
export PATH="$temp_dir/bin:$PATH"

# Build GNU make.
pkg_name=make-4.4
wget -nc -P "$temp_dir" "https://ftp.gnu.org/gnu/make/$pkg_name.tar.gz" || :
tar x -C "$temp_dir" -k -z -f "$temp_dir/$pkg_name.tar.gz"
cd "$temp_dir/$pkg_name"
./configure --prefix="$temp_dir" --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix --without-guile --without-customs --without-dmalloc
./build.sh
./make -j$numcpus install

# Build XZ Utils.
pkg_name=xz-5.4.3
wget -nc -P "$temp_dir" "https://github.com/tukaani-project/xz/releases/download/v5.4.3/$pkg_name.tar.gz" || :
tar x -C "$temp_dir" -k -z -f "$temp_dir/$pkg_name.tar.gz"
cd "$temp_dir/$pkg_name"
./configure --prefix="$temp_dir" --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix
make -j$numcpus install

# Build CMake.
pkg_name=cmake-3.27.1
wget -nc -P "$temp_dir" "https://github.com/Kitware/CMake/releases/download/v3.27.1/$pkg_name.tar.gz" || :
tar x -C "$temp_dir" -k -z -f "$temp_dir/$pkg_name.tar.gz"
cd "$temp_dir/$pkg_name"
./bootstrap --prefix="$temp_dir" --no-debugger --parallel=$numcpus --generator="Unix Makefiles" -- -DCMAKE_USE_OPENSSL=OFF
make -j$numcpus install

# Build Python3.
pkg_name=Python-3.11.4
wget -nc -P "$temp_dir" "https://www.python.org/ftp/python/3.11.4/$pkg_name.tar.xz" || :
tar x -C "$temp_dir" -k -J -f "$temp_dir/$pkg_name.tar.xz"
cd "$temp_dir/$pkg_name"
./configure --prefix="$temp_dir" --without-ensurepip
make -j$numcpus install

# Build LLVM.
pkg_name=llvm-project-16.0.6.src
wget -nc -P "$temp_dir" "https://github.com/llvm/llvm-project/releases/download/llvmorg-16.0.6/$pkg_name.tar.xz" || :
tar x -C "$temp_dir" -k -J -f "$temp_dir/$pkg_name.tar.xz"
cd "$temp_dir/$pkg_name"
cmake -S llvm -B build -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="clang;lld" -DLLVM_TARGETS_TO_BUILD="AArch64;RISCV;WebAssembly;X86" -DLLVM_ENABLE_LIBXML2=OFF -DLLVM_ENABLE_LIBEDIT=OFF -DLLVM_ENABLE_LIBPFM=OFF -DLLVM_ENABLE_ZLIB=OFF -DLLVM_ENABLE_BINDINGS=OFF -DLLVM_ENABLE_UNWIND_TABLES=OFF -DCLANG_ENABLE_ARCMT=OFF -DCMAKE_INSTALL_PREFIX="$llvm_dir" -DCMAKE_BUILD_TYPE=Release -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_TESTS=OFF
make -C build -j$numcpus install-clang install-lld install-llvm-objcopy

echo "LLVM bootstrap successful!"
echo "Set LLVM=\"$llvm_dir\" to use it."
