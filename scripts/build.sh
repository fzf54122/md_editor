#!/usr/bin/env bash
set -euo pipefail

# 构建目录（默认为 build）
BUILD_DIR=${1:-build}
BUILD_TYPE=${BUILD_TYPE:-Release}

choose_generator() {
    if [[ -n "${GENERATOR:-}" ]]; then
        echo "${GENERATOR}"
        return
    fi
    if command -v ninja >/dev/null 2>&1; then
        echo "Ninja"
    elif command -v mingw32-make >/dev/null 2>&1; then
        echo "MinGW Makefiles"
    elif command -v nmake >/dev/null 2>&1; then
        echo "NMake Makefiles"
    else
        echo "Ninja"
    fi
}

if [[ "$OSTYPE" == msys* || "$OSTYPE" == cygwin* || "$OSTYPE" == win32 ]]; then
    echo "在 Windows 环境中构建..."
    GEN=$(choose_generator)
    echo "使用生成器: ${GEN}"

    cmake -S . -B "${BUILD_DIR}" -G "${GEN}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}"

    echo "尝试生成 NSIS 安装包..."
    if cpack -C "${BUILD_TYPE}"; then
        echo "成功生成 NSIS 安装包！"
    else
        echo "警告：无法生成 NSIS 安装包。"
    fi

elif [[ "$OSTYPE" == linux* ]]; then
    echo "生成 DEB 包..."
    cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}"
    cpack -G DEB -C "${BUILD_TYPE}"
fi

# 进入构建目录
pushd "${BUILD_DIR}" >/dev/null

# 输出生成的文件位置
echo "安装包已生成，位置：${BUILD_DIR}"

popd >/dev/null
