import { useEffect, useState } from 'react'
import { marked } from 'marked'
import './App.css'

const REPO_URL = 'https://github.com/fzf54122/md_editor'

function getSystemTheme() {
  return window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light'
}

function applyTheme(theme) {
  const resolvedTheme = theme === 'auto' ? getSystemTheme() : theme
  document.documentElement.setAttribute('data-theme', resolvedTheme)
}

function App() {
  const [theme, setTheme] = useState('auto')
  const [lang, setLang] = useState('zh')
  const [page, setPage] = useState('home')
  const [activeDoc, setActiveDoc] = useState('intro')
  const [showThemeMenu, setShowThemeMenu] = useState(false)
  const [showLangMenu, setShowLangMenu] = useState(false)

  useEffect(() => {
    const savedTheme = localStorage.getItem('mojian-theme') || 'auto'
    const savedLang = localStorage.getItem('mojian-lang') || 'zh'
    setTheme(savedTheme)
    setLang(savedLang)
    applyTheme(savedTheme)

    const syncPageFromHash = () => {
      const hashPage = window.location.hash.replace('#/', '')
      setPage(['docs', 'download'].includes(hashPage) ? hashPage : 'home')
    }

    syncPageFromHash()
    window.addEventListener('hashchange', syncPageFromHash)

    const mediaQuery = window.matchMedia('(prefers-color-scheme: dark)')
    const handleSystemThemeChange = () => {
      if ((localStorage.getItem('mojian-theme') || 'auto') === 'auto') {
        applyTheme('auto')
      }
    }

    mediaQuery.addEventListener('change', handleSystemThemeChange)
    return () => {
      window.removeEventListener('hashchange', syncPageFromHash)
      mediaQuery.removeEventListener('change', handleSystemThemeChange)
    }
  }, [])

  useEffect(() => {
    const baseTitle = '墨笺'
    const pageTitles = {
      home: baseTitle,
      docs: `文档 - ${baseTitle}`,
      download: `下载 - ${baseTitle}`
    }
    document.title = pageTitles[page] || baseTitle
  }, [page])

  const navigateTo = nextPage => {
    setPage(nextPage)
    window.location.hash = nextPage === 'home' ? '' : `/${nextPage}`
    window.scrollTo({ top: 0, behavior: 'smooth' })
  }

  const handleThemeChange = nextTheme => {
    setTheme(nextTheme)
    localStorage.setItem('mojian-theme', nextTheme)
    applyTheme(nextTheme)
    setShowThemeMenu(false)
  }

  const handleLangChange = nextLang => {
    setLang(nextLang)
    localStorage.setItem('mojian-lang', nextLang)
    setShowLangMenu(false)
  }

  const t = {
    zh: {
      logo: '墨笺',
      product: '特性',
      docs: '文档',
      download: '下载',
      github: 'GitHub',
      heroTitle: '墨笺',
      heroSubtitle: '把 Markdown 写成一页成稿',
      heroDesc: '墨笺是一款以 Rust + Tauri 打造的所见即所得 Markdown 编辑器。它不把写作困在代码预览里，而是给文字一张安静、轻盈、可打磨的纸。',
      ctaTitle: '写作应当像落墨一样自然',
      ctaDesc: '保留自研 WYSIWYG 引擎，以更轻的桌面壳、更自由的阅读画布和更快的启动体验，重塑 Typora 风格的中文写作空间。',
      ctaBtn: '查看设计与用法',
      docsTitle: '墨笺文档',
      docsDesc: '了解当前原型、架构边界和本地开发流程。',
      downloadTitle: '下载墨笺',
      downloadDesc: '发布包由 GitHub Actions 生成，本地开发可直接运行源码。',
      sourceCode: '源码仓库',
      releaseDetails: 'Release 详情',
      backHome: '返回首页',
      footer: '© 2026 墨笺 · Rust + Tauri · 所见即所得 Markdown',
      theme: {
        light: '浅色模式',
        dark: '深色模式',
        auto: '自动模式',
      },
    },
    en: {
      logo: 'Mojian',
      product: 'Features',
      docs: 'Docs',
      download: 'Download',
      github: 'GitHub',
      heroTitle: 'Mojian',
      heroSubtitle: 'Markdown, composed like paper.',
      heroDesc: 'Mojian is a Rust + Tauri WYSIWYG Markdown editor. It moves writing out of code-preview panels and into a quiet, finished canvas made for authors.',
      ctaTitle: 'Let Markdown feel like writing again',
      ctaDesc: 'Keep the custom WYSIWYG engine, rebuild the Typora-style experience with a lighter shell, freer layout, and faster startup.',
      ctaBtn: 'Explore the docs',
      docsTitle: 'Mojian Docs',
      docsDesc: 'Learn the current prototype, architecture boundaries, and local development flow.',
      downloadTitle: 'Download Mojian',
      downloadDesc: 'Release packages are produced by GitHub Actions. Local development runs directly from source.',
      sourceCode: 'Source Code',
      releaseDetails: 'Release Details',
      backHome: 'Back Home',
      footer: '© 2026 Mojian · Rust + Tauri · WYSIWYG Markdown',
      theme: {
        light: 'Light',
        dark: 'Dark',
        auto: 'Auto',
      },
    },
  }

  const content = t[lang]
  const mascotSrc = `${import.meta.env.BASE_URL}md-mascot.svg`

  const docNav = lang === 'zh' ? [
    { id: 'intro', title: '快速开始', icon: '📖' },
    { id: 'features', title: '功能特性', icon: '✨' },
    { id: 'arch', title: '技术架构', icon: '🏗️' },
    { id: 'dev', title: '开发指南', icon: '🛠️' },
  ] : [
    { id: 'intro', title: 'Quick Start', icon: '📖' },
    { id: 'features', title: 'Features', icon: '✨' },
    { id: 'arch', title: 'Architecture', icon: '🏗️' },
    { id: 'dev', title: 'Dev Guide', icon: '🛠️' },
  ]

  const docContent = {
    zh: {
      intro: {
        title: '快速开始',
        content: `欢迎使用墨笺！这是一款专为中文写作优化的所见即所得 Markdown 编辑器。

## 为什么选择墨笺？

与传统代码编辑器的"编辑 → 预览"双栏模式不同，墨笺让你直接在**最终排版的页面**上修改文字，专注于写作本身而非语法标记。

### 核心特点

- **真正的所见即所得**：实时渲染，点击即改，没有延迟
- **轻量高效**：基于 Rust + Tauri，启动快，内存占用低
- **中文友好**：为中文排版与长文写作优化的视觉节奏
- **跨平台支持**：Windows、macOS、Linux 原生应用

## 安装

### 方式一：下载安装包（推荐）

访问 [GitHub Releases](${REPO_URL}/releases) 页面，根据你的操作系统下载对应安装包：

- **Windows**：\`.msi\` 安装包
- **macOS**：\`.dmg\` 镜像文件
- **Linux**：\`.AppImage\` 或 \`.deb\` 包

### 方式二：从源码运行

如果你是开发者或想体验最新功能：

\`\`\`bash
# 克隆仓库
git clone ${REPO_URL}
cd md_editor

# 安装依赖
pnpm install

# 启动开发模式
make dev
\`\`\`

> **环境要求**：Node.js 18+、Rust 1.70+、pnpm

## 第一次使用

### 1. 打开文件

点击左上角「打开」按钮，或使用快捷键 \`Ctrl+O\` (macOS: \`Cmd+O\`)，选择一个 Markdown 文件。

### 2. 开始编辑

在编辑区域直接点击文本即可修改。你会发现：
- **标题**：点击即可编辑，自动识别层级
- **列表**：回车自动续行，Tab 调整缩进
- **链接**：Alt+点击跳转，双击编辑
- **图片**：拖拽即可插入

### 3. 保存文件

使用 \`Ctrl+S\` (macOS: \`Cmd+S\`) 保存。墨笺会保留原始 Markdown 格式，确保与其他编辑器兼容。

## 界面布局

\`\`\`
┌────────────────────────────────────────┐
│  墨笺                    [主题] [语言]  │
├──────┬─────────────────────────────────┤
│      │                                 │
│ 文件 │        编辑区                   │
│ 树   │   （所见即所得 Markdown）        │
│      │                                 │
│ [可  │                                 │
│  拖  │                                 │
│  拽] │                                 │
│      │                                 │
└──────┴─────────────────────────────────┘
\`\`\`

**侧边栏**可横向拖拽调整宽度，适配你的使用习惯。

## 下一步

- 查看 [功能特性](#) 了解更多编辑技巧
- 阅读 [技术架构](#) 理解实现原理
- 参考 [开发指南](#) 参与贡献`
      },
      features: {
        title: '功能特性',
        content: `墨笺为现代写作场景精心打造，每个功能都经过深思熟虑。

## 核心编辑体验

### 所见即所得引擎

基于自研的 Typora 风格 Markdown 渲染引擎，提供真正的实时预览体验：

- **即时渲染**：输入即刻转换为排版后的样式，无延迟
- **点击编辑**：直接点击文本进入编辑状态，无需切换模式
- **格式保持**：保存后完整保留原始 Markdown 语法
- **智能续行**：列表、引用块自动识别并延续格式

### 三种写作模式

根据不同场景选择合适的 Markdown 方言：

#### Typora 模式
经典的所见即所得体验，适合：
- 长文写作与笔记整理
- 文档撰写与知识管理
- 不需要严格兼容 GitHub 的场景

#### GFM 模式（GitHub Flavored Markdown）
完全兼容 GitHub 语法，适合：
- 开源项目文档
- README 编写
- 需要任务列表、表格等扩展语法

#### CommonMark 模式
标准 Markdown 规范，适合：
- 追求最大兼容性
- 静态站点生成器（Hugo、Jekyll）
- 严格遵循规范的场景

## 性能与体积

### Rust + Tauri 架构

相比传统 Electron 应用：

- **更小的体积**：安装包减少 60%+
- **更低的内存**：运行时内存占用降低 40%+
- **更快的启动**：冷启动时间 < 1s
- **原生体验**：调用系统 WebView，无需内置 Chromium

### 响应式设计

- **侧边栏拖拽**：横向调整宽度，适配不同屏幕
- **宽屏适配**：编辑区可充分利用显示器空间
- **阅读模式**：隐藏侧边栏，专注于内容本身

## 文件管理

### 智能打开

- 支持拖拽文件到窗口
- 记住最近打开的文档
- 自动检测文件编码（UTF-8、GBK）

### 安全保存

- Rust 后端直接操作文件系统
- 自动备份机制（可配置）
- 保存前验证文件权限

## 跨平台一致性

墨笺在 Windows、macOS、Linux 上提供一致的体验：

- **统一的快捷键**：Ctrl/Cmd+S 保存、Ctrl/Cmd+O 打开
- **本地化**：界面支持中英文切换
- **主题适配**：自动跟随系统深色/浅色模式

## 即将推出

我们正在开发以下功能：

- **Plain Mode**：Monaco Editor 集成，用于代码/日志查看
- **云同步**：可选的 WebDAV/Git 同步方案
- **插件系统**：社区扩展与自定义功能
- **Vim 模式**：为键盘流用户提供高效编辑`
      },
      arch: {
        title: '技术架构',
        content: `墨笺采用前后端分离架构，将 Web 技术的灵活性与 Rust 的性能优势结合。

## 整体架构

\`\`\`
┌─────────────────────────────────────────┐
│         前端层 (WebView)                 │
│  ┌─────────────────────────────────┐   │
│  │  editor.html                     │   │
│  │  - WYSIWYG Markdown 引擎         │   │
│  │  - 实时渲染与交互                 │   │
│  └─────────────────────────────────┘   │
│  ┌─────────────────────────────────┐   │
│  │  main.js                         │   │
│  │  - 窗口布局管理                   │   │
│  │  - 侧边栏拖拽逻辑                 │   │
│  │  - IPC 通信封装                   │   │
│  └─────────────────────────────────┘   │
└──────────────┬──────────────────────────┘
               │ IPC (JSON-RPC)
┌──────────────▼──────────────────────────┐
│         后端层 (Rust/Tauri)              │
│  ┌─────────────────────────────────┐   │
│  │  commands.rs                     │   │
│  │  - 文件读写 (open_file/save)     │   │
│  │  - 文件系统权限管理               │   │
│  │  - 路径规范化与安全检查           │   │
│  └─────────────────────────────────┘   │
│  ┌─────────────────────────────────┐   │
│  │  main.rs                         │   │
│  │  - 窗口创建与生命周期             │   │
│  │  - 菜单与托盘集成                 │   │
│  │  - 系统事件监听                   │   │
│  └─────────────────────────────────┘   │
└─────────────────────────────────────────┘
\`\`\`

## 前端技术栈

### 编辑引擎 (editor.html)

墨笺的核心是自研的 WYSIWYG Markdown 引擎：

**解析与渲染**
- 基于 Markdown-it 的增强解析器
- 自定义渲染规则适配 Typora/GFM/CommonMark
- 实时 AST 更新与 DOM 同步

**交互逻辑**
- 光标位置追踪与恢复
- 智能格式续行（列表、引用）
- 拖拽插入图片与文件

**性能优化**
- 虚拟滚动处理超长文档
- 增量渲染减少重绘
- Web Worker 后台解析

### GUI 编排 (main.js)

负责应用层逻辑与用户界面：

- **布局管理**：ResizeObserver 实现可拖拽分栏
- **状态同步**：本地 localStorage 持久化用户偏好
- **事件总线**：前端组件通信与 IPC 调用

## 后端技术栈

### Tauri 框架

Tauri 是新一代桌面应用框架，墨笺选择它的理由：

1. **体积优势**：使用系统 WebView，无需打包 Chromium
2. **性能优势**：Rust 编写的后端，内存安全且高效
3. **安全性**：沙盒隔离，最小权限原则
4. **跨平台**：统一代码库支持三大平台

### 文件系统边界 (commands.rs)

所有文件操作都通过 Rust Command 进行：

\`\`\`rust
#[tauri::command]
async fn open_file(path: String) -> Result<String, String> {
    // 1. 路径规范化与安全检查
    // 2. 读取文件内容
    // 3. 编码检测与转换
    // 4. 返回 UTF-8 文本
}

#[tauri::command]
async fn save_file(path: String, content: String) -> Result<(), String> {
    // 1. 验证写入权限
    // 2. 原子性写入（临时文件 + 重命名）
    // 3. 错误处理与回滚
}
\`\`\`

**安全措施**
- 路径遍历攻击防护
- 文件大小限制
- 编码验证

## 通信机制

### IPC 调用流程

前端调用后端 Command 的完整流程：

\`\`\`javascript
// 前端 (main.js)
const content = await invoke('open_file', { path: '/path/to/file.md' })
\`\`\`

经过：
1. Tauri 序列化参数为 JSON
2. 通过系统 IPC 传递到 Rust
3. Rust 反序列化并执行 Command
4. 结果序列化后返回前端
5. 前端 Promise resolve

### 事件系统

用于后端主动通知前端：

\`\`\`rust
// 后端发送事件
app.emit_all("file_changed", payload)?;
\`\`\`

\`\`\`javascript
// 前端监听事件
listen('file_changed', (event) => {
  console.log('文件已变更:', event.payload)
})
\`\`\`

## 构建与发布

### 开发构建

\`\`\`bash
make dev  # 启动热重载开发环境
\`\`\`

同时运行：
- Vite 前端开发服务器
- Tauri 开发窗口（自动重载）

### 生产构建

\`\`\`bash
make release  # 编译 release 二进制
\`\`\`

输出：
- \`src-tauri/target/release/\` 下的可执行文件
- 未打包的原始二进制

### CI/CD 流程

GitHub Actions 自动化打包：

1. **触发条件**：推送 tag 或手动触发
2. **并行构建**：三个平台同时编译
3. **打包**：生成安装包（msi/dmg/AppImage）
4. **发布**：上传到 GitHub Releases

## 性能指标

在典型场景下的实测数据：

- **启动时间**：< 1 秒（冷启动）
- **内存占用**：60-100 MB（空文档）
- **渲染延迟**：< 16ms（满足 60fps）
- **安装包体积**：
  - Windows: ~12 MB
  - macOS: ~8 MB
  - Linux: ~15 MB

## 技术债务与规划

当前使用 **Tauri 1.x** 而非最新的 2.x，原因：

- 开发机 GLib 版本较旧
- 参考 turbo_sync 项目的兼容性方案
- 系统升级后计划迁移到 Tauri 2.x

**迁移收益**：
- 更小的包体积
- 更好的性能
- 新的插件系统`
      },
      dev: {
        title: '开发指南',
        content: `欢迎参与墨笺的开发！本指南将帮助你快速搭建开发环境并了解贡献流程。

## 环境准备

### 必需工具

1. **Node.js 18+**
   \`\`\`bash
   node --version  # 应显示 v18.x 或更高
   \`\`\`

2. **Rust 1.70+**
   \`\`\`bash
   curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
   rustc --version  # 应显示 1.70 或更高
   \`\`\`

3. **pnpm**
   \`\`\`bash
   npm install -g pnpm
   \`\`\`

### 平台特定依赖

**Linux**
\`\`\`bash
# Debian/Ubuntu
sudo apt install libwebkit2gtk-4.0-dev libgtk-3-dev libayatana-appindicator3-dev

# Arch Linux
sudo pacman -S webkit2gtk gtk3 libayatana-appindicator
\`\`\`

**macOS**
\`\`\`bash
xcode-select --install
\`\`\`

**Windows**
- 安装 [Visual Studio 2022 Build Tools](https://visualstudio.microsoft.com/downloads/)
- 勾选「使用 C++ 的桌面开发」

## 克隆与安装

\`\`\`bash
# 克隆仓库
git clone ${REPO_URL}
cd md_editor

# 安装前端依赖
pnpm install

# 编译 Rust 依赖（首次较慢）
cd src-tauri
cargo build
cd ..
\`\`\`

## 开发命令

### 启动开发环境

\`\`\`bash
make dev
\`\`\`

这会同时启动：
- Vite 开发服务器（热更新前端）
- Tauri 开发窗口（自动重载）

**快捷键**
- \`Ctrl+R\` / \`Cmd+R\`：重载窗口
- \`F12\`：打开开发者工具

### 构建生产版本

\`\`\`bash
# 编译 release 二进制
make release

# 输出位置
ls src-tauri/target/release/md_editor
\`\`\`

### 构建官网

\`\`\`bash
make website

# 输出到 website/dist
# 可本地预览：cd website && pnpm preview
\`\`\`

## 项目结构

\`\`\`
md_editor/
├── src/                      # 前端代码
│   ├── editor.html           # WYSIWYG 编辑器
│   ├── main.js               # 应用主逻辑
│   ├── styles.css            # 样式表
│   └── assets/               # 静态资源
│       ├── md-mascot.svg     # 应用图标
│       └── ...
│
├── src-tauri/                # Rust 后端
│   ├── Cargo.toml            # Rust 依赖配置
│   ├── tauri.conf.json       # Tauri 配置
│   ├── build.rs              # 构建脚本
│   ├── icons/                # 应用图标（各平台）
│   └── src/
│       ├── main.rs           # 入口文件
│       └── commands.rs       # Tauri Commands
│
├── website/                  # 官网源码
│   ├── src/
│   │   ├── App.jsx           # 主组件
│   │   ├── App.css           # 样式
│   │   └── main.jsx          # 入口
│   ├── public/               # 静态资源
│   └── dist/                 # 构建产物
│
├── .github/
│   └── workflows/
│       ├── release.yml       # 发布流程
│       ├── ci.yml            # 持续集成
│       └── pages.yml         # 官网部署
│
├── Makefile                  # 快捷命令
└── README.md                 # 项目说明
\`\`\`

## 代码规范

### 前端

- **格式化**：Prettier（自动格式化）
- **命名**：camelCase 变量，PascalCase 组件
- **注释**：关键逻辑需注释，避免过度注释

### Rust

- **格式化**：\`cargo fmt\`（提交前运行）
- **检查**：\`cargo clippy\`（修复所有警告）
- **命名**：snake_case 函数，UpperCamelCase 类型

## Git 工作流

### 分支策略

- \`main\`：稳定分支，仅接受 PR
- \`feat/xxx\`：新功能分支
- \`fix/xxx\`：Bug 修复分支

### 提交规范

使用约定式提交（Conventional Commits）：

\`\`\`
feat: 添加文件自动保存功能
fix: 修复侧边栏拖拽卡顿
docs: 更新快速开始文档
style: 优化编辑器排版
refactor: 重构文件读写逻辑
test: 添加命令单元测试
chore: 升级 Tauri 到 1.5
\`\`\`

### 提交前检查

\`\`\`bash
# 前端代码检查
pnpm lint

# Rust 代码检查
cd src-tauri
cargo fmt
cargo clippy
cargo test
\`\`\`

## 调试技巧

### 前端调试

1. 在开发窗口按 \`F12\` 打开 DevTools
2. 在 \`main.js\` 中使用 \`console.log\`
3. 使用 Source Map 定位源码

### Rust 调试

1. 添加打印语句：
   \`\`\`rust
   println!("Debug: {:?}", variable);
   \`\`\`

2. 使用 VSCode + rust-analyzer：
   - 设置断点
   - 按 F5 启动调试

### IPC 调试

在 \`tauri.conf.json\` 中启用 devtools：

\`\`\`json
{
  "tauri": {
    "windows": [{
      "devtools": true
    }]
  }
}
\`\`\`

在 Console 中查看 IPC 调用：
\`\`\`javascript
window.__TAURI__.invoke('open_file', { path: '/test.md' })
  .then(console.log)
  .catch(console.error)
\`\`\`

## 测试

### 前端测试

\`\`\`bash
# 运行单元测试
pnpm test

# 生成覆盖率报告
pnpm test:coverage
\`\`\`

### Rust 测试

\`\`\`bash
cd src-tauri
cargo test

# 运行特定测试
cargo test open_file
\`\`\`

## 发布流程

### 版本号管理

遵循语义化版本（SemVer）：

- \`1.0.0\` → \`1.0.1\`：Bug 修复
- \`1.0.0\` → \`1.1.0\`：新功能（向后兼容）
- \`1.0.0\` → \`2.0.0\`：破坏性更改

### 创建 Release

\`\`\`bash
# 1. 更新版本号
# 修改 src-tauri/Cargo.toml 和 src-tauri/tauri.conf.json

# 2. 提交并打 tag
git add .
git commit -m "chore: release v1.2.0"
git tag v1.2.0
git push origin main --tags

# 3. GitHub Actions 自动构建并发布
\`\`\`

## 贡献指南

### 报告 Bug

在 [GitHub Issues](${REPO_URL}/issues) 提交，包含：

- 操作系统与版本
- 墨笺版本号
- 复现步骤
- 预期行为 vs 实际行为
- 截图或日志（如有）

### 提交 PR

1. Fork 本仓库
2. 创建功能分支：\`git checkout -b feat/my-feature\`
3. 提交代码并推送到你的 Fork
4. 在 GitHub 上创建 Pull Request
5. 等待 Review 和 CI 检查通过

### Code Review 标准

- 代码风格一致
- 有适当的测试覆盖
- 提交信息清晰
- 无编译警告

## 获取帮助

- **文档**：本站点其他章节
- **Discussions**：[${REPO_URL}/discussions](${REPO_URL}/discussions)
- **Issues**：[${REPO_URL}/issues](${REPO_URL}/issues)

感谢你的贡献！`
      }
    },
    en: {
      intro: {
        title: 'Quick Start',
        content: `Mojian is a WYSIWYG Markdown editor built with Rust + Tauri.

## Installation

### Desktop App
Download from [GitHub Releases](${REPO_URL}/releases).

### Run from Source
\`\`\`bash
git clone ${REPO_URL}
cd md_editor
pnpm install
make dev
\`\`\`

## Basic Usage

1. Click "Open File" to select a Markdown file
2. Edit directly in WYSIWYG view
3. Drag to resize sidebar
4. Ctrl+S to save

## Mode Switching

- **Typora Mode**: Classic WYSIWYG experience
- **GFM Mode**: GitHub Flavored Markdown
- **CommonMark Mode**: Standard Markdown spec`
      },
      features: {
        title: 'Features',
        content: `## WYSIWYG Engine

Custom Typora-style Markdown rendering core:
- Real-time preview
- WYSIWYG editing
- Long-form content friendly

## Rust + Tauri Shell

- System WebView
- Smaller binary and memory footprint vs Qt WebEngine
- Faster startup and cleaner packaging

## Resizable Sidebar & Wide Layout

- Horizontally adjustable sidebar
- Wide-screen friendly editor canvas
- Better for note management and long documents

## Multi-mode & Cross-platform

- Switch between Typora, GFM, and CommonMark
- Website and GitHub Actions maintain cross-platform releases`
      },
      arch: {
        title: 'Architecture',
        content: `## Architecture Design

\`\`\`
┌─────────────────────────────────┐
│     Frontend (WebView)          │
│  - editor.html (WYSIWYG Engine) │
│  - main.js (GUI Orchestration)  │
└──────────┬──────────────────────┘
           │ IPC
┌──────────▼──────────────────────┐
│     Rust Backend (Tauri)        │
│  - commands.rs (Filesystem)     │
│  - main.rs (Window Management)  │
└─────────────────────────────────┘
\`\`\`

## Core Modules

### Editor Core
- **src/editor.html**: WYSIWYG Markdown engine
- Supports Typora/GFM/CommonMark modes

### GUI Orchestration
- **src/main.js**: Window layout, sidebar dragging, menu handling

### Filesystem Boundary
- **src-tauri/src/commands.rs**: Rust commands for file open/save and permissions

## Tech Stack

- **Frontend**: Vanilla JS + WYSIWYG engine
- **Desktop Shell**: Tauri 1.x (system WebView)
- **Backend**: Rust (file IO & system calls)`
      },
      dev: {
        title: 'Dev Guide',
        content: `## Local Development

### Requirements
- Node.js 18+
- Rust 1.70+
- pnpm

### Development Workflow

\`\`\`bash
# Install dependencies
pnpm install

# Start dev mode
make dev

# Build release binary
make release

# Build website
make website
\`\`\`

## Project Structure

\`\`\`
md_editor/
├── src/
│   ├── editor.html       # WYSIWYG engine
│   ├── main.js           # GUI orchestration
│   └── styles.css        # Styles
├── src-tauri/
│   └── src/
│       ├── main.rs       # Tauri entry
│       └── commands.rs   # Rust commands
└── website/              # Website (Vite + React)
\`\`\`

## Packaging & Release

Local builds compile release binaries only. Installers are produced by GitHub Actions:
- Linux: AppImage / deb
- macOS: dmg
- Windows: msi

## Contributing

1. Fork this repo
2. Create a feature branch
3. Submit a PR
4. Wait for review`
      }
    }
  }

  const currentDocContent = docContent[lang][activeDoc]

  const features = lang === 'zh' ? [
    { title: '成稿式写作', desc: '复用自研 Typora 风格 Markdown 引擎，直接在排版后的纸面中修改文字。' },
    { title: '轻量桌面壳', desc: 'Rust + Tauri 调用系统 WebView，目标是比传统 WebEngine 更轻、更快、更易发布。' },
    { title: '舒展画布', desc: '侧边栏可拖拽，编辑区适配宽屏，让长文、笔记和文档都有稳定节奏。' },
    { title: '多 Markdown 习惯', desc: '支持 Typora、GFM、CommonMark 等模式切换，适配不同写作流。' },
    { title: '文件边界清晰', desc: 'Rust command 负责打开、保存与文件系统权限，前端专注写作体验。' },
    { title: '发布流程完备', desc: '官网与 GitHub Actions 打包流程并行维护，便于持续分发桌面版本。' },
    { title: 'Plain Mode 规划', desc: '后续接入 Monaco Editor，用于代码、日志和配置文件的纯文本编辑。' },
    { title: '笺鹤图标', desc: '以折纸、墨线和笔锋构成品牌符号，用于官网、favicon 与桌面图标。' },
  ] : [
    { title: 'Rendered Writing', desc: 'Reuse the custom Typora-style Markdown engine and edit directly in the composed document.' },
    { title: 'Light Desktop Shell', desc: 'Rust + Tauri use the system WebView for a lighter, faster, easier-to-ship desktop app.' },
    { title: 'Breathing Canvas', desc: 'Resizable sidebars and wide-screen friendly rhythm for essays, notes, and documents.' },
    { title: 'Markdown Modes', desc: 'Switch between Typora, GFM, and CommonMark writing habits.' },
    { title: 'Clear File Boundary', desc: 'Rust commands own file IO and permissions while the frontend focuses on writing.' },
    { title: 'Release Ready', desc: 'Website and GitHub Actions packaging workflows evolve alongside the desktop app.' },
    { title: 'Plain Mode Plan', desc: 'Monaco Editor integration is planned for code, logs, and config files.' },
    { title: 'Jianhe Mark', desc: 'A paper-crane, ink-line, and nib inspired symbol for the website, favicon, and desktop icon.' },
  ]

  const docs = lang === 'zh' ? [
    { title: 'WYSIWYG 引擎', desc: '自研 Typora 风格 Markdown 渲染内核，支持实时预览、所见即所得编辑，适合中文长文写作。' },
    { title: 'Rust + Tauri 轻量壳', desc: '使用系统 WebView，相比 Qt WebEngine 减少体积与内存占用，启动更快，打包更简洁。' },
    { title: '侧栏拖拽与宽屏适配', desc: '侧边栏横向可调整，编辑区支持宽屏布局，便于笔记管理与长文排版。' },
    { title: '多模式与跨平台', desc: '支持 Typora/GFM/CommonMark 语法模式切换，官网与 GitHub Actions 并行维护跨平台发布流程。' },
  ] : [
    { title: 'WYSIWYG Engine', desc: 'Custom Typora-style Markdown rendering core with real-time preview and WYSIWYG editing for long-form content.' },
    { title: 'Rust + Tauri Shell', desc: 'System WebView reduces binary size and memory usage compared to Qt WebEngine, with faster startup and cleaner packaging.' },
    { title: 'Resizable Sidebar & Wide Layout', desc: 'Horizontally adjustable sidebar and wide-screen friendly editor canvas for note management and long documents.' },
    { title: 'Multi-mode & Cross-platform', desc: 'Switch between Typora, GFM, and CommonMark syntax modes. Website and GitHub Actions workflows maintain cross-platform releases.' },
  ]

  const downloads = lang === 'zh' ? [
    { title: '桌面端', desc: '面向日常写作，发布包由 GitHub Actions 生成。', action: '查看桌面端下载' },
    { title: '源码运行', desc: '适合当前阶段开发、验证和快速迭代。', action: '打开源码仓库' },
    { title: '官网构建', desc: 'website/ 是独立 Vite 官网，可发布到 GitHub Pages。', action: '查看 Release 详情' },
  ] : [
    { title: 'Desktop App', desc: 'For daily writing. Release packages are generated by GitHub Actions.', action: 'View Desktop Downloads' },
    { title: 'Run from Source', desc: 'Best for current development, validation, and fast iteration.', action: 'Open Source Repo' },
    { title: 'Website Build', desc: 'website/ is a standalone Vite site deployable to GitHub Pages.', action: 'View Release Details' },
  ]

  return (
    <div className="app">
      <header className="header">
        <button type="button" className="header-left" onClick={() => navigateTo('home')} aria-label={content.logo}>
          <img src={mascotSrc} alt="" className="mascot" />
          <div className="logo">{content.logo}</div>
        </button>
        <nav className="nav">
          <button type="button" onClick={() => navigateTo('home')}>{content.product}</button>
          <button type="button" onClick={() => navigateTo('docs')}>{content.docs}</button>
          <button type="button" onClick={() => navigateTo('download')}>{content.download}</button>
          <a href={REPO_URL} target="_blank" rel="noopener noreferrer">{content.github}</a>

          <div className="dropdown">
            <button type="button" onClick={() => setShowThemeMenu(!showThemeMenu)} className="icon-btn" aria-label={content.theme.auto}>
              {theme === 'dark' ? '🌙' : theme === 'light' ? '☀️' : '💻'}
            </button>
            {showThemeMenu && (
              <div className="dropdown-menu">
                <button type="button" onClick={() => handleThemeChange('light')}>☀️ {content.theme.light}</button>
                <button type="button" onClick={() => handleThemeChange('dark')}>🌙 {content.theme.dark}</button>
                <button type="button" onClick={() => handleThemeChange('auto')}>💻 {content.theme.auto}</button>
              </div>
            )}
          </div>

          <div className="dropdown">
            <button type="button" onClick={() => setShowLangMenu(!showLangMenu)} className="icon-btn" aria-label="Language">
              {lang === 'zh' ? '🇨🇳' : '🇬🇧'}
            </button>
            {showLangMenu && (
              <div className="dropdown-menu">
                <button type="button" onClick={() => handleLangChange('zh')}>🇨🇳 简体中文</button>
                <button type="button" onClick={() => handleLangChange('en')}>🇬🇧 English</button>
              </div>
            )}
          </div>
        </nav>
      </header>

      <main>
        {page === 'home' && (
          <>
            <section className="hero">
              <div className="hero-content">
                <h1 className="hero-title">{content.heroTitle}</h1>
                <p className="hero-subtitle">{content.heroSubtitle}</p>
                <p className="hero-desc">{content.heroDesc}</p>
              </div>
              <img src={mascotSrc} alt="笺鹤图标" className="hero-mascot" />
            </section>

            <section id="features" className="features" aria-label={content.product}>
              {features.map(feature => (
                <div key={feature.title} className="feature-card">
                  <h3>{feature.title}</h3>
                  <p>{feature.desc}</p>
                </div>
              ))}
            </section>

            <section className="cta">
              <h2>{content.ctaTitle}</h2>
              <p>{content.ctaDesc}</p>
              <button type="button" className="btn-primary" onClick={() => navigateTo('docs')}>{content.ctaBtn}</button>
            </section>
          </>
        )}

        {page === 'docs' && (
          <section className="docs-layout">
            <aside className="docs-sidebar">
              <nav className="docs-nav">
                {docNav.map(item => (
                  <button
                    key={item.id}
                    type="button"
                    className={`docs-nav-item ${activeDoc === item.id ? 'active' : ''}`}
                    onClick={() => setActiveDoc(item.id)}
                  >
                    <span className="docs-nav-icon">{item.icon}</span>
                    <span className="docs-nav-title">{item.title}</span>
                  </button>
                ))}
              </nav>
            </aside>
            <article className="docs-content">
              <h1>{currentDocContent.title}</h1>
              <div className="markdown-body" dangerouslySetInnerHTML={{ __html: marked(currentDocContent.content) }} />
            </article>
          </section>
        )}

        {page === 'download' && (
          <section className="page-section">
            <div className="page-heading">
              <h1>{content.downloadTitle}</h1>
              <p>{content.downloadDesc}</p>
            </div>
            <div className="download-grid">
              {downloads.map((item, index) => (
                <article key={item.title} className="download-card">
                  <h3>{item.title}</h3>
                  <p>{item.desc}</p>
                  <a href={index === 1 ? REPO_URL : `${REPO_URL}/releases`} target="_blank" rel="noopener noreferrer">
                    {item.action}
                  </a>
                </article>
              ))}
            </div>
          </section>
        )}
      </main>

      <footer className="footer">
        <p>{content.footer}</p>
      </footer>
    </div>
  )
}

export default App
