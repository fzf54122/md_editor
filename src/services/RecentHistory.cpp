#include "RecentHistory.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>

namespace {
constexpr int kDefaultMaxRecentEntries = 5;
const char *kRecentFilesKey = "recentFiles";
const char *kRecentFoldersKey = "recentFolders";
}

RecentHistory::RecentHistory(int maxEntries)
    : maxEntries(maxEntries <= 0 ? kDefaultMaxRecentEntries : maxEntries)
{
}

QString RecentHistory::normalizePath(const QString &path) const
{
    QFileInfo info(path);
    QString canonical = info.canonicalFilePath();
    if (!canonical.isEmpty())
        return QDir::cleanPath(canonical);
    return QDir::cleanPath(info.absoluteFilePath());
}

bool RecentHistory::pruneHistoryList(QStringList &list, bool requireDirectory) const
{
    bool modified = false;
    for (int i = list.size() - 1; i >= 0; --i) {
        QFileInfo info(list.at(i));
        if (!info.exists() || (requireDirectory ? !info.isDir() : !info.isFile())) {
            list.removeAt(i);
            modified = true;
            continue;
        }
        const QString canonical = info.canonicalFilePath();
        const QString normalized = canonical.isEmpty() ? info.absoluteFilePath() : canonical;
        if (list.at(i) != normalized) {
            list[i] = normalized;
            modified = true;
        }
    }

    while (list.size() > maxEntries) {
        list.removeLast();
        modified = true;
    }
    return modified;
}

void RecentHistory::saveList(const QStringList &list, const char *key) const
{
    QSettings settings("md-editor", "app");
    settings.setValue(key, list);
}

void RecentHistory::load()
{
    QSettings settings("md-editor", "app");
    recentFiles = settings.value(kRecentFilesKey).toStringList();
    recentFolders = settings.value(kRecentFoldersKey).toStringList();

    const bool filesChanged = pruneHistoryList(recentFiles, false);
    const bool foldersChanged = pruneHistoryList(recentFolders, true);

    if (filesChanged)
        saveList(recentFiles, kRecentFilesKey);
    if (foldersChanged)
        saveList(recentFolders, kRecentFoldersKey);
}

void RecentHistory::rememberPath(QStringList &list, const QString &path, bool requireDirectory)
{
    const QString normalized = normalizePath(path);
    if (normalized.isEmpty())
        return;

    QFileInfo info(normalized);
    if (!info.exists() || (requireDirectory ? !info.isDir() : !info.isFile()))
        return;

    list.removeAll(normalized);
    list.prepend(normalized);
    while (list.size() > maxEntries) {
        list.removeLast();
    }
}

void RecentHistory::rememberFile(const QString &path)
{
    rememberPath(recentFiles, path, false);
    saveList(recentFiles, kRecentFilesKey);
}

void RecentHistory::rememberFolder(const QString &path)
{
    rememberPath(recentFolders, path, true);
    saveList(recentFolders, kRecentFoldersKey);
}

void RecentHistory::removeEntry(const QString &path, bool isFolder)
{
    const QString normalized = normalizePath(path);
    if (normalized.isEmpty())
        return;

    QStringList &list = isFolder ? recentFolders : recentFiles;
    const char *key = isFolder ? kRecentFoldersKey : kRecentFilesKey;

    if (list.removeAll(normalized) > 0) {
        saveList(list, key);
    }
}

bool RecentHistory::findLatestExisting(QString &filePathOut, QString &folderPathOut) const
{
    for (const QString &filePath : recentFiles) {
        QFileInfo info(filePath);
        if (!info.exists() || !info.isFile())
            continue;
        filePathOut = info.absoluteFilePath();
        return true;
    }

    for (const QString &folderPath : recentFolders) {
        QDir dir(folderPath);
        if (!dir.exists())
            continue;
        folderPathOut = dir.absolutePath();
        return true;
    }

    return false;
}
