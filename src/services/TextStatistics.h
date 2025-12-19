#pragma once

#include <QString>

struct TextStats
{
    int lines{0};
    int words{0};
    int characters{0};
    int minutes{0};
};

// 负责计算文本的行数、词数、字符数与阅读时间（分钟）。
class TextStatistics
{
public:
    static TextStats compute(const QString &text, int wordsPerMinute = 350);

private:
    static bool isCjkCharacter(const QChar &ch);
    static bool isAsciiWordChar(const QChar &ch);
    static bool isMathSymbolCharacter(const QChar &ch);
    static int calculateWordCount(const QString &text);
};
