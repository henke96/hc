#!/bin/sh --
set -e
cd -- "$(dirname -- "$0")"
. ./recipe.sh
recipe_init "./make.sh"

URL="https://github.com/tukaani-project/xz/releases/download/v5.4.3/xz-5.4.3.tar.gz"
SHA512="aff0fe166af6df4491a6f5df2372cab100b081452461a0e8c6fd65b72af3f250f16c64d9fb8fd309141e9b9ae4e41649f48687cc29e63dd82f27f2eab19b4023"

recipe_start
./configure --prefix="$SCRIPT_DIR/$RECIPE_NAME" --disable-nls --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix
make -j "$NUM_CPUS"
mkdir -p "../$RECIPE_NAME/bin"
mkdir -p "../$RECIPE_NAME/lib"
install -c ./src/xz/.libs/xz "../$RECIPE_NAME/bin/"
install -c ./src/liblzma/.libs/liblzma.so.* "../$RECIPE_NAME/lib/"
recipe_finish
