#!/usr/bin/env bash
# 发布 Md Editor 安装包到 GitHub Releases
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "$REPO_ROOT"

usage() {
    cat <<'EOF'
用法：scripts/publish.sh --tag <版本> [可选参数]

参数说明：
  --tag <版本>         Release / Tag 名称（如 v1.2.0），必填
  --title <标题>       自定义 Release 标题，默认 "Md Editor <tag>"
  --target <ref>       Release 对应的 git ref（默认当前 HEAD）
  --notes <文本>       Release Notes 文本（与 --notes-file 互斥）
  --notes-file <路径>  从文件读取 Release Notes
  --build-dir <目录>   指定构建目录，默认 build
  --draft              以 Draft 形式发布
  -h, --help           显示本帮助

环境要求：
  需提前设置 GH_TOKEN 或 GITHUB_TOKEN，并安装 GitHub CLI（gh）。

脚本流程：
  1. 调用 scripts/build.sh 构建 & 打包（Linux 生成 DEB，检测到 NSIS 时生成 EXE）。
  2. 使用 gh release create 创建/更新 Release，并上传全部 md-editor*.deb/.exe。
EOF
}

BUILD_DIR="build"
RELEASE_TAG=""
RELEASE_TITLE=""
RELEASE_TARGET=""
RELEASE_NOTES=""
RELEASE_NOTES_FILE=""
RELEASE_DRAFT="false"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --tag)
            RELEASE_TAG=$2
            shift 2
            ;;
        --title)
            RELEASE_TITLE=$2
            shift 2
            ;;
        --target)
            RELEASE_TARGET=$2
            shift 2
            ;;
        --notes)
            RELEASE_NOTES=$2
            shift 2
            ;;
        --notes-file)
            RELEASE_NOTES_FILE=$2
            shift 2
            ;;
        --build-dir)
            BUILD_DIR=$2
            shift 2
            ;;
        --draft)
            RELEASE_DRAFT="true"
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage >&2
            exit 1
            ;;
    esac
done

if [[ -z "$RELEASE_TAG" ]]; then
    echo "Error: --tag is required." >&2
    usage >&2
    exit 1
fi

if [[ -n "$RELEASE_NOTES" && -n "$RELEASE_NOTES_FILE" ]]; then
    echo "Error: --notes and --notes-file are mutually exclusive." >&2
    exit 1
fi

if ! command -v gh >/dev/null 2>&1; then
    echo "错误：缺少 GitHub CLI (gh)，请先安装 https://cli.github.com/。" >&2
    exit 1
fi

if [[ -z "${GH_TOKEN:-${GITHUB_TOKEN:-}}" ]]; then
    echo "错误：必须提供 GH_TOKEN 或 GITHUB_TOKEN 才能创建 Release。" >&2
    exit 1
fi

# Ensure build dir exists after running build.sh
mkdir -p "$BUILD_DIR"

./scripts/build.sh "$BUILD_DIR"

mapfile -t artifacts < <(find "$BUILD_DIR" -maxdepth 1 -type f \( -name 'md-editor-*.deb' -o -name 'md-editor-*.exe' \) | sort)

if [[ ${#artifacts[@]} -eq 0 ]]; then
    echo "错误：在 $BUILD_DIR 未找到 md-editor-*.deb / md-editor-*.exe。" >&2
    exit 1
fi

title=${RELEASE_TITLE:-"Md Editor ${RELEASE_TAG}"}

gh_args=(release create "$RELEASE_TAG" "${artifacts[@]}" --title "$title")

if [[ -n "$RELEASE_TARGET" ]]; then
    gh_args+=(--target "$RELEASE_TARGET")
fi

if [[ "$RELEASE_DRAFT" == "true" ]]; then
    gh_args+=(--draft)
fi

if [[ -n "$RELEASE_NOTES_FILE" ]]; then
    gh_args+=(--notes-file "$RELEASE_NOTES_FILE")
elif [[ -n "$RELEASE_NOTES" ]]; then
    gh_args+=(--notes "$RELEASE_NOTES")
else
    gh_args+=(--notes "Automated release for ${RELEASE_TAG}")
fi

echo "准备发布以下安装包："
printf ' - %s\n' "${artifacts[@]}"

echo "正在创建 Release ${RELEASE_TAG}..."
gh "${gh_args[@]}"

echo "Release ${RELEASE_TAG} 发布完成。"
