#pragma once

#include <QString>

class QWidget;
class QTreeWidget;
class QTreeWidgetItem;

class RecentHistory;

// 管理笔记本文件树相关的 UI 填充、路径规范化与删除操作。
class NotebookManager
{
public:
    enum class DeleteResult {
        Cancelled,
        NotFound,
        Failed,
        Deleted
    };

    static void refreshTree(QTreeWidget *tree, const QString &rootPath);
    static void populateFileTree(QTreeWidgetItem *parentItem, const QString &path);
    static bool selectPath(QTreeWidget *tree, const QString &path);
    static QString normalizePath(const QString &path);

    static DeleteResult deleteFile(QWidget *parent, const QString &path, RecentHistory &history, QString *normalizedOut = nullptr);
    static DeleteResult deleteFolder(QWidget *parent, const QString &path, RecentHistory &history, QString *normalizedOut = nullptr);
};
