#!/bin/sh --
set -eax
SCRIPT_DIR="$(cd -- "${0%/*}/" && pwd)"
. "$SCRIPT_DIR/../tools/shell/recipe.sh"
recipe_init "make.sh"

pkg="xz-5.4.3"
URL="https://github.com/tukaani-project/xz/releases/download/v5.4.3/$pkg.tar.gz"
SHA512="aff0fe166af6df4491a6f5df2372cab100b081452461a0e8c6fd65b72af3f250f16c64d9fb8fd309141e9b9ae4e41649f48687cc29e63dd82f27f2eab19b4023"

recipe_start
export PATH="$PWD/make.sh-out/bin:$PATH"
rm -rf "./$pkg"; gzip -d -c "$DOWNLOAD" | tar xf -; cd "./$pkg"

./configure --prefix="$RECIPE_OUT" --disable-nls --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix
make -j "$NUM_CPUS" install

cd ..; rm -rf "./$pkg"
recipe_finish
