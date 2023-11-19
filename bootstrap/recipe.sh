recipe_init() {
    SCRIPT_NAME="$(basename -- "$0")"
    RECIPE_NAME="${SCRIPT_NAME%.sh}"

    if test -z "$BUILD_TIMESTAMP"; then
        export BUILD_TIMESTAMP="$(date)"

        if test -n "$START_DEV"; then
            rm -rf "./$RECIPE_NAME"
            mkdir "./$RECIPE_NAME"
            set +x
            echo "$BUILD_TIMESTAMP" > "./$RECIPE_NAME/development"
            echo "$RECIPE_NAME is now in development mode."
            exit
        elif test -n "$STOP_DEV"; then
            rm -rf "./$RECIPE_NAME"
            set +x
            echo "$RECIPE_NAME is no longer in development mode."
            exit
        fi
        # Run ourself with a clean environment.
        exec env -i BUILD_TIMESTAMP="$BUILD_TIMESTAMP" DOWNLOADS="${DOWNLOADS:-.}" NUM_CPUS="${NUM_CPUS:-1}" USER_SHELL="$SHELL" PATH="$PATH" TERM=xterm SHELL=/bin/sh CC=cc CXX=c++ SOURCE_DATE_EPOCH=0 TZ=UTC0 LC_ALL=C "./$SCRIPT_NAME"
    fi

    # Run dependencies recipes that have not already been built.
    for recipe in $1 $2; do
        if test "$BUILD_TIMESTAMP" != "$(cat "${recipe%.sh}/sha512-timestamp")"; then "$recipe"; fi
    done

    # Check if the recipe needs to be rebuilt.
    if sha512sum -c "./$RECIPE_NAME/sha512"; then
        echo "$BUILD_TIMESTAMP" > "./$RECIPE_NAME/sha512-timestamp"
        exit
    fi

    BIN_DEPENDENCIES="$1"
    SCRIPT_DIR="$(pwd)"
}

recipe_start() {
    # Add requested bin dirs to PATH.
    for recipe in $BIN_DEPENDENCIES; do
        recipe_bin_path="$(cd -- "${recipe%.sh}" && pwd)/bin"
        if test -d "$recipe_bin_path"; then export PATH="$recipe_bin_path:$PATH"; fi
    done

    if test -f "./$RECIPE_NAME/development"; then
        set +x
        while :; do
            echo "$RECIPE_NAME is in development mode."
            echo "1) Continue without rebuild"
            echo "2) Enter shell for manual rebuild"
            echo -n "Select option: "
            read -r answer
            case "$answer" in
                1)
                exit
                ;;
                2)
                rm -f "./$RECIPE_NAME/sha512"
                echo "Entering shell. Type \`exit\` when done with rebuild."
                export DEPENDENCIES SCRIPT_NAME RECIPE_NAME NUM_CPUS SCRIPT_DIR
                $USER_SHELL
                # Modify sha512 file, but keep the hash invalid.
                sha512sum "./$RECIPE_NAME/development" > "./$RECIPE_NAME/temp.sha512"
                echo "$BUILD_TIMESTAMP" > "./$RECIPE_NAME/development"
                mv "./$RECIPE_NAME/temp.sha512" "./$RECIPE_NAME/sha512"
                exit
                ;;
            esac
        done
    fi

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
    rm -rf "./$RECIPE_NAME"
}

recipe_finish() {
    sha512sum "./$SCRIPT_NAME" > "./$RECIPE_NAME/temp.sha512"
    for recipe in $DEPENDENCIES; do
        sha512sum "${recipe%.sh}/sha512" >> "./$RECIPE_NAME/temp.sha512"
    done
    for file in $FILE_DEPENDENCIES; do
        sha512sum "$file" >> "./$RECIPE_NAME/temp.sha512"
    done
    mv "./$RECIPE_NAME/temp.sha512" "./$RECIPE_NAME/sha512"
    echo "$BUILD_TIMESTAMP" > "./$RECIPE_NAME/sha512-timestamp"
}
