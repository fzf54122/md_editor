#include "NotebookManager.h"

#include <QAbstractItemView>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "RecentHistory.h"

void NotebookManager::refreshTree(QTreeWidget *tree, const QString &rootPath)
{
    if (!tree)
        return;

    const QSignalBlocker blocker(tree);
    tree->clear();

    auto addPlaceholder = [tree](const QString &text) {
        QTreeWidgetItem *placeholder = new QTreeWidgetItem(tree, QStringList(text));
        placeholder->setFlags(Qt::NoItemFlags);
    };

    if (rootPath.isEmpty()) {
        addPlaceholder("暂无内容");
        return;
    }

    QDir dir(rootPath);
    if (!dir.exists()) {
        addPlaceholder("路径不存在");
        return;
    }

    QString rootLabel = QFileInfo(dir.absolutePath()).fileName();
    if (rootLabel.isEmpty())
        rootLabel = dir.absolutePath();

    QTreeWidgetItem *rootItem = new QTreeWidgetItem(tree, QStringList(rootLabel));
    rootItem->setData(0, Qt::UserRole, dir.absolutePath());
    rootItem->setData(0, Qt::UserRole + 1, true);
    populateFileTree(rootItem, dir.absolutePath());
    rootItem->setExpanded(true);
    tree->setCurrentItem(rootItem);
}

void NotebookManager::populateFileTree(QTreeWidgetItem *parentItem, const QString &path)
{
    if (!parentItem)
        return;

    QDir dir(path);
    if (!dir.exists())
        return;

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst | QDir::Name);
    const QFileInfoList entries = dir.entryInfoList();
    for (const QFileInfo &info : entries) {
        QTreeWidgetItem *item = new QTreeWidgetItem(parentItem, QStringList(info.fileName()));
        item->setData(0, Qt::UserRole, info.absoluteFilePath());
        item->setData(0, Qt::UserRole + 1, info.isDir());
        if (info.isDir()) {
            populateFileTree(item, info.absoluteFilePath());
        }
    }
}

bool NotebookManager::selectPath(QTreeWidget *tree, const QString &path)
{
    if (!tree || path.isEmpty())
        return false;

    const QString normalizedTarget = normalizePath(path);
    if (normalizedTarget.isEmpty())
        return false;

    QList<QTreeWidgetItem *> stack;
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        stack.append(tree->topLevelItem(i));
    }

    QTreeWidgetItem *found = nullptr;
    while (!stack.isEmpty()) {
        QTreeWidgetItem *item = stack.takeLast();
        const QString itemPath = normalizePath(item->data(0, Qt::UserRole).toString());
        if (itemPath == normalizedTarget) {
            found = item;
            break;
        }
        for (int i = 0; i < item->childCount(); ++i) {
            stack.append(item->child(i));
        }
    }

    if (!found)
        return false;

    QTreeWidgetItem *parent = found->parent();
    while (parent) {
        parent->setExpanded(true);
        parent = parent->parent();
    }

    tree->setCurrentItem(found);
    tree->scrollToItem(found, QAbstractItemView::PositionAtCenter);
    return true;
}

QString NotebookManager::normalizePath(const QString &path)
{
    QFileInfo info(path);
    QString canonical = info.canonicalFilePath();
    if (!canonical.isEmpty())
        return QDir::cleanPath(canonical);
    return QDir::cleanPath(info.absoluteFilePath());
}

NotebookManager::DeleteResult NotebookManager::deleteFile(QWidget *parent, const QString &path, RecentHistory &history, QString *normalizedOut)
{
    if (path.isEmpty())
        return DeleteResult::Cancelled;

    const QString normalized = normalizePath(path);
    if (normalizedOut)
        *normalizedOut = normalized;

    const QString prompt = QString("确定要删除文件吗？\n%1\n此操作无法撤销。").arg(normalized);
    const QMessageBox::StandardButton ret = QMessageBox::warning(
        parent,
        "删除文件",
        prompt,
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel
    );
    if (ret != QMessageBox::Yes)
        return DeleteResult::Cancelled;

    QFile file(normalized);
    if (!file.exists()) {
        QMessageBox::information(parent, "提示", "文件不存在，可能已被移动。");
        return DeleteResult::NotFound;
    }

    if (!file.remove()) {
        QMessageBox::warning(parent, "删除失败", "无法删除该文件，请检查权限。");
        return DeleteResult::Failed;
    }

    history.removeEntry(normalized, false);
    return DeleteResult::Deleted;
}

NotebookManager::DeleteResult NotebookManager::deleteFolder(QWidget *parent, const QString &path, RecentHistory &history, QString *normalizedOut)
{
    if (path.isEmpty())
        return DeleteResult::Cancelled;

    const QString normalized = normalizePath(path);
    if (normalizedOut)
        *normalizedOut = normalized;

    QDir dir(normalized);
    if (!dir.exists()) {
        QMessageBox::information(parent, "提示", "目标文件夹不存在，可能已被移动。");
        return DeleteResult::NotFound;
    }

    const QString prompt = QString("确定要删除整个文件夹吗？\n%1\n此操作会永久删除其中所有内容。").arg(normalized);
    const QMessageBox::StandardButton ret = QMessageBox::warning(
        parent,
        "删除文件夹",
        prompt,
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel
    );
    if (ret != QMessageBox::Yes)
        return DeleteResult::Cancelled;

    if (!dir.removeRecursively()) {
        QMessageBox::warning(parent, "删除失败", "无法删除该文件夹，请检查权限或是否被占用。");
        return DeleteResult::Failed;
    }

    history.removeEntry(normalized, true);
    return DeleteResult::Deleted;
}
