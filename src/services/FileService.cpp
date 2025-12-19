#include "FileService.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

bool FileService::readTextFile(const QString &fileName, QString &outContent)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    outContent = in.readAll();
    return true;
}

bool FileService::writeTextFile(const QString &fileName, const QString &content)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << content;
    return true;
}

bool FileService::isMarkdownFile(const QString &path)
{
    if (path.isEmpty())
        return true;

    const QString ext = QFileInfo(path).suffix().toLower();
    static const QStringList markdownExt = {
        "md", "markdown", "mdown", "mkd", "mdtext", "mdtxt"
    };
    return markdownExt.contains(ext);
}
