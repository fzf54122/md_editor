# 🦉 md-editor

<div align="center">

**一个用 Rust + Tauri 构建的高性能 WYSIWYG Markdown 编辑器原型。**

[🚀 快速开始](#-快速开始) | [🎯 产品定位](#-产品定位) | [🏗️ 架构](#️-架构) | [🛠️ 开发命令](#️-开发命令) | [🗺️ 路线图](#️-路线图)

</div>

## 🎯 产品定位

md-editor 面向写作者、开发者文档维护者和知识库用户，目标不是做 Zed / VS Code 这类代码编辑器，而是做更快、更轻、更开放的 Typora 风格 Markdown 写作工具。

核心方向：

- ✍️ **WYSIWYG Markdown**：保留并复用原有的自研 Typora 风格编辑引擎。
- ⚡ **轻量桌面壳**：使用 Rust + Tauri，避免 Qt WebEngine 的高内存占用。
- 🧩 **可调布局**：侧边栏支持横向拖拽，编辑区自适应宽屏。
- 📚 **多文件类型基础兼容**：当前支持打开 Markdown、文本、代码和常见配置文件内容。

## ✅ 当前状态

| 能力 | 状态 |
|------|------|
| Rust + Tauri 桌面壳 | 已搭建 |
| 旧 C++/Qt 项目清理 | 已完成，保留 Git 历史 |
| WYSIWYG Markdown 引擎 | 已从 Git 历史恢复到 `src/editor.html` |
| 侧边栏拖拽 | 已支持 CSS 原生横向 resize |
| Markdown 模式切换 | 已支持 Typora / GFM / CommonMark 消息通道 |
| 打开文件 | 已支持浏览器文件选择读取内容 |
| 保存文件 | 已支持 Rust 后端写入指定路径 |
| 原生文件对话框 | 暂缓，当前系统 GLib 版本过旧 |
| GitHub Actions 打包 | 已配置跨平台 release workflow |
| 官网 | 已提供 `website/` 静态官网 |

## 🚀 快速开始

安装前端依赖：

```bash
pnpm install
```

启动桌面应用：

```bash
make dev
```

只检查 Rust 后端：

```bash
make rust-check
```

完整本地检查：

```bash
make check
```

启动官网：

```bash
make website
```

## 🧱 系统依赖说明

当前开发机的 GLib / GObject 版本是 `2.58.3`。Tauri 2.x 在 Linux 上会拉起要求 `glib-2.0 >= 2.70` 的依赖，导致 `cargo check` 失败。

参考 `/home/fzf/code/rust/turbo_sync` 的桌面端配置后，本项目暂时使用 **Tauri 1.x**，该版本已在当前机器通过：

```bash
cargo check --manifest-path src-tauri/Cargo.toml
```

后续如果系统升级到 GLib 2.70+，可以重新迁移到 Tauri 2.x，并切回原生文件对话框插件。

## 🏗️ 架构

```text
md-editor
├── src/
│   ├── index.html      # Tauri 前端壳：侧栏、工具栏、编辑器 iframe
│   ├── main.js         # 文件打开/保存、快捷键、编辑器消息桥
│   ├── styles.css      # 主界面视觉系统和拖拽侧栏
│   └── editor.html     # 复用的 WYSIWYG Markdown 编辑引擎
├── src-tauri/
│   ├── src/main.rs     # Tauri 入口
│   ├── src/lib.rs      # 命令注册
│   ├── src/commands.rs # Rust 文件读写命令
│   └── tauri.conf.json # 桌面窗口与打包配置
├── website/            # 产品官网
└── .github/workflows/  # CI、release、Pages 发布
```

通信方式：

```text
Tauri WebView
  ├─ main.js 调用 Rust command: write_file / read_file
  └─ iframe editor.html 通过 postMessage 接收 setMarkdown / setMarkdownMode
```

## 🛠️ 开发命令

```bash
make help          # 查看全部命令
make install       # 安装桌面端 pnpm 依赖
make dev           # 启动 Tauri 桌面应用
make fmt           # 检查 Rust 格式
make fmt-fix       # 自动格式化 Rust
make rust-check    # cargo check
make test          # cargo test
make check         # fmt + rust-check + test
make build         # 本地只编译 release 二进制，安装包由 GitHub Actions 生成
make website       # 启动官网 dev server
make website-build # 构建官网静态产物
make clean         # 清理 Rust 构建产物
```

## 🧭 使用说明

1. 点击左侧 **打开 Markdown** 选择本地文件。
2. 编辑器会把内容发送到 `editor.html` 的 WYSIWYG 引擎。
3. 编辑后标题会出现 `*` 标记未保存状态。
4. 点击 **保存**，输入保存路径，Rust 后端会写入文件。
5. 顶部可切换 Typora / GFM / CommonMark Markdown 模式。

## 🗺️ 路线图

### MVP 稳定化

- 接入原生文件打开/保存对话框。
- 保存最近打开文件列表。
- 修复所有 WebView 控制台错误。
- 增加启动示例文档。

### 专业编辑体验

- Plain Mode 接入 Monaco Editor。
- 代码文件类型自动识别。
- 文件树与工作区目录打开。
- 全文搜索与大纲导航。

### 产品打磨

- 快捷键系统。
- 主题系统迁移。
- 自动保存与崩溃恢复。
- Linux / macOS / Windows 打包发布。

## 💎 设计原则

- 保留自研 WYSIWYG Markdown 引擎，这是产品核心竞争力。
- 桌面壳尽量薄，复杂逻辑放 Rust 后端或编辑器内核。
- 先跑通可用原型，再逐步补专业编辑器能力。
- 避免重新引入 Qt WebEngine 级别的重量级运行时。
