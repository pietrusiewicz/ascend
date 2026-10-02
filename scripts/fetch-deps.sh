#!/usr/bin/env bash
# Pobiera zależności rdzenia (nlohmann/json, googletest) do lokalnego katalogu,
# żeby build działał bez sieci: cmake ... -DASC_DEPS_DIR=<katalog>.
# SPDX-License-Identifier: GPL-3.0-or-later
set -euo pipefail

JSON_VERSION="${JSON_VERSION:-3.12.0}"
JSON_SHA256="4b92eb0c06d10683f7447ce9406cb97cd4b453be18d7279320f7b2f025c10187"
GTEST_VERSION="${GTEST_VERSION:-1.18.0}"
GTEST_SHA256="6e3191c1455468b3fc35a417fb565c1c5071aee1b7e7f85e30cf48a98d37d8b5"

DEPS_DIR="${1:-$PWD/.deps}"
mkdir -p "$DEPS_DIR"

fetch() {
    local name="$1" url="$2" expected="$3"
    local archive="$DEPS_DIR/$name.tar.gz"
    echo ">> $name"
    curl -fL --retry 3 -o "$archive" "$url"
    echo "$expected  $archive" | sha256sum -c -
    tar -xzf "$archive" -C "$DEPS_DIR"
    local extracted
    extracted=$(tar -tzf "$archive" | head -1 | cut -d/ -f1)
    rm -rf "$DEPS_DIR/$name"
    mv "$DEPS_DIR/$extracted" "$DEPS_DIR/$name"
    rm -f "$archive"
}

fetch json "https://github.com/nlohmann/json/archive/refs/tags/v${JSON_VERSION}.tar.gz" "$JSON_SHA256"
fetch googletest "https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.tar.gz" "$GTEST_SHA256"

echo "Gotowe: $DEPS_DIR/{json,googletest}"
