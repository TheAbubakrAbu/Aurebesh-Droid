#!/bin/bash
# run-dev.sh

# One-time setup:
#   chmod +x run-dev.sh
#
# To build and run Aurebesh Droid (every time after that):
#   ./run-dev.sh

pgrep AurebeshDroid && killall AurebeshDroid

echo "🧪 Building in DEBUG mode..."

BUILD_DIR=~/Downloads/aurebesh-droid-debug
SOURCE_DIR="$(cd "$(dirname "$0")"; pwd -P)"

echo "SOURCE_DIR = $SOURCE_DIR"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake -DCMAKE_BUILD_TYPE=Debug "$SOURCE_DIR"
cmake --build . --target run