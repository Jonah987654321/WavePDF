#!/bin/bash
set -euo pipefail

BASE_DIR=$(pwd)

if [ ! -f "$BASE_DIR/conanfile.txt" ] || [ ! -f "$BASE_DIR/CMakeLists.txt" ]; then
    echo "ERROR: Couldn't locate conanfile.txt or CMakeLists.txt please run from base directory (Currently running from $BASE_DIR)"
    exit 1
fi

echo "Switching to build directory && starting make"
cd build
make
ctest --verbose
echo "Starting executable..."
echo "############################"
./WavePDF
