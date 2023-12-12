recipe_init() {
    if test -z "$hc_TIMESTAMP"; then
        SCRIPT_NAME="${0##*/}"
        RECIPE_NAME="${SCRIPT_NAME%.sh}"
        if test -n "$START_DEV"; then
            rm -rf "./$RECIPE_NAME"
            echo "$(date)" > "./${RECIPE_NAME}-development"
            set +x
            echo "$RECIPE_NAME is now in development mode."
            exit
        elif test -n "$STOP_DEV"; then
            rm -rf "./$RECIPE_NAME"
            rm "./${RECIPE_NAME}-development"
            set +x
            echo "$RECIPE_NAME is no longer in development mode."
            exit
        fi
        # Run ourself with a clean environment.
        exec env -i hc_TIMESTAMP="$(date)" DOWNLOADS="${DOWNLOADS:-.}" NUM_CPUS="${NUM_CPUS:-1}" CC="${CC:-cc}" CXX="${CXX:-c++}" USER_SHELL="$SHELL" PATH="$PATH" TERM=xterm SHELL=/bin/sh SOURCE_DATE_EPOCH=0 TZ=UTC0 LC_ALL=C "./$SCRIPT_NAME"
    fi

    # Run dependencies recipes that have not already been built.
    for hc_temp in $1 $2; do
        if test "$hc_TIMESTAMP" != "$(cat "${hc_temp%.sh}/sha512-timestamp")"; then "$hc_temp"; fi
    done

    SCRIPT_NAME="${0##*/}"
    RECIPE_NAME="${SCRIPT_NAME%.sh}"

    # Check if the recipe needs to be rebuilt.
    if sha512sum -c "./$RECIPE_NAME/sha512"; then
        echo "$hc_TIMESTAMP" > "./$RECIPE_NAME/sha512-timestamp"
        set +x
        echo "Already built $RECIPE_NAME"
        exit
    fi

    # Add requested bin dirs to PATH.
    for hc_temp in $1; do
        PATH="$(cd -- "${hc_temp%.sh}" && pwd)/bin:$PATH"
    done
    hc_DEPS="$1 $2"
    SCRIPT_DIR="$(pwd)"
}

recipe_start() {
    if test -n "$URL"; then
        DOWNLOAD="$DOWNLOADS/${URL##*/}"
        # Fetch and verify source.
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

    if test -f "./${RECIPE_NAME}-development"; then
        set +x
        while :; do
            echo "$RECIPE_NAME is in development mode."
            echo "1) Enter shell for manual build"
            echo "2) Continue without build"
            echo -n "Select option: "
            read -r hc_temp
            case "$hc_temp" in
                1)
                set -x
                rm -rf "./$RECIPE_NAME"
                set +x
                echo "Entering shell. Type \`exit\` when done with build."
                $USER_SHELL
                set -x
                # Modify sha512 file, but keep the hash invalid.
                sha512sum "./${RECIPE_NAME}-development" > "./$RECIPE_NAME/temp.sha512"
                echo "$hc_TIMESTAMP" > "./${RECIPE_NAME}-development"
                mv "./$RECIPE_NAME/temp.sha512" "./$RECIPE_NAME/sha512"
                echo "$hc_TIMESTAMP" > "./$RECIPE_NAME/sha512-timestamp"
                set +x
                echo "Manually built $RECIPE_NAME"
                exit
                ;;
                2)
                exit
                ;;
            esac
        done
    fi
    rm -rf "./$RECIPE_NAME"
}

recipe_finish() {
    sha512sum "./$SCRIPT_NAME" > "./$RECIPE_NAME/temp.sha512"
    for hc_temp in $hc_DEPS; do
        sha512sum "${hc_temp%.sh}/sha512" >> "./$RECIPE_NAME/temp.sha512"
    done
    for hc_temp in $FILE_DEPENDENCIES; do
        sha512sum "$hc_temp" >> "./$RECIPE_NAME/temp.sha512"
    done
    mv "./$RECIPE_NAME/temp.sha512" "./$RECIPE_NAME/sha512"
    echo "$hc_TIMESTAMP" > "./$RECIPE_NAME/sha512-timestamp"
    set +x
    echo "Built $RECIPE_NAME"
}
