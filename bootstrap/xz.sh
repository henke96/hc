#!/bin/sh --
set -eax
cd -- "${0%/*}/"
. ./recipe.sh
recipe_init "./make.sh" ""

pkg="xz-5.4.3"
URL="https://github.com/tukaani-project/xz/releases/download/v5.4.3/$pkg.tar.gz"
SHA512="aff0fe166af6df4491a6f5df2372cab100b081452461a0e8c6fd65b72af3f250f16c64d9fb8fd309141e9b9ae4e41649f48687cc29e63dd82f27f2eab19b4023"

recipe_start
rm -rf "./$pkg"; gzip -d -c "$DOWNLOAD" | tar xf -; cd "./$pkg"

./configure --prefix="$SCRIPT_DIR/$RECIPE_NAME" --disable-nls --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix
make -j "$NUM_CPUS"
mkdir -p "../$RECIPE_NAME/bin"
mkdir "../$RECIPE_NAME/lib"
mv ./src/xz/.libs/xz "../$RECIPE_NAME/bin/"
mv ./src/liblzma/.libs/liblzma.so.* "../$RECIPE_NAME/lib/"

cd ..; rm -rf "./$pkg"
recipe_finish
