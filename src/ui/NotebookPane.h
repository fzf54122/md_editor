#pragma once

#include <QWidget>

class QButtonGroup;
class QStackedWidget;
class QToolButton;
class QTreeWidget;
class QTreeWidgetItem;
class QListWidget;
class QListWidgetItem;
class QLabel;

// NotebookPane 负责侧栏 UI（文件树、大纲、新建按钮），并通过信号将事件转发给宿主。
class NotebookPane : public QWidget
{
    Q_OBJECT

public:
    explicit NotebookPane(QWidget *parent = nullptr);

    QTreeWidget *fileTreeWidget() const { return fileTree; }
    QListWidget *outlineListWidget() const { return outlineList; }
    QLabel *activeDocLabelWidget() const { return activeDocLabel; }
    void setActiveDocumentLabel(const QString &text);
    void showOutlineTab();
    void showNotebookTab();
    void setCollapsed(bool collapsed);
    void setCurrentTab(int index);

signals:
    void fileItemActivated(QTreeWidgetItem *item);
    void fileContextMenuRequested(const QPoint &pos);
    void newFileRequested();
    void outlineItemActivated(QListWidgetItem *item);

private:
    QButtonGroup *sideTabGroup{};
    QStackedWidget *sideStack{};
    QToolButton *notebookTabBtn{};
    QToolButton *outlineTabBtn{};
    QTreeWidget *fileTree{};
    QListWidget *outlineList{};
    QLabel *activeDocLabel{};
};
