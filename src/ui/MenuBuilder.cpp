#include "MenuBuilder.h"

#include <QActionGroup>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>

MenuActions MenuBuilder::build(QMainWindow *owner)
{
    MenuActions actions;
    QMenuBar *menuBar = owner->menuBar();

    QMenu *fileMenu = menuBar->addMenu("文件");
    actions.newAction = fileMenu->addAction("新建");
    actions.newAction->setShortcut(QKeySequence::New);
    actions.openAction = fileMenu->addAction("打开");
    actions.openAction->setShortcut(QKeySequence::Open);
    actions.openFolderAction = fileMenu->addAction("打开文件夹");
    actions.openFolderAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
    actions.saveAction = fileMenu->addAction("保存");
    actions.saveAction->setShortcut(QKeySequence::Save);
    actions.saveAsAction = fileMenu->addAction("另存为");
    actions.saveAsAction->setShortcut(QKeySequence::SaveAs);
    actions.exitAction = fileMenu->addAction("退出");

    QMenu *editMenu = menuBar->addMenu("编辑");
    actions.undoAction = editMenu->addAction("撤销");
    actions.undoAction->setShortcut(QKeySequence::Undo);
    actions.redoAction = editMenu->addAction("重做");
    actions.redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addSeparator();
    actions.cutAction = editMenu->addAction("剪切");
    actions.cutAction->setShortcut(QKeySequence::Cut);
    actions.copyAction = editMenu->addAction("复制");
    actions.copyAction->setShortcut(QKeySequence::Copy);
    actions.pasteAction = editMenu->addAction("粘贴");
    actions.pasteAction->setShortcut(QKeySequence::Paste);
    actions.selectAllAction = editMenu->addAction("全选");
    actions.selectAllAction->setShortcut(QKeySequence::SelectAll);

    QMenu *paragraphMenu = menuBar->addMenu("段落");
    actions.heading1Action = paragraphMenu->addAction("标题 H1");
    actions.heading2Action = paragraphMenu->addAction("标题 H2");
    actions.heading3Action = paragraphMenu->addAction("标题 H3");
    paragraphMenu->addSeparator();
    actions.blockquoteAction = paragraphMenu->addAction("引用");

    QMenu *formatMenu = menuBar->addMenu("格式");
    actions.boldAction = formatMenu->addAction("加粗");
    actions.boldAction->setShortcut(QKeySequence::Bold);

    actions.italicAction = formatMenu->addAction("斜体");
    actions.italicAction->setShortcut(QKeySequence::Italic);

    actions.codeAction = formatMenu->addAction("行内代码");
    actions.codeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_QuoteLeft));

    actions.codeBlockAction = formatMenu->addAction("代码块");
    actions.codeBlockAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_QuoteLeft));

    actions.linkAction = formatMenu->addAction("插入链接");
    actions.linkAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_K));

    actions.unorderedListAction = formatMenu->addAction("无序列表");
    actions.unorderedListAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));

    QMenu *viewMenu = menuBar->addMenu("视图");
    actions.focusModeAction = viewMenu->addAction("聚焦模式");
    actions.focusModeAction->setCheckable(true);
    actions.outlineViewAction = viewMenu->addAction("显示大纲");

    QMenu *themeMenu = menuBar->addMenu("主题");
    QActionGroup *themeGroup = new QActionGroup(owner);
    themeGroup->setExclusive(true);
    actions.lightThemeAction = themeMenu->addAction("晨光浅色");
    actions.darkThemeAction = themeMenu->addAction("墨夜深色");
    actions.solarizedThemeAction = themeMenu->addAction("暮光暖日");
    actions.elegantThemeAction = themeMenu->addAction("优雅黑金");
    actions.forestThemeAction = themeMenu->addAction("雾森青黛");
    actions.sakuraThemeAction = themeMenu->addAction("樱云粉雾");
    for (QAction *action : {actions.lightThemeAction, actions.darkThemeAction, actions.solarizedThemeAction, actions.elegantThemeAction, actions.forestThemeAction, actions.sakuraThemeAction}) {
        action->setCheckable(true);
        action->setActionGroup(themeGroup);
    }

    QMenu *helpMenu = menuBar->addMenu("帮助");
    actions.shortcutsAction = helpMenu->addAction("快捷键参考");
    actions.aboutAction = helpMenu->addAction("关于");

    return actions;
}
