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
    for recipe in $BUILD_DEPENDENCIES; do
        "$recipe"
        recipe_dir="$(cd -- "${recipe%.sh}" && pwd)"
        export PATH="$recipe_dir/bin:$PATH"
    done

    # Clean up before build.
    rm -rf temp
    rm -rf "./$recipe_name"
    rm -rf "./$recipe_name.src"

    # Fetch and verify source.
    if ! sha256sum -c - <<end
$SHA256  $URL_filename
end
    then
        if ! { { wget -O temp "$URL" || fetch -o temp "$URL"; } && mv temp "./$URL_filename"; }
        then
            echo "Failed to download $URL"
            echo "Please fetch \"$(pwd)/$URL_filename\" manually, then press enter to continue"
            read -r answer
            rm -f temp
        fi
        sha256sum -c - <<end
$SHA256  $URL_filename
end
    fi

    # Extract and enter source directory.
    mkdir temp
    cd temp
    if test "$URL_ext" = ".gz"; then
        gzip -d -c "../$URL_filename" | tar xf -
    elif test "$URL_ext" = ".xz"; then
        xz -d -c "../$URL_filename" | tar xf -
    elif test "$URL_ext" = ".bz2"; then
        bzip2 -d -c "../$URL_filename" | tar xf -
    else
        tar xf "../$URL_filename"
    fi
    mv "./$URL_name" "../$recipe_name.src"
    cd ..
    rmdir temp
    cd "./$recipe_name.src"
}

recipe_finish() {
    cd ..
    sha256sum "./$script_name" > temp
    for recipe in $BUILD_DEPENDENCIES; do
        cat "${recipe%.sh}/sha256" >> temp
    done
    mv temp "./$recipe_name/sha256"
    rm -rf "./$recipe_name.src"
}