#!/bin/bash
set -euo pipefail

BASE_DIR=$(pwd)

if [ ! -f "$BASE_DIR/conanfile.txt" ] || [ ! -f "$BASE_DIR/CMakeLists.txt" ]; then
    echo "ERROR: Couldn't locate conanfile.txt or CMakeLists.txt please run from base directory (Currently running from $BASE_DIR)"
    exit 1
fi

REBUILD=false
BUILD_TYPE="Debug"
for arg in "$@"; do
    if [ "$arg" == "--rebuild" ]; then
        REBUILD=true
    elif [ "$arg" == "--release" ]; then
        BUILD_TYPE="Release"
    fi
done

if [ -d "$BASE_DIR/build" ] && [ "$REBUILD" == false ]; then
    echo "ERROR: build directory already existing, try using --rebuild"
    exit 1
fi

if [ "$REBUILD" == true ]; then
    echo "Rebuild: Removing existing build directory"
    rm -rf build
fi

mkdir build
cd build 
mkdir conan
conan install .. --output-folder=./conan --build=missing -s build_type=$BUILD_TYPE 
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE 
echo "CMake Build finished with build type $BUILD_TYPE"