#!/bin/sh --
set -ex
cd -- "$(dirname -- "$0")"
. ./recipe.sh
recipe_init "" ""

URL="https://ftp.gnu.org/gnu/make/make-4.4.tar.gz"
SHA512="4be73f494295dcfa10034531b0d920cfdb5438bc20625f863f5c878549c140e1e67195162580c53060c3c11c67a2c739c09051f02cdd283e5aa9ebcd68975a1f"

recipe_start
./configure --prefix="$SCRIPT_DIR/$RECIPE_NAME" --disable-dependency-tracking --without-libiconv-prefix --without-libintl-prefix --without-guile --without-customs --without-dmalloc
./build.sh
mkdir -p "../$RECIPE_NAME/bin"
install -c ./make "../$RECIPE_NAME/bin/"
recipe_finish
