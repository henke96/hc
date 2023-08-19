fetch() {
    set +e
    test -f "$2"
    if test $? -ne 0; then
        wget -nv -nc -O temp "$1$2" && mv temp "$2"
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
    mkdir temp
    decrypt_opt=""
    if test "$2" = ".tar.gz"; then
        decrypt_opt="-z"
    elif test "$2" = ".tar.xz"; then
        decrypt_opt="-J"
    fi
    set +e
    test -d "$1"
    if test $? -ne 0; then
        tar x -C temp -k "$decrypt_opt" -f "$1$2" && mv "temp/$1" "$1"
        if test $? -ne 0; then
            echo "Failed to extract $1$2"
            echo "Please extract $(pwd)/$1 manually, then type OK to continue"
            read answer
            test "$answer" = "OK" || exit 1
            rm -rf "temp/$1"
        fi
    fi
    set -e
    rmdir temp
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
