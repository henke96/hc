recipe_start() {
    SCRIPT_NAME="${0##*/}"
    if test -z "$recipe_TIMESTAMP"; then
        if test -z "$OUT"; then
            set +x
            echo 'Please set environment variable `OUT` to the output directory.'
            exit 1
        fi
        OUT="$(cd -- "$OUT" && pwd)"
        test "$OUT" != "$SCRIPT_DIR"
        DOWNLOADS="$(cd -- "${DOWNLOADS:-"$OUT"}" && pwd)"
        # Run ourself with a clean environment.
        exec env -i recipe_TIMESTAMP="$(date)" recipe_DOWNLOADS="$DOWNLOADS" OUT="$OUT" NUM_CPUS="${NUM_CPUS:-1}" CC="${CC:-cc}" CXX="${CXX:-c++}" PATH="$PATH" TERM=xterm SOURCE_DATE_EPOCH=0 TZ=UTC0 LC_ALL=C sh "$SCRIPT_DIR/$SCRIPT_NAME"
    fi
    cd "$SCRIPT_DIR"
    recipe_outdir="$OUT/$SCRIPT_NAME"

    # Build dependencies if needed.
    for recipe_dep in $DEPENDENCIES; do
        if test "$recipe_TIMESTAMP" != "$(cat "$OUT/${recipe_dep##*/}/sha512-timestamp")"; then sh "./$recipe_dep"; fi
    done

    # Check if the recipe needs to be rebuilt.
    if test -n "$DIR_DEPENDENCIES"; then
        find $DIR_DEPENDENCIES -type d -print -o -exec sha512sum {} + | sort > "$recipe_outdir-dirdeps"
    fi
    if sha512sum -c "$recipe_outdir/sha512"; then
        echo "$recipe_TIMESTAMP" > "$recipe_outdir/sha512-timestamp"
        set +x
        echo "Already built $recipe_outdir"
        exit
    fi
    rm -rf "$recipe_outdir"
    mkdir -p "$recipe_outdir"
    cd "$recipe_outdir"
}

recipe_finish() {
    cd "$SCRIPT_DIR"
    sha512sum "./$SCRIPT_NAME" > "$recipe_outdir/temp.sha512"
    for recipe_dep in $DEPENDENCIES; do
        sha512sum "$OUT/${recipe_dep##*/}/sha512" >> "$recipe_outdir/temp.sha512"
    done
    if test -n "$DIR_DEPENDENCIES"; then
        sha512sum "$recipe_outdir-dirdeps" >> "$recipe_outdir/temp.sha512"
    fi
    mv "$recipe_outdir/temp.sha512" "$recipe_outdir/sha512"
    echo "$recipe_TIMESTAMP" > "$recipe_outdir/sha512-timestamp"
    set +x
    echo "Built $recipe_outdir"
}

recipe_download() {
    cd "$recipe_DOWNLOADS"
    recipe_filename="${1##*/}"
    if ! sha512sum -c - >&2 <<end
$2  $recipe_filename
end
    then
        wget -O "$recipe_filename" "$1" >&2 || fetch -o "$recipe_filename" "$1" >&2
        sha512sum -c - >&2 <<end
$2  $recipe_filename
end
    fi
    printf "%s" "$PWD/$recipe_filename"
}
