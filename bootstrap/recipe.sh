recipe_init() {
    DEPENDENCIES="$1"
    SCRIPT_NAME="$(basename -- "$0")"
    RECIPE_NAME="${SCRIPT_NAME%.sh}"

    if test -z "$BUILD_TIMESTAMP"; then
        export BUILD_TIMESTAMP="$(date)"

        if test -n "$START_DEV"; then
            rm -rf "./$RECIPE_NAME"
            mkdir "./$RECIPE_NAME"
            echo "$BUILD_TIMESTAMP" > "./$RECIPE_NAME/development"
            echo "$RECIPE_NAME is now in development mode."
            exit
        elif test -n "$STOP_DEV"; then
            rm -rf "./$RECIPE_NAME"
            echo "$RECIPE_NAME is no longer in development mode."
            exit
        fi
    fi

    # Run dependencies recipes that have not already been built.
    for recipe in $DEPENDENCIES; do
        if test "$BUILD_TIMESTAMP" != "$(cat "${recipe%.sh}/sha512-timestamp")"; then "$recipe"; fi
    done

    # Check if the recipe needs to be rebuilt.
    if sha512sum -c "./$RECIPE_NAME/sha512"; then
        echo "$BUILD_TIMESTAMP" > "./$RECIPE_NAME/sha512-timestamp"
        exit
    fi

    NUM_CPUS="${NUM_CPUS:-1}"
    SCRIPT_DIR="$(pwd)"
}

recipe_start() {
    # Add dependencies bin dirs to PATH.
    for recipe in $DEPENDENCIES; do
        recipe_bin_path="$(cd -- "${recipe%.sh}" && pwd)/bin"
        if test -d "$recipe_bin_path"; then export PATH="$recipe_bin_path:$PATH"; fi
    done
    # Assist with reproducible builds.
    export SOURCE_DATE_EPOCH=0 TZ=UTC0 LC_ALL=C

    if test -f "./$RECIPE_NAME/development"; then
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
                $SHELL
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
        url_filename="${URL##*/}"
        url_ext="${url_filename##*.tar}"
        url_base="${URL%/*}"
        SOURCE_DIR_NAME="${url_filename%.tar*}"
        if test -n "$MIRROR"; then url_base="$MIRROR"; fi

        # Fetch and verify source.
        if ! sha512sum -c - <<end
$SHA512  $url_filename
end
        then
            if ! { curl -LO "$url_base/$url_filename" || wget "$url_base/$url_filename" || fetch "$url_base/$url_filename"; }
            then
                rm -f "./$url_filename"
                echo "Failed to download $url_base/$url_filename"
                echo "Please fetch \"$SCRIPT_DIR/$url_filename\" manually, then press enter to continue"
                read -r answer
            fi
            sha512sum -c - <<end
$SHA512  $url_filename
end
        fi

        # Extract source tar.
        rm -rf "./$SOURCE_DIR_NAME"
        if test "$url_ext" = ".gz"; then
            gzip -d -c "./$url_filename" | tar xf -
        elif test "$url_ext" = ".xz"; then
            xz -d -c "./$url_filename" | tar xf -
        elif test "$url_ext" = ".bz2"; then
            bzip2 -d -c "./$url_filename" | tar xf -
        else
            tar xf "./$url_filename"
        fi
    else
        SOURCE_DIR_NAME="$RECIPE_NAME-temp"
        rm -rf "./$SOURCE_DIR_NAME"
        mkdir "./$SOURCE_DIR_NAME"
    fi
    rm -rf "./$RECIPE_NAME"
    cd "./$SOURCE_DIR_NAME"
}

recipe_finish() {
    cd ..
    rm -rf "./$SOURCE_DIR_NAME"

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
