#!/bin/sh
set -e

if test -z "$1"
then
    echo "Usage: $0 PATH PROGRAM_NAME"
    exit 1
fi

script_dir="$(dirname $0)"
root_dir="$script_dir/../.."

common_flags="-Wl,-subsystem,efi_application"
debug_flags="$common_flags -fsanitize-undefined-trap-on-error -fsanitize=undefined -g -O2 $FLAGS"
release_flags="$common_flags -Ddebug_NDEBUG -O2 -s $FLAGS"
"$root_dir/cc_pe.sh" $debug_flags -S -o "$1/$2.debug.efi.s" "$1/$2.c" $LFLAGS
"$root_dir/cc_pe.sh" $debug_flags -o "$1/$2.debug.efi" "$1/$2.c" $LFLAGS
"$root_dir/cc_pe.sh" $release_flags -S -o "$1/$2.efi.s" "$1/$2.c" $LFLAGS
"$root_dir/cc_pe.sh" $release_flags -o "$1/$2.efi" "$1/$2.c" $LFLAGS

# Static analysis.
analyse_flags="--analyze --analyzer-output text -Xclang -analyzer-opt-analyze-headers"
"$root_dir/cc_pe.sh" $debug_flags $analyse_flags "$1/$2.c" $LFLAGS
"$root_dir/cc_pe.sh" $release_flags $analyse_flags "$1/$2.c" $LFLAGS
