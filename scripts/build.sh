#!/usr/bin/env bash
set -euo pipefail

# 构建目录（默认为 build）
BUILD_DIR=${1:-build}

# 如果在 Windows 上运行，我们需要指定 MinGW 或 MSVC 的编译器
if [[ "$OSTYPE" == msys* || "$OSTYPE" == cygwin* ]]; then
    echo "在 Windows 环境中构建..."

    # 使用 CMake 配置和构建项目
    echo "配置项目并构建..."
    cmake -S . -B "${BUILD_DIR}" -G "MinGW Makefiles"  # 使用 MinGW 编译器
    cmake --build "${BUILD_DIR}"

    # 使用 NSIS 打包 Windows 安装包
    echo "尝试生成 NSIS 安装包..."
    if cpack -G NSIS; then
        echo "成功生成 NSIS 安装包！"
    else
        echo "警告：无法生成 NSIS 安装包。"
    fi

elif [[ "$OSTYPE" == linux* ]]; then
    # 如果是 Linux 系统，生成 .deb 包
    echo "生成 DEB 包..."
    cmake -S . -B "${BUILD_DIR}"
    cmake --build "${BUILD_DIR}"
    cpack -G DEB
fi

# 进入构建目录
pushd "${BUILD_DIR}" >/dev/null

# 输出生成的文件位置
echo "安装包已生成，位置：${BUILD_DIR}"

popd >/dev/null
