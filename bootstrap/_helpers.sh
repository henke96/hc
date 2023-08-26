fetch_file() {
    if ! test -f "$2"; then
        if ! { { wget -O temp "$1$2" || fetch -o temp "$1$2"; } && mv temp "$2"; }
        then
            echo "Failed to download $1$2"
            echo "Please fetch $(pwd)/$2 manually, then type OK to continue"
            read -r answer
            test "$answer" = "OK"
            rm -f temp
        fi
    fi
}

extract_and_enter() {
    name="${1%.tar*}"
    ext="${1##*.tar}"
    if ! test -d "$name"; then
        mkdir temp
        cd temp
        if test "$ext" = ".gz"; then
            gzip -d -c "../$1" | tar xf - && mv "$name" "../$name"
        elif test "$ext" = ".xz"; then
            xz -d -c "../$1" | tar xf - && mv "$name" "../$name"
        elif test "$ext" = ".bz2"; then
            bzip2 -d -c "../$1" | tar xf - && mv "$name" "../$name"
        else
            tar xf "../$1" && mv "$name" "../$name"
        fi
        cd ..
        rmdir temp
    fi
    cd "$name"
}

verify_checksums() {
    if ! sha256sum -c - <<end
$1
end
    then
        echo "Failed to verify checksums"
        echo "Please verify the following checksums manually, then type OK to continue"
        echo "$1"
        read -r answer
        test "$answer" = "OK"
    fi
}
