export HOST_ARCH="$(uname -m)"

export NO_WASM32=1
if test "$HOST_ARCH" != "x86_64"; then export NO_X86_64=1; fi
if test "$HOST_ARCH" != "aarch64"; then export NO_AARCH64=1; fi
if test "$HOST_ARCH" != "riscv64"; then export NO_RISCV64=1; fi

export NO_EFI=1
export NO_WINDOWS=1
export NO_LINUX=1
export NO_GNULINUX=1
export NO_ANDROID=1
export NO_FREEBSD=1
case "$(uname)" in
    FreeBSD)
    export NO_FREEBSD=
    export HOST_ABI=freebsd14
    ;;
    Linux)
    export NO_LINUX=
    export HOST_ABI=linux
    ;;
    *)
    exit 1
    ;;
esac
