#include "Theme.h"

static const char *LIGHT_THEME = R"(
QMainWindow {
    background-color: #ffffff;
}
QTextEdit, QTextBrowser {
    background-color: #ffffff;
    color: #000000;
}
QMenuBar, QMenu {
    background-color: #f5f5f5;
    color: #000000;
}
QMenu::item:selected {
    background-color: #e0e0e0;
})";

static const char *DARK_THEME = R"(
QMainWindow {
    background-color: #2b2b2b;
}
QTextEdit, QTextBrowser {
    background-color: #1e1e1e;
    color: #d4d4d4;
}
QMenuBar, QMenu {
    background-color: #2b2b2b;
    color: #ffffff;
}
QMenu::item:selected {
    background-color: #3a3a3a;
})";

static const char *SOLARIZED_THEME = R"(
QMainWindow {
    background-color: #fdf6e3;
}
QTextEdit, QTextBrowser {
    background-color: #fdf6e3;
    color: #657b83;
    selection-background-color: #eee8d5;
}
QMenuBar, QMenu {
    background-color: #eee8d5;
    color: #586e75;
}
QMenu::item:selected {
    background-color: #93a1a1;
    color: #fdf6e3;
})";

QString themeStylesheet(ThemeId id)
{
    switch (id) {
    case ThemeId::Light:
        return QString::fromUtf8(LIGHT_THEME);
    case ThemeId::Dark:
        return QString::fromUtf8(DARK_THEME);
    case ThemeId::Solarized:
        return QString::fromUtf8(SOLARIZED_THEME);
    default:
        return QString::fromUtf8(LIGHT_THEME);
    }
}
