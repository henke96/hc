#!/bin/sh
set -e
script_dir="$(dirname "$(realpath "$0")")"
root_dir="$script_dir/root"
mkdir -p "$root_dir"

LLVM_PROJECTS="${LLVM_PROJECTS:-clang;lld;lldb}"
NUM_CPUS="${NUM_CPUS:-1}"
export PATH="$root_dir/bin:$PATH"
. "$script_dir/_helpers.sh"

pkg_make="make-4.4"
pkg_ext_make=".tar.gz"
pkg_xz="xz-5.4.3"
pkg_ext_xz=".tar.gz"
pkg_cmake="cmake-3.27.1"
pkg_ext_cmake=".tar.gz"
pkg_python="Python-3.11.4"
pkg_ext_python=".tar.xz"
pkg_llvm="llvm-project-16.0.6.src"
pkg_ext_llvm=".tar.xz"

cd "$script_dir"
rm -rf temp
fetch_file "https://ftp.gnu.org/gnu/make/" "$pkg_make$pkg_ext_make"
fetch_file "https://github.com/tukaani-project/xz/releases/download/v5.4.3/" "$pkg_xz$pkg_ext_xz"
fetch_file "https://github.com/Kitware/CMake/releases/download/v3.27.1/" "$pkg_cmake$pkg_ext_cmake"
fetch_file "https://www.python.org/ftp/python/3.11.4/" "$pkg_python$pkg_ext_python"
fetch_file "https://github.com/llvm/llvm-project/releases/download/llvmorg-16.0.6/" "$pkg_llvm$pkg_ext_llvm"

echo 'b1a6b0135fa11b94476e90f5b32c4c8fad480bf91cf22d0ded98ce22c5132004  cmake-3.27.1.tar.gz
581f4d4e872da74b3941c874215898a7d35802f03732bdccee1d4a7979105d18  make-4.4.tar.gz
1c382e0bc2e4e0af58398a903dd62fff7e510171d2de47a1ebe06d1528e9b7e9  xz-5.4.3.tar.gz
ce5e71081d17ce9e86d7cbcfa28c4b04b9300f8fb7e78422b1feb6bc52c3028e  llvm-project-16.0.6.src.tar.xz
2f0e409df2ab57aa9fc4cbddfb976af44e4e55bf6f619eee6bc5c2297264a7f6  Python-3.11.4.tar.xz' > sha256.sum
verify_checksums sha256.sum

# Build GNU make.
cd "$script_dir" && extract_and_enter "$pkg_make" "$pkg_ext_make"
./configure --prefix="$root_dir" --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix --without-guile --without-customs --without-dmalloc
./build.sh
./make -j$NUM_CPUS install

# Build XZ Utils.
cd "$script_dir" && extract_and_enter "$pkg_xz" "$pkg_ext_xz"
./configure --prefix="$root_dir" --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix
make -j$NUM_CPUS install

# Build CMake.
cd "$script_dir" && extract_and_enter "$pkg_cmake" "$pkg_ext_cmake"
./bootstrap --prefix="$root_dir" --no-debugger --parallel=$NUM_CPUS --generator="Unix Makefiles" -- -DCMAKE_USE_OPENSSL=OFF
make -j$NUM_CPUS install

# Build Python3.
cd "$script_dir" && extract_and_enter "$pkg_python" "$pkg_ext_python"
./configure --prefix="$root_dir" --without-ensurepip
make -j$NUM_CPUS install

# Build LLVM.
cd "$script_dir" && extract_and_enter "$pkg_llvm" "$pkg_ext_llvm"
cmake -S llvm -B build -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="$LLVM_PROJECTS" -DLLVM_TARGETS_TO_BUILD="AArch64;RISCV;WebAssembly;X86" -DLLVM_ENABLE_LIBXML2=OFF -DLLVM_ENABLE_LIBEDIT=OFF -DLLVM_ENABLE_LIBPFM=OFF -DLLVM_ENABLE_ZLIB=OFF -DLLVM_ENABLE_BINDINGS=OFF -DLLVM_ENABLE_UNWIND_TABLES=OFF -DCLANG_ENABLE_ARCMT=OFF -DCMAKE_INSTALL_PREFIX="$root_dir" -DCMAKE_BUILD_TYPE=Release -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_TESTS=OFF
make -C build -j$NUM_CPUS install
