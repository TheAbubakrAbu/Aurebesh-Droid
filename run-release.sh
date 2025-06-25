#!/bin/bash
# Builds and runs in release mode using CMake presets

echo "🚀 Building in RELEASE mode..."

BUILD_DIR=~/Downloads/aurebesh-droid-release

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake -DCMAKE_BUILD_TYPE=Release "$OLDPWD"
cmake --build . --target run