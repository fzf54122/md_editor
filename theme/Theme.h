#pragma once

#include <QString>

enum class ThemeId {
    Light,
    Dark,
    Solarized,
};

QString themeStylesheet(ThemeId id);
