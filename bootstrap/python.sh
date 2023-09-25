#!/bin/sh --
set -e
cd -- "$(dirname -- "$0")"
. ./recipe.sh
recipe_init "./make.sh ./xz.sh"

URL="https://www.python.org/ftp/python/3.11.4/Python-3.11.4.tar.xz"
SHA256="2f0e409df2ab57aa9fc4cbddfb976af44e4e55bf6f619eee6bc5c2297264a7f6"

recipe_start
./configure --prefix="$SCRIPT_DIR/$RECIPE_NAME" --without-ensurepip
make -j "$NUM_CPUS" install DESTDIR="../$RECIPE_NAME/" prefix="../$RECIPE_NAME/"
recipe_finish
