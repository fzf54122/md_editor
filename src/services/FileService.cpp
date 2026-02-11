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

    const QFileInfo info(path);
    const QString ext = info.suffix().toLower();
    const QString fileName = info.fileName().toLower();

    if (fileName == QStringLiteral("dockerfile"))
        return QStringLiteral("dockerfile");
    if (fileName == QStringLiteral("makefile"))
        return QStringLiteral("makefile");
    if (fileName == QStringLiteral("cmakelists.txt"))
        return QStringLiteral("cmake");
    if (fileName == QStringLiteral("requirements.txt"))
        return QStringLiteral("text");
    if (fileName == QStringLiteral(".gitignore") || fileName == QStringLiteral(".dockerignore"))
        return QStringLiteral("text");

    static const QHash<QString, QString> mapping = {
        { "py", "python" },
        { "pyi", "python" },
        { "pyx", "python" },
        { "c", "c" },
        { "h", "c" },
        { "cpp", "cpp" },
        { "cc", "cpp" },
        { "cxx", "cpp" },
        { "hpp", "cpp" },
        { "hh", "cpp" },
        { "hxx", "cpp" },
        { "go", "go" },
        { "java", "java" },
        { "js", "javascript" },
        { "mjs", "javascript" },
        { "cjs", "javascript" },
        { "jsx", "javascript" },
        { "ts", "typescript" },
        { "tsx", "tsx" },
        { "sql", "sql" },
        { "json", "json" },
        { "jsonc", "json" },
        { "json5", "json" },
        { "yml", "yaml" },
        { "yaml", "yaml" },
        { "toml", "toml" },
        { "ini", "ini" },
        { "cfg", "ini" },
        { "conf", "ini" },
        { "sh", "shell" },
        { "bash", "shell" },
        { "zsh", "shell" },
        { "fish", "shell" },
        { "ps1", "powershell" },
        { "rs", "rust" },
        { "cs", "csharp" },
        { "kt", "kotlin" },
        { "kts", "kotlin" },
        { "swift", "swift" },
        { "php", "php" },
        { "rb", "ruby" },
        { "lua", "lua" },
        { "md", "markdown" },
        { "markdown", "markdown" },
        { "rst", "rst" },
        { "xml", "xml" },
        { "html", "html" },
        { "htm", "html" },
        { "css", "css" },
        { "scss", "css" },
        { "less", "css" },
        { "vue", "vue" },
        { "txt", "text" },
        { "log", "text" }
    };

    return mapping.value(ext);
}
