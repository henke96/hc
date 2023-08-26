#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
. "$script_dir/recipe.sh"

URL="https://github.com/tukaani-project/xz/releases/download/v5.4.3/xz-5.4.3.tar.gz"
SHA256="1c382e0bc2e4e0af58398a903dd62fff7e510171d2de47a1ebe06d1528e9b7e9"
DEPENDENCIES="./make.sh"

recipe_start
./configure --prefix="/path/that/doesnt/exist" --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix
make -j "$NUM_CPUS"
mkdir -p ../xz/bin
install -c ./src/xz/.libs/xz ../xz/bin/xz
recipe_finish
