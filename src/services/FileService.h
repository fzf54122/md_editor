#pragma once

#include <QString>

// 文件读写与类型判断工具。
class FileService
{
public:
    // 读取 UTF-8/文本文件内容，成功返回 true。
    static bool readTextFile(const QString &fileName, QString &outContent);

    // 写入文本内容，成功返回 true。
    static bool writeTextFile(const QString &fileName, const QString &content);

    // 判断是否为 Markdown 扩展名。
    static bool isMarkdownFile(const QString &path);

    // 基于扩展名返回语言提示（用于 plain mode 语法高亮），未知则返回空串。
    static QString languageHintForPath(const QString &path);
};
