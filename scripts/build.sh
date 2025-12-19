#!/usr/bin/env bash
set -euo pipefail

# 构建目录（默认为 build）
BUILD_DIR=${1:-build}

# 使用 CMake 配置和构建项目
echo "配置项目并构建..."
cmake -S . -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}"

# 进入构建目录
pushd "${BUILD_DIR}" >/dev/null

# 检查操作系统类型并执行相应的打包操作
echo "开始生成安装包..."

# 如果是 Linux 系统，生成 .deb 包
if [[ "$OSTYPE" == linux* ]]; then
    echo "生成 DEB 包..."
    cpack -G DEB
fi

# 直接尝试生成 NSIS 包
echo "尝试生成 NSIS 安装包..."
if cpack -G NSIS; then
    echo "成功生成 NSIS 安装包！"
else
    echo "警告：无法生成 NSIS 安装包。"
fi

popd >/dev/null

echo "安装包已生成，位置：${BUILD_DIR}"