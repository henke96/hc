#!/bin/sh --
set -eax
SCRIPT_DIR="$(cd -- "${0%/*}/" && pwd)"
. "$SCRIPT_DIR/../tools/shell/recipe.sh"
recipe_init "make.sh xz.sh"

pkg="Python-3.11.4"
URL="https://www.python.org/ftp/python/3.11.4/$pkg.tar.xz"
SHA512="7eb14fecbf60824d10c22a9057584c3a142c2866f4af6caa2525c10c8bcb24e6e7afb32a44a0e118df0a2b2543d578c3b422ffd4a5fa317dfe6ea371cc7ee1ee"

recipe_start
export PATH="$PWD/make.sh-out/bin:$PWD/xz.sh-out/bin:$PATH"
rm -rf "./$pkg"; xz -d -c "$DOWNLOAD" | tar xf -; cd "./$pkg"

./configure --prefix="$RECIPE_OUT" --without-ensurepip
make -j "$NUM_CPUS" install

cd ..; rm -rf "./$pkg"
recipe_finish
