---
mode: plan
cwd: /home/fzf/code/c++/md_editor
task: 拆分过长的 MainWindow.cpp，按模块重构 UI 与服务
complexity: medium
tool: mcp__sequential-thinking__sequentialthinking
total_thoughts: 7
created_at: 2025-12-19 11:07:16
---

# Plan: 拆分 MainWindow.cpp 模块

🎯 任务概述  
当前 `src/ui/MainWindow.cpp` 长达 1600+ 行，集中了 UI 构建、文件 IO、最近列表、统计、主题切换、Markdown 渲染桥接等多重职责，需要拆分为更小的组件与服务，使 MainWindow 仅负责界面组装和信号转发。

📋 执行计划
1. 梳理 MainWindow 的职责，按功能分块：菜单/工具栏、状态栏统计、文件打开保存与最近列表、Markdown 渲染桥接、主题/布局初始化。
2. 设计目录与类：UI 子组件放在 `src/ui/`（如 ToolbarWidget、StatsWidget、PreviewPane），非 UI 逻辑放 `src/services/` 或 `src/core/`（FileService、RecentItems、StatsService）。
3. 创建新类文件（头/源），迁移对应成员与槽函数，保持接口和父子关系，MainWindow 只组装组件并转发信号。
4. 更新 `CMakeLists.txt` 以包含新源文件/子目录，确保依赖清晰。
5. 编译或运行现有构建命令验证；检查信号/槽与对象生命周期，修复遗漏。

⚠️ 风险与注意事项
- 信号/槽连接对象父子关系必须正确，避免悬挂指针或双重删除。
- 公共状态（当前文件路径、dirty 标记、渲染配置）需集中到小型 model/state 对象，避免全局变量或重复状态。
- CMake 目标要保持原有接口与可执行名称，避免破坏现有打包/安装流程。

📎 参考
- `src/ui/MainWindow.cpp:1`
- `src/ui/MainWindow.h:1`
- `CMakeLists.txt:1`
