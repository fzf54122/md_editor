#include "TextStatistics.h"

namespace {
constexpr int kDefaultWordsPerMinute = 350;
}

TextStats TextStatistics::compute(const QString &text, int wordsPerMinute)
{
    TextStats stats;
    stats.characters = text.length();
    stats.lines = text.isEmpty() ? 0 : text.count('\n') + 1;
    stats.words = calculateWordCount(text);

    const int wpm = wordsPerMinute > 0 ? wordsPerMinute : kDefaultWordsPerMinute;
    stats.minutes = stats.words <= 0 ? 0 : ((stats.words - 1) / wpm) + 1;
    return stats;
}

bool TextStatistics::isCjkCharacter(const QChar &ch)
{
    const uint code = ch.unicode();
    return (code >= 0x3400 && code <= 0x4DBF)
        || (code >= 0x4E00 && code <= 0x9FFF)
        || (code >= 0x3040 && code <= 0x30FF)
        || (code >= 0xAC00 && code <= 0xD7AF);
}

bool TextStatistics::isAsciiWordChar(const QChar &ch)
{
    return ch.isLetterOrNumber() || ch == '_' || ch == '\'' || ch == '-';
}

bool TextStatistics::isMathSymbolCharacter(const QChar &ch)
{
    return ch.category() == QChar::Symbol_Math;
}

int TextStatistics::calculateWordCount(const QString &text)
{
    int count = 0;
    bool inAsciiWord = false;
    for (const QChar &ch : text) {
        if (ch.isSpace()) {
            if (inAsciiWord) {
                ++count;
                inAsciiWord = false;
            }
            continue;
        }

        if (isCjkCharacter(ch) || isMathSymbolCharacter(ch)) {
            if (inAsciiWord) {
                ++count;
                inAsciiWord = false;
            }
            ++count;
            continue;
        }

        if (isAsciiWordChar(ch)) {
            inAsciiWord = true;
            continue;
        }

        if (inAsciiWord) {
            ++count;
            inAsciiWord = false;
        }
    }

    if (inAsciiWord) {
        ++count;
    }
    return count;
}
