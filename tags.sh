#!/bin/sh
set -e
cd -- "${0%/*}/"
ctags --extras=Ff -R .
