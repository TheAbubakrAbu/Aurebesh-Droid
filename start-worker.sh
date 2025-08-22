#!/usr/bin/env bash
# start-worker.sh

# One-time setup:
#   chmod +x start-worker.sh

set -euo pipefail

OWNER="TheAbubakrAbu"
REPO="Aurebesh-Droid"
ASSET_NAME="LinuxBinary"
BIN="./LinuxBinary"

API="https://api.github.com/repos/$OWNER/$REPO/releases/latest"

AUTH_HEADER=()
if [[ -n "${GH_TOKEN:-}" ]]; then
  AUTH_HEADER=(-H "Authorization: Bearer $GH_TOKEN")
fi

# Extract the browser_download_url for the exact asset name, without jq
JSON="$(curl -fsSL "${AUTH_HEADER[@]}" "$API")"
URL="$(printf '%s\n' "$JSON" \
  | sed -nE "s/.*\"browser_download_url\" *: *\"([^\"]*\/$ASSET_NAME)\".*/\1/p" \
  | head -n1)"

if [[ -z "${URL:-}" ]]; then
  echo "ERROR: Could not find asset named '$ASSET_NAME' in latest release of $OWNER/$REPO"
  exit 1
fi

echo "Downloading: $URL"
curl -fL --retry 3 --retry-delay 2 "${AUTH_HEADER[@]}" -o "$BIN" "$URL"
chmod +x "$BIN"

# Fonts next to the binary for exe_dir()/assets/fonts
mkdir -p assets/fonts
cp -r src/assets/fonts/* assets/fonts/ 2>/dev/null || true

exec "$BIN"