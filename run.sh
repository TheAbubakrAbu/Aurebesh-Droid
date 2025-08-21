#!/bin/bash
# run.sh

# One-time setup:
#   chmod +x run.sh
#
# To build and run Aurebesh Droid (every time after that):
#   ./run.sh

set -euo pipefail

pkill -x AurebeshDroid 2>/dev/null || true

echo "🧪 Building in DEBUG mode..."

SOURCE_DIR="$(cd "$(dirname "$0")"; pwd -P)"
BUILD_DIR="$SOURCE_DIR/build.nosync"

echo "SOURCE_DIR = $SOURCE_DIR"
echo "BUILD_DIR  = $BUILD_DIR"

mkdir -p "$BUILD_DIR"

if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
  cached_src="$(grep -E '^CMAKE_HOME_DIRECTORY:' "$BUILD_DIR/CMakeCache.txt" | sed 's/^[^=]*=//')"
  if [[ "$cached_src" != "$SOURCE_DIR" ]]; then
    echo "⚠️  Cache points to a different source dir:"
    echo "    $cached_src"
    echo "    → Clearing $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
  fi
fi

cd "$BUILD_DIR"

cmake -DCMAKE_BUILD_TYPE=Debug "$SOURCE_DIR"
cmake --build . --target run