#!/bin/sh --
set -e
script_dir="$(cd -- "$(dirname -- "$0")" && pwd)"
. "$script_dir/recipe.sh"

URL="https://www.python.org/ftp/python/3.11.4/Python-3.11.4.tar.xz"
SHA256="2f0e409df2ab57aa9fc4cbddfb976af44e4e55bf6f619eee6bc5c2297264a7f6"
DEPENDENCIES="./make.sh ./xz.sh"

recipe_start
./configure --prefix="/path/that/doesnt/exist" --without-ensurepip
make -j "$NUM_CPUS" install DESTDIR=../python/ prefix=../python/
recipe_finish
