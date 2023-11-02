#!/bin/sh --
set -e
cd -- "$(dirname -- "$0")"
. ./recipe.sh
recipe_init "./make.sh"

URL="https://github.com/Kitware/CMake/releases/download/v3.27.1/cmake-3.27.1.tar.gz"
SHA512="d8831105fed3772bcb821a2b184fe0ffbff15795a5b0a89f2ad1cb0372a40c8f22f2bab72c671a6d17464521b7337067df929a5c3ece99f84848cc3a2fe6716f"

recipe_start
./bootstrap --prefix="$SCRIPT_DIR/$RECIPE_NAME" --no-debugger --parallel="$NUM_CPUS" --generator="Unix Makefiles" -- -DCMAKE_USE_OPENSSL=OFF
make -j "$NUM_CPUS" install
recipe_finish
