#pragma once

#include <QAction>

struct MenuActions
{
    QAction *newAction{};
    QAction *openAction{};
    QAction *openFolderAction{};
    QAction *saveAction{};
    QAction *saveAsAction{};
    QAction *exitAction{};

    QAction *undoAction{};
    QAction *redoAction{};
    QAction *cutAction{};
    QAction *copyAction{};
    QAction *pasteAction{};
    QAction *selectAllAction{};

    QAction *boldAction{};
    QAction *italicAction{};
    QAction *codeAction{};
    QAction *codeBlockAction{};
    QAction *linkAction{};
    QAction *unorderedListAction{};

    QAction *heading1Action{};
    QAction *heading2Action{};
    QAction *heading3Action{};
    QAction *blockquoteAction{};

    QAction *focusModeAction{};
    QAction *outlineViewAction{};

    QAction *lightThemeAction{};
    QAction *darkThemeAction{};
    QAction *solarizedThemeAction{};
    QAction *elegantThemeAction{};
    QAction *forestThemeAction{};
    QAction *sakuraThemeAction{};

    QAction *shortcutsAction{};
    QAction *aboutAction{};
};

class QMainWindow;

// 负责创建菜单栏与各项 QAction，返回统一的动作集合供宿主连接信号。
class MenuBuilder
{
public:
    static MenuActions build(QMainWindow *owner);
};
