#!/bin/bash
# run-release.sh

# Builds and runs in release mode using CMake presets
# To build and run Aurebesh Droid:
#   chmod +x run-dev.sh run-release.sh
#   ./run-dev.sh      # builds and runs in Debug mode
#   ./run-release.sh  # builds and runs in Release mode

echo "🚀 Building in RELEASE mode..."

BUILD_DIR=~/Downloads/aurebesh-droid-release

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake -DCMAKE_BUILD_TYPE=Release "$OLDPWD"
cmake --build . --target run