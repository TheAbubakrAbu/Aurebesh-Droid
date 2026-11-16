#!/usr/bin/env bash
# run.sh
#
# Builds Aurebesh Droid in Debug mode and runs it with the token from .env.
# One-time setup: chmod +x run.sh

set -euo pipefail

pkill -x AurebeshDroid 2>/dev/null || true

SOURCE_DIR="$(cd "$(dirname "$0")"; pwd -P)"
BUILD_DIR="$SOURCE_DIR/build.nosync"  # .nosync keeps the build out of iCloud
echo "Building in Debug mode: $SOURCE_DIR -> $BUILD_DIR"

mkdir -p "$BUILD_DIR"

# A build folder made for another copy of the project can't be reused.
if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
  cached_src="$(grep -E '^CMAKE_HOME_DIRECTORY:' "$BUILD_DIR/CMakeCache.txt" | sed 's/^[^=]*=//')"
  if [[ "$cached_src" != "$SOURCE_DIR" ]]; then
    echo "WARNING: the build folder was made for $cached_src, so it is being cleared"
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
  fi
fi

cd "$BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE=Debug "$SOURCE_DIR"
cmake --build . --target run
