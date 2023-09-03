recipe_init() {
    DEPENDENCIES="$1"
    NUM_CPUS="${NUM_CPUS:-1}"
    SCRIPT_DIR="$(pwd)"
    SCRIPT_NAME="$(basename -- "$0")"
    RECIPE_NAME="${SCRIPT_NAME%.sh}"

    if test -z "$BUILD_TIMESTAMP"; then
        export BUILD_TIMESTAMP="$(date)"
    fi

    # Run dependencies recipes.
    for recipe in $DEPENDENCIES; do
        "$recipe"
    done

    if test "$DEVELOPMENT" = "$RECIPE_NAME"; then
        rm -f "./$RECIPE_NAME/sha256-timestamp"
    fi

    # Check if recipe is already built.
    if test -f "./$RECIPE_NAME/sha256-timestamp"; then
        if test "$BUILD_TIMESTAMP" = "$(cat "./$RECIPE_NAME/sha256-timestamp")"; then exit 0; fi
        if sha256sum -c "./$RECIPE_NAME/sha256"; then
            echo "$BUILD_TIMESTAMP" > "./$RECIPE_NAME/sha256-timestamp"
            exit 0
        fi
    fi
}

recipe_start() {
    rm -rf "./$RECIPE_NAME"

    # Add dependencies bin folders to PATH.
    for recipe in $DEPENDENCIES; do
        export PATH="$(cd -- "${recipe%.sh}" && pwd)/bin:$PATH"
    done

    if test -n "$URL"; then
        url_filename="${URL##*/}"
        url_ext="${url_filename##*.tar}"
        url_base="${URL%/*}"
        source_dir="${url_filename%.tar*}"
        if test -n "$MIRROR"; then url_base="$MIRROR"; fi

        # Fetch and verify source.
        if ! sha256sum -c - <<end
$SHA256  $url_filename
end
        then
            if ! { curl -LO "$url_base/$url_filename" || wget "$url_base/$url_filename" || fetch "$url_base/$url_filename" ; }
            then
                rm -f "./$url_filename"
                echo "Failed to download $url_base/$url_filename"
                echo "Please fetch \"$SCRIPT_DIR/$url_filename\" manually, then press enter to continue"
                read -r answer
            fi
            sha256sum -c - <<end
$SHA256  $url_filename
end
        fi

        # Extract and enter source directory.
        rm -rf "./$source_dir"
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
        source_dir=temp
        rm -rf "./$source_dir"
        mkdir "$source_dir"
    fi
    cd "./$source_dir"
    if test "$DEVELOPMENT" = "$RECIPE_NAME"; then
        /bin/sh
        exit 137
    fi
}

recipe_finish() {
    cd ..
    rm -rf "./$source_dir"

    sha256sum "./$SCRIPT_NAME" > "$RECIPE_NAME/sha256"
    for recipe in $DEPENDENCIES; do
        sha256sum "${recipe%.sh}/sha256" >> "$RECIPE_NAME/sha256"
    done
    for file in $FILE_DEPENDENCIES; do
        sha256sum "$file" >> "$RECIPE_NAME/sha256"
    done
    echo "$BUILD_TIMESTAMP" > "./$RECIPE_NAME/sha256-timestamp"
}
