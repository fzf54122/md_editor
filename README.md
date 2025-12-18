# Md Editor · Elegant Typora Experience

Md Editor 致力于复刻 Typora 的沉浸式写作体验：一边输入、一边渲染，所见即所得。项目基于 Qt6 Widgets + WebEngine 构建，配合多主题皮肤、Notebook 侧栏、Plain Mode、智能统计等特性，让 Markdown 与纯文本都能在统一的优雅界面中创作。

---

## ✨ Highlights

| 维度 | 亮点 |
| --- | --- |
| 写作体验 | Typora 风格的实时渲染、标题/列表/代码块的语义保持 |
| 双栏 Notebook | “文/纲”双视图 + 自定义折叠按钮，快速切换文稿与大纲 |
| 多主题 + Plain Mode | 六套内置主题 + 自动识别 `.txt/.py/.cpp` 等文件并开启等宽卡片式 Plain Mode |
| 富文本工具 | 菜单/快捷键覆盖加粗、斜体、链接、代码、引用、列表等常用操作 |
| 文件管理 | 新建/打开/保存/最近/删除/刷新/脏标记/关闭前提示 |
| 状态统计 | 行、词、字符、分钟四种视图，菜单项也会实时展示 “显示字符: 1033字符” 等信息 |
| 持久化偏好 | QSettings 记录主题、最近文件夹、Plain Mode 状态，重启即恢复 |

---

## 🗂️ Project Layout

| Path | Description |
| --- | --- |
| `src/ui/MainWindow.cpp/.h` | 主窗口、侧栏、菜单、文件管理、统计、Plain Mode 切换等核心逻辑 |
| `src/theme/Theme.cpp` | Light / Dark / Solarized / Elegant Noir / Forest Mist / Sakura Breeze 六套 QSS |
| `src/resources/editor.html` | Web 编辑器：Markdown ↔ HTML、命令桥接、Plain Mode 样式 |
| `plan/` | 阶段计划与执行记录，确保 “Plan ⇄ Git ⇄ README” 一致 |
| `TESTING.md` | 手动测试用例、回归建议 |

---

## 🚀 Build & Run

**Requirements**

- Qt 6（需启用 Widgets & WebEngineWidgets）
- CMake ≥ 3.20
- C++17 Toolchain

**Commands**

```bash
cmake -S . -B build
cmake --build build
./build/md_editor
```

首次启动会提示选择文稿或 Notebook 目录；也可直接打开 `plan/` 了解当前迭代目标。

---

## 📅 Progress Snapshot

| Phase | Goal | Status |
| --- | --- | --- |
| 1. Baseline | 确认依赖/渲染方案/目录结构 | ✅ |
| 2. Core UI | Typora 式三栏、状态栏、菜单 | ✅ |
| 3. Markdown Render | 节流 + WebEngine 渲染 + Bridge | ✅ |
| 4. Status Sync | 滚动联动、行/词/字符/分钟统计 | ✅ |
| 5. File Ops | 历史记录、脏标记、保存策略 | ✅ |
| 6. Rich Commands | 格式菜单、快捷键、工具按钮 | ✅ |
| 7. Themes & Prefs | 六套主题 + QSettings 存储 | ✅ |
| 8. Notebook+Outline | 双栈侧栏、折叠、聚焦模式 | ✅ |
| 9. Plain Mode | 自动识别普通文本、等宽视图 | ✅ |

详细里程碑、风险和 TODO 请查阅 `plan/2025-12-18_18-19-39-ui-polish.md` 等文档。

---

## 🧪 Testing

- `TESTING.md` 列出手动验证流程（编辑、文件、主题、大纲、Plain Mode 等）。
- 推荐逐步把高频用例迁至 Qt Test/GUI Automation，避免后续重构破坏体验。

---

## 🔮 What’s Next

1. **Syntax Flair**：为 Plain Mode 提供行号/语法高亮插件。
2. **Automation**：将 `TESTING.md` 中的用例迁移到自动化框架。
3. **Plugin Hooks**：开放简单 API 支持自定义主题与命令扩展。

---

## 🤝 Contributing

- PR 前请先阅读 `plan/` 中最新计划，保持节奏一致。
- Commit 建议采用 `feat: phase-<n>-<summary>` 并引用 plan。
- UI/文档改动需同步 README + 测试清单。

---

**Md Editor** 既保留了 Typora 的优雅，又拥抱了纯文本的极简。欢迎提出创意或 PR，一起打造更好的写作空间。✨
