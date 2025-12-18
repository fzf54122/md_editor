#pragma once

#include <QString>

enum class ThemeId {
    Light,
    Dark,
    Solarized,
    Elegant,
    Forest,
    Sakura,
};

struct ThemePalette {
    QString bodyBackground;
    QString pageBackground;
    QString textColor;
    QString mutedTextColor;
    QString accentColor;
    QString borderColor;
    QString inlineCodeBackground;
    QString inlineCodeColor;
    QString codeBlockBackground;
    QString codeBlockText;
    QString blockquoteBackground;
    QString blockquoteBorder;
    QString blockquoteText;
    QString selectionBackground;
    QString tableHeaderBackground;
    QString tableHeaderText;
    QString linkUnderlineColor;
};

QString themeStylesheet(ThemeId id);
ThemePalette themePalette(ThemeId id);
