recipe_start() {
    SCRIPT_NAME="${0##*/}"

    if test -z "$hc_TIMESTAMP"; then
        if test -z "$OUT"; then
            set +x
            echo 'Please set environment variable `OUT` to the output directory.'
            exit 1
        fi
        DOWNLOADS="$(cd -- "${DOWNLOADS:-"$OUT"}" && pwd)"
        cd -- "$OUT"
        if test -n "$START_DEV"; then
            rm -rf "./$SCRIPT_NAME-out"
            echo "$(date)" > "./$SCRIPT_NAME-development"
            set +x
            echo "$SCRIPT_NAME is now in development mode."
            exit
        elif test -n "$STOP_DEV"; then
            rm -rf "./$SCRIPT_NAME-out"
            rm "./$SCRIPT_NAME-development"
            set +x
            echo "$SCRIPT_NAME is no longer in development mode."
            exit
        fi
        # Run ourself with a clean environment.
        exec env -i hc_TIMESTAMP="$(date)" hc_DOWNLOADS="$DOWNLOADS" hc_SHELL="${SHELL:-/bin/sh}" NUM_CPUS="${NUM_CPUS:-1}" CC="${CC:-cc}" CXX="${CXX:-c++}" PATH="$PATH" TERM=xterm SHELL=/bin/sh SOURCE_DATE_EPOCH=0 TZ=UTC0 LC_ALL=C "$SCRIPT_DIR/$SCRIPT_NAME"
    fi

    # Run dependencies recipes that have not already been built.
    for hc_dep in $DEPENDENCIES; do
        if test "$hc_TIMESTAMP" != "$(cat "./${hc_dep##*/}-out/sha512-timestamp")"; then "$SCRIPT_DIR/$hc_dep"; fi
    done

    # Check if the recipe needs to be rebuilt.
    RECIPE_OUT="$PWD/$SCRIPT_NAME-out"
    if sha512sum -c "$RECIPE_OUT/sha512"; then
        echo "$hc_TIMESTAMP" > "$RECIPE_OUT/sha512-timestamp"
        set +x
        echo "Already built $RECIPE_OUT"
        exit
    fi

    # Download and verify source.
    if test -n "$URL"; then
        DOWNLOAD="$hc_DOWNLOADS/${URL##*/}"
        if ! sha512sum -c - <<end
$SHA512  $DOWNLOAD
end
        then
            wget -O "$DOWNLOAD" "$URL" || fetch -o "$DOWNLOAD" "$URL"
            sha512sum -c - <<end
$SHA512  $DOWNLOAD
end
        fi
    fi
    set +a

    # Handle development mode.
    if test -f "./$SCRIPT_NAME-development"; then
        set +x
        while :; do
            echo "$SCRIPT_NAME is in development mode."
            echo "1) Enter shell for manual build"
            echo "2) Continue without build"
            echo -n "Select option: "
            read -r hc_answer
            case "$hc_answer" in
                1)
                set -x
                rm -rf "$RECIPE_OUT"
                set +x
                echo "Entering shell. Type \`exit\` when done with build."
                $hc_SHELL
                set -x
                # Modify sha512 file, but keep the hash invalid.
                sha512sum "./$SCRIPT_NAME-development" > "$RECIPE_OUT/temp.sha512"
                echo "$hc_TIMESTAMP" > "./$SCRIPT_NAME-development"
                mv "$RECIPE_OUT/temp.sha512" "$RECIPE_OUT/sha512"
                echo "$hc_TIMESTAMP" > "$RECIPE_OUT/sha512-timestamp"
                set +x
                echo "Manually built $SCRIPT_NAME"
                exit
                ;;
                2)
                exit
                ;;
            esac
        done
    fi
    rm -rf "$RECIPE_OUT"
}

recipe_finish() {
    sha512sum "$0" > "$RECIPE_OUT/temp.sha512"
    for hc_dep in $DEPENDENCIES; do
        sha512sum "$PWD/${hc_dep##*/}-out/sha512" >> "$RECIPE_OUT/temp.sha512"
    done
    for hc_dep in $FILE_DEPENDENCIES; do
        sha512sum "$SCRIPT_DIR/$hc_dep" >> "$RECIPE_OUT/temp.sha512"
    done
    mv "$RECIPE_OUT/temp.sha512" "$RECIPE_OUT/sha512"
    echo "$hc_TIMESTAMP" > "$RECIPE_OUT/sha512-timestamp"
    set +x
    echo "Built $RECIPE_OUT"
}
