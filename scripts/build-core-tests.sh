#!/usr/bin/env bash
# Szybka pętla developerska: konfiguracja + build + testy rdzenia na hoście.
# SPDX-License-Identifier: GPL-3.0-or-later
set -euo pipefail
cd "$(dirname "$0")/.."

cmake --preset host-debug
cmake --build --preset host-debug -j"$(nproc)"
ctest --preset host-debug --output-on-failure
