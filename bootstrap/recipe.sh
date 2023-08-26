NUM_CPUS="${NUM_CPUS:-1}"

recipe_start() {
    cd -- "$(dirname -- "$0")"
    URL_filename="${URL##*/}"
    URL_name="${URL_filename%.tar*}"
    URL_ext="${URL_filename##*.tar}"
    script_name="$(basename -- "$0")"
    recipe_name="${script_name%.sh}"

    # Check if recipe is already built.
    sha256sum -c "./$recipe_name/sha256" && exit 0

    # Build dependencies.
    for recipe in $DEPENDENCIES; do
        "$recipe"
        recipe_dir="$(cd -- "${recipe%.sh}" && pwd)"
        export PATH="$recipe_dir/bin:$PATH"
    done

    # Clean up before build.
    rm -rf "./$recipe_name"
    rm -rf "./$URL_name"

    # Fetch and verify source.
    if ! sha256sum -c - <<end
$SHA256  $URL_filename
end
    then
        if ! { wget "$URL" || fetch "$URL"; }
        then
            rm -f "./$URL_filename"
            echo "Failed to download $URL"
            echo "Please fetch \"$(pwd)/$URL_filename\" manually, then press enter to continue"
            read -r answer
        fi
        sha256sum -c - <<end
$SHA256  $URL_filename
end
    fi

    # Extract and enter source directory.
    if test "$URL_ext" = ".gz"; then
        gzip -d -c "./$URL_filename" | tar xf -
    elif test "$URL_ext" = ".xz"; then
        xz -d -c "./$URL_filename" | tar xf -
    elif test "$URL_ext" = ".bz2"; then
        bzip2 -d -c "./$URL_filename" | tar xf -
    else
        tar xf "./$URL_filename"
    fi
    cd "./$URL_name"
}

recipe_finish() {
    cd ..
    sha256sum "./$script_name" > "$recipe_name/temp"
    for recipe in $DEPENDENCIES; do
        cat "${recipe%.sh}/sha256" >> "$recipe_name/temp"
    done
    for file in $FILE_DEPENDENCIES; do
        sha256sum "$file" >> "$recipe_name/temp"
    done
    mv "$recipe_name/temp" "./$recipe_name/sha256"
    rm -rf "./$URL_name"
}