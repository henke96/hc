fetch() {
    set +e
    test -f "$2"
    if test $? -ne 0; then
        wget -O temp "$1$2" && mv temp "$2"
        if test $? -ne 0; then
            echo "Failed to download $1$2"
            echo "Please fetch $(pwd)/$2 manually, then type OK to continue"
            read answer
            test "$answer" = "OK" || exit 1
        fi
    fi
    set -e
}

extract_and_enter() {
    set +e
    test -d "$1"
    if test $? -ne 0; then
        set -e
        mkdir temp
        cd temp
        if test "$2" = ".tar.gz"; then
            gzip -d -c "../$1$2" | tar xf - && mv "$1" "../$1"
        elif test "$2" = ".tar.xz"; then
            xz -d -c "../$1$2" | tar xf - && mv "$1" "../$1"
        elif test "$2" = ".tar.bz2"; then
            bzip2 -d -c "../$1$2" | tar xf - && mv "$1" "../$1"
        else
            tar xf "../$1$2" && mv "$1" "../$1"
        fi
        cd ..
        rmdir temp
    fi
    set -e
    cd "$1"
}

verify_checksums() {
    set +e
    sha256sum -c "$1"
    if test $? -ne 0; then
        echo "Failed to verify checksums"
        echo "Please verify checksums in $(pwd)/$1 manually, then type OK to continue"
        read answer
        test "$answer" = "OK" || exit 1
    fi
    set -e
}
