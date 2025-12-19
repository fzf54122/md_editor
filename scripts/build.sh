#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR=${1:-build}

cmake -S . -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}"

pushd "${BUILD_DIR}" >/dev/null

if [[ "$OSTYPE" == linux* ]]; then
    cpack -G DEB
fi

if cpack -L 2>/dev/null | grep -qi nsis; then
    cpack -G NSIS
fi

popd >/dev/null

echo "Packages are located in ${BUILD_DIR}"
