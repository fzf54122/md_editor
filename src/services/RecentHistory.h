#pragma once

#include <QStringList>

// 管理最近打开的文件/文件夹历史，负责读取、去重、裁剪并持久化到 QSettings。
class RecentHistory
{
public:
    explicit RecentHistory(int maxEntries = 5);

    void load();
    void rememberFile(const QString &path);
    void rememberFolder(const QString &path);
    void removeEntry(const QString &path, bool isFolder);
    bool findLatestExisting(QString &filePathOut, QString &folderPathOut) const;

    const QStringList &files() const { return recentFiles; }
    const QStringList &folders() const { return recentFolders; }

private:
    QString normalizePath(const QString &path) const;
    bool pruneHistoryList(QStringList &list, bool requireDirectory) const;
    void rememberPath(QStringList &list, const QString &path, bool requireDirectory);
    void saveList(const QStringList &list, const char *key) const;

    int maxEntries;
    QStringList recentFiles;
    QStringList recentFolders;
};
