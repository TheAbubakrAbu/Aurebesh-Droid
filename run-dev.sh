#!/bin/bash
# Builds and runs in debug mode using CMake presets

echo "🧪 Building in DEBUG mode..."

BUILD_DIR=~/Downloads/aurebesh-droid-debug

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake -DCMAKE_BUILD_TYPE=Debug "$OLDPWD"
cmake --build . --target run