#!/bin/sh
set -e
cd -- "${0%/*}/"
${CTAGS:-ctags} $(find . -name "*.[ch]")
