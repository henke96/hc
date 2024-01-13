recipe_start() {
    SCRIPT_NAME="${0##*/}"

    if test -z "$hc_TIMESTAMP"; then
        if test -z "$OUT"; then
            set +x
            echo 'Please set environment variable `OUT` to the output directory.'
            exit 1
        fi
        OUT="$(cd -- "$OUT" && pwd)"
        test "$OUT" != "$SCRIPT_DIR"
        DOWNLOADS="$(cd -- "${DOWNLOADS:-"$OUT"}" && pwd)"
        # Run ourself with a clean environment.
        exec env -i hc_TIMESTAMP="$(date)" hc_DOWNLOADS="$DOWNLOADS" OUT="$OUT" NUM_CPUS="${NUM_CPUS:-1}" CC="${CC:-cc}" CXX="${CXX:-c++}" PATH="$PATH" TERM=xterm SHELL=/bin/sh SOURCE_DATE_EPOCH=0 TZ=UTC0 LC_ALL=C "$SCRIPT_DIR/$SCRIPT_NAME"
    fi
    cd "$SCRIPT_DIR"
    hc_outdir="$OUT/$SCRIPT_NAME"

    # Build dependencies if needed.
    for hc_dep in $DEPENDENCIES; do
        if test "$hc_TIMESTAMP" != "$(cat "$OUT/${hc_dep##*/}/sha512-timestamp")"; then "./$hc_dep"; fi
    done

    # Check if the recipe needs to be rebuilt.
    if test -n "$DIR_DEPENDENCIES"; then
        find $DIR_DEPENDENCIES -type d -print -o -exec sha512sum {} + | sort > "$hc_outdir-dirdeps"
    fi
    if sha512sum -c "$hc_outdir/sha512"; then
        echo "$hc_TIMESTAMP" > "$hc_outdir/sha512-timestamp"
        set +x
        echo "Already built $hc_outdir"
        exit
    fi
    rm -rf "$hc_outdir"
    mkdir -p "$hc_outdir"
    cd "$hc_outdir"
}

recipe_finish() {
    cd "$SCRIPT_DIR"
    sha512sum "./$SCRIPT_NAME" > "$hc_outdir/temp.sha512"
    for hc_dep in $DEPENDENCIES; do
        sha512sum "$OUT/${hc_dep##*/}/sha512" >> "$hc_outdir/temp.sha512"
    done
    if test -n "$DIR_DEPENDENCIES"; then
        sha512sum "$hc_outdir-dirdeps" >> "$hc_outdir/temp.sha512"
    fi
    mv "$hc_outdir/temp.sha512" "$hc_outdir/sha512"
    echo "$hc_TIMESTAMP" > "$hc_outdir/sha512-timestamp"
    set +x
    echo "Built $hc_outdir"
}

recipe_download() {
    cd "$hc_DOWNLOADS"
    hc_filename="${1##*/}"
    if ! sha512sum -c - >&2 <<end
$2 $hc_filename
end
    then
        wget -O "$hc_filename" "$1" >&2 || fetch -o "$hc_filename" "$1" >&2
        sha512sum -c - >&2 <<end
$2  $hc_filename
end
    fi
    printf "%s" "$PWD/$hc_filename"
}
