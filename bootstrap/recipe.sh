NUM_CPUS="${NUM_CPUS:-1}"

recipe_start() {
    cd -- "$(dirname -- "$0")"
    url_filename="${URL##*/}"
    url_name="${url_filename%.tar*}"
    url_ext="${url_filename##*.tar}"
    url_base="${URL%/*}"
    if test -n "$MIRROR"; then url_base="$MIRROR"; fi
    script_name="$(basename -- "$0")"
    recipe_name="${script_name%.sh}"

    # Run dependencies recipes.
    for recipe in $DEPENDENCIES; do
        "$recipe"
    done

    # Check if recipe is already built.
    sha256sum -c "./$recipe_name/sha256" && exit 0

    # Clean up before build.
    rm -rf "./$recipe_name"
    rm -rf "./$url_name"

    # Add dependencies bin folders to PATH.
    for recipe in $DEPENDENCIES; do
        export PATH="$(cd -- "${recipe%.sh}" && pwd)/bin:$PATH"
    done

    # Fetch and verify source.
    if ! sha256sum -c - <<end
$SHA256  $url_filename
end
    then
        if ! { curl -LO "$url_base/$url_filename" || wget "$url_base/$url_filename" || fetch "$url_base/$url_filename" ; }
        then
            rm -f "./$url_filename"
            echo "Failed to download $url_base/$url_filename"
            echo "Please fetch \"$(pwd)/$url_filename\" manually, then press enter to continue"
            read -r answer
        fi
        sha256sum -c - <<end
$SHA256  $url_filename
end
    fi

    # Extract and enter source directory.
    if test "$url_ext" = ".gz"; then
        gzip -d -c "./$url_filename" | tar xf -
    elif test "$url_ext" = ".xz"; then
        xz -d -c "./$url_filename" | tar xf -
    elif test "$url_ext" = ".bz2"; then
        bzip2 -d -c "./$url_filename" | tar xf -
    else
        tar xf "./$url_filename"
    fi
    cd "./$url_name"
    if test -n "$DEVELOPMENT"; then /bin/sh; fi
}

recipe_finish() {
    cd ..
    sha256sum "./$script_name" > "$recipe_name/temp"
    for recipe in $DEPENDENCIES; do
        sha256sum "${recipe%.sh}/sha256" >> "$recipe_name/temp"
    done
    for file in $FILE_DEPENDENCIES; do
        sha256sum "$file" >> "$recipe_name/temp"
    done
    mv "$recipe_name/temp" "./$recipe_name/sha256"
    if test -z "$DEVELOPMENT"; then rm -rf "./$url_name"; fi
}