#include "FileService.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QHash>

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
        "md", "markdown", "mdown", "mkd", "mdtext", "mdtxt",
        "mkdown", "mdwn", "mdx", "rmd", "qmd"
    };
    return markdownExt.contains(ext);
}

QString FileService::languageHintForPath(const QString &path)
{
    if (path.isEmpty())
        return QString();

    const QString ext = QFileInfo(path).suffix().toLower();
    static const QHash<QString, QString> mapping = {
        { "py", "python" },
        { "c", "c" },
        { "h", "c" },
        { "cpp", "cpp" },
        { "cc", "cpp" },
        { "cxx", "cpp" },
        { "hpp", "cpp" },
        { "go", "go" },
        { "java", "java" },
        { "js", "javascript" },
        { "ts", "typescript" },
        { "tsx", "tsx" },
        { "sql", "sql" },
        { "json", "json" },
        { "yml", "yaml" },
        { "yaml", "yaml" },
        { "toml", "toml" },
        { "ini", "ini" },
        { "sh", "shell" },
        { "bash", "shell" },
        { "zsh", "shell" },
        { "rs", "rust" },
        { "cs", "csharp" }
    };

    return mapping.value(ext);
}
