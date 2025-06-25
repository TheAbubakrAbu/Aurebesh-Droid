#!/bin/bash
# Builds and runs in debug mode using CMake presets
# To build and run Aurebesh Droid:
#   chmod +x run-dev.sh run-release.sh
#   ./run-dev.sh      # builds and runs in Debug mode
#   ./run-release.sh  # builds and runs in Release mode

echo "🧪 Building in DEBUG mode..."

BUILD_DIR=~/Downloads/aurebesh-droid-debug

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake -DCMAKE_BUILD_TYPE=Debug "$OLDPWD"
cmake --build . --target run