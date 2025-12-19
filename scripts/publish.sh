#!/usr/bin/env bash
set -euo pipefail

# 加载 .env 文件
if [ -f .env ]; then
    export $(grep -v '^#' .env | xargs)
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "$REPO_ROOT"

usage() {
    cat <<'EOF'
用法：scripts/publish.sh --tag <版本> [可选参数]
...
EOF
}

BUILD_DIR="build"
RELEASE_TAG=""
RELEASE_TITLE=""
RELEASE_TARGET=""
RELEASE_NOTES=""
RELEASE_NOTES_FILE=""
RELEASE_DRAFT="true"  # 默认草稿发布

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

if ! command -v gh >/dev/null 2>&1; then
    echo "错误：缺少 GitHub CLI (gh)，请先安装 https://cli.github.com/。" >&2
    exit 1
fi

if [[ -z "${GH_TOKEN:-${GITHUB_TOKEN:-}}" ]]; then
    echo "错误：必须提供 GH_TOKEN 或 GITHUB_TOKEN 才能创建 Release。" >&2
    exit 1
fi

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

bash ./scripts/build.sh "$BUILD_DIR"

# 收集构建产物，避免子 shell 丢失 BUILD_DIR 变量
mapfile -t artifacts < <(find "$BUILD_DIR" -maxdepth 1 -type f \( -name "md-editor-*.deb" -o -name "md-editor-*.exe" \) -print | sort)

# 检查数组是否为空
if [[ ${#artifacts[@]} -eq 0 ]]; then
    echo "错误：在 $BUILD_DIR 未找到 md-editor-*.deb / md-editor-*.exe。" >&2
    exit 1
fi

title=${RELEASE_TITLE:-"Md Editor ${RELEASE_TAG}"}

# 自动获取变更日志
if [[ -z "$RELEASE_NOTES" && -z "$RELEASE_NOTES_FILE" ]]; then
    TARGET_REF="${RELEASE_TARGET:-HEAD}"
    LAST_TAG=$(git describe --tags --abbrev=0 2>/dev/null || true)
    if [[ -n "$LAST_TAG" ]]; then
        RELEASE_NOTES=$(git log --oneline "${LAST_TAG}..${TARGET_REF}")
    else
        RELEASE_NOTES=$(git log --oneline "${TARGET_REF}")
    fi
fi

gh_args=(release create "$RELEASE_TAG" "${artifacts[@]}" --title "$title" --notes "$RELEASE_NOTES")

if [[ -n "$RELEASE_TARGET" ]]; then
    gh_args+=(--target "$RELEASE_TARGET")
fi

if [[ "$RELEASE_DRAFT" == "true" ]]; then
    gh_args+=(--draft)
fi

echo "准备发布以下安装包："
printf ' - %s\n' "${artifacts[@]}"

echo "正在创建 Release ${RELEASE_TAG}..."
gh "${gh_args[@]}"

echo "Release ${RELEASE_TAG} 发布完成。"
