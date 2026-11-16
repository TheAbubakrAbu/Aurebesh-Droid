#!/usr/bin/env bash
# start-worker.sh
#
# Downloads the newest build and starts the bot. The server's systemd service
# runs this. One-time setup: chmod +x start-worker.sh

set -euo pipefail

OWNER="TheAbubakrAbu"
REPO="Aurebesh-Droid"
# build.yml uploads every build to this fixed tag. Download from the tag
# directly rather than "latest", so a hand-made release can't hide the binary.
TAG="nightly"
ASSET_NAME="LinuxBinary"
BIN="./LinuxBinary"

URL="https://github.com/$OWNER/$REPO/releases/download/$TAG/$ASSET_NAME"

# Download to a temp file first so a failed download never clobbers the
# binary from the last good start.
TMP="$BIN.download"
echo "Downloading: $URL"
if curl -fsSL --retry 3 --retry-delay 2 -o "$TMP" "$URL"; then
  chmod +x "$TMP"
  mv -f "$TMP" "$BIN"
else
  rm -f "$TMP"
  if [[ -x "$BIN" ]]; then
    echo "WARNING: download failed, starting the existing $BIN instead"
  else
    echo "ERROR: download failed and there is no existing $BIN to fall back on"
    exit 1
  fi
fi

# The fonts are compiled into the binary, so nothing else needs copying.
exec "$BIN"
