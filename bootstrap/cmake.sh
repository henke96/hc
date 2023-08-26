#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
. "$script_dir/recipe.sh"

URL="https://github.com/Kitware/CMake/releases/download/v3.27.1/cmake-3.27.1.tar.gz"
SHA256="b1a6b0135fa11b94476e90f5b32c4c8fad480bf91cf22d0ded98ce22c5132004"
DEPENDENCIES="./make.sh"

recipe_start
./bootstrap --prefix="$script_dir/cmake" --no-debugger --parallel="$NUM_CPUS" --generator="Unix Makefiles" -- -DCMAKE_USE_OPENSSL=OFF
make -j "$NUM_CPUS" install
recipe_finish
