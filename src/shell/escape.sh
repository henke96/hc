# Credit to mtraceur :)
escape() {
    printf \'
    unescaped=$1
    while :; do
        case $unescaped in
        *\'*)
            printf %s "${unescaped%%\'*}'\''"
            unescaped=${unescaped#*\'}
            ;;
        *)
            printf %s "$unescaped"
            break
        esac
    done
    printf \'
}
