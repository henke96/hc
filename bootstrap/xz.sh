#!/bin/sh --
set -e
cd -- "$(dirname -- "$0")"
. ./recipe.sh
recipe_init "./make.sh"

URL="https://github.com/tukaani-project/xz/releases/download/v5.4.3/xz-5.4.3.tar.gz"
SHA256="1c382e0bc2e4e0af58398a903dd62fff7e510171d2de47a1ebe06d1528e9b7e9"

recipe_start
./configure --prefix="$SCRIPT_DIR/$RECIPE_NAME" --disable-nls --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix
make -j "$NUM_CPUS"
mkdir -p "../$RECIPE_NAME/bin"
mkdir -p "../$RECIPE_NAME/lib"
install -c ./src/xz/.libs/xz "../$RECIPE_NAME/bin/"
install -c ./src/liblzma/.libs/liblzma.so.* "../$RECIPE_NAME/lib/"
recipe_finish
