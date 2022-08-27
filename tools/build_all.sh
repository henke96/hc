# Make sure it's not run from the wrong folder.
if test "$(dirname $0)" != "."; then
    echo "Usage: ./build_all.sh"
    exit 1
fi

# Run all build.sh scripts.
find . -name "build.sh" -type f -exec {} \;
