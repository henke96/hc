#!/bin/sh --
set -e
cd -- "$(dirname -- "$0")"
. ./recipe.sh
recipe_init ""

URL="https://ftp.gnu.org/gnu/make/make-4.4.tar.gz"
SHA256="581f4d4e872da74b3941c874215898a7d35802f03732bdccee1d4a7979105d18"

recipe_start
./configure --prefix="/path/that/doesnt/exist" --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix --without-guile --without-customs --without-dmalloc
./build.sh
mkdir -p "../$RECIPE_NAME/bin"
install -c ./make "../$RECIPE_NAME/bin/make"
recipe_finish
