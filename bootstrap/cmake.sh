#!/bin/sh --
set -e
cd -- "$(dirname -- "$0")"
. ./recipe.sh
recipe_init "./make.sh"

URL="https://github.com/Kitware/CMake/releases/download/v3.27.1/cmake-3.27.1.tar.gz"
SHA256="b1a6b0135fa11b94476e90f5b32c4c8fad480bf91cf22d0ded98ce22c5132004"

recipe_start
./bootstrap --prefix="$SCRIPT_DIR/$RECIPE_NAME" --no-debugger --parallel="$NUM_CPUS" --generator="Unix Makefiles" -- -DCMAKE_USE_OPENSSL=OFF
make -j "$NUM_CPUS" install
recipe_finish
