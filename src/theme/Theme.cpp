#include "Theme.h"

static const char *CLASSIC_LIGHT_THEME = R"(
QMainWindow {
    background-color: #f6f7fb;
    color: #2b2d3c;
}
QWidget {
    font-family: "Source Han Sans", "PingFang SC", "Microsoft YaHei", sans-serif;
}
QMenuBar {
    background-color: #ffffff;
    border-bottom: 1px solid #e2e6f0;
}
QMenuBar::item {
    padding: 6px 12px;
    margin: 0 4px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #e8ebf5;
    color: #1f2232;
}
QMenu {
    background-color: #ffffff;
    border: 1px solid #dfe3ef;
    color: #1f2232;
}
QMenu::item {
    padding: 6px 18px;
    border-radius: 6px;
}
QMenu::item:selected {
    background: #f1f4ff;
    color: #1a1d2b;
}
QStatusBar {
    background: #ffffff;
    border-top: 1px solid #e0e3ef;
    color: #4b4f66;
}
QToolButton#StatsButton {
    background: #ffffff;
    border: 1px solid #dfe3ef;
    border-radius: 14px;
    padding: 4px 12px;
    color: #4b4f66;
}
QToolButton#StatsButton:pressed {
    background: #eff2fb;
}
#StatsPanelTitle {
    color: #616685;
}
#StatsValueLabel {
    color: #1f2235;
}
#StatsUnitLabel {
    color: #7b7f95;
}
#SideContainer {
    background: #f6f7fb;
    border-right: 1px solid #e1e5f2;
}
#SideStack {
    background: transparent;
}
#MarkdownWebView {
    background: #f6f7fb;
    border: none;
}
#SideRail {
    background: #f0f2fa;
    border-right: 1px solid #e1e5f2;
}
#SideRailButton {
    border: none;
    border-left: 3px solid transparent;
    background: transparent;
    color: #7a7b87;
    font-weight: 600;
}
#SideRailButton:checked {
    color: #2c3150;
    border-left-color: #4b6cff;
    background: #e5ecff;
}
#CollapseButton {
    border: none;
    background: transparent;
    color: #7a7b87;
    font-size: 12px;
}
#CollapseButton:checked {
    color: #2c2d3c;
}
#NotebookFilter {
    background: #ffffff;
    border: 1px solid #dfe3ef;
    border-radius: 8px;
    padding: 6px 10px;
}
#SideLabel {
    font-size: 11px;
    font-weight: 600;
    text-transform: uppercase;
    color: #7a7b87;
    letter-spacing: 1px;
}
#NewFileButton {
    background: #eef2ff;
    border: 1px solid #d2d9ff;
    color: #4b55aa;
    border-radius: 8px;
}
#NewFileButton:hover {
    background: #ffffff;
}
#ActiveDocLabel {
    color: #6c7086;
    font-weight: 600;
    background: transparent;
}
QTreeWidget, QListWidget {
    background: transparent;
    color: #2c3043;
}
QTreeWidget::item:selected, QListWidget::item:selected {
    background: #e5ecff;
    color: #20253a;
}
QPushButton {
    background: #4b6cff;
    border-radius: 8px;
    color: #ffffff;
    padding: 6px 16px;
    border: none;
}
QPushButton:disabled {
    background: #d9dded;
    color: #9ea2b5;
}
QLineEdit, QComboBox {
    background: #ffffff;
    border: 1px solid #dfe3ef;
    border-radius: 6px;
    padding: 4px 8px;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: transparent;
    width: 10px;
    margin: 4px;
}
QScrollBar::handle {
    background: #cfd6eb;
    border-radius: 5px;
}
QScrollBar::handle:hover {
    background: #b8c2df;
}
#MarkdownWebView {
    background: #ffffff;
    border: none;
}
)";

static const char *MIDNIGHT_INK_THEME = R"(
QMainWindow {
    background-color: #0f111a;
    color: #e3e8ff;
}
QMenuBar {
    background: #141826;
    border-bottom: 1px solid #1f2335;
}
QMenuBar::item {
    padding: 6px 12px;
    margin: 0 4px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #1f2740;
    color: #eef1ff;
}
QMenu {
    background: #181c2c;
    border: 1px solid #252c44;
    color: #d7dcff;
}
QMenu::item {
    padding: 6px 18px;
    border-radius: 4px;
}
QMenu::item:selected {
    background: #2c365a;
}
QStatusBar {
    background: #141826;
    border-top: 1px solid #1d2133;
    color: #9da7d7;
}
QToolButton#StatsButton {
    background: #181c2c;
    border: 1px solid #232842;
    border-radius: 14px;
    padding: 4px 12px;
    color: #c8d0ff;
}
#StatsPanelTitle {
    color: #8690c2;
}
#StatsValueLabel {
    color: #eef1ff;
}
#StatsUnitLabel {
    color: #a0a9da;
}
#SideContainer {
    background: #0f111a;
    border-right: 1px solid #1d2133;
}
#SideStack {
    background: #131627;
}
#MarkdownWebView {
    background: #0f111a;
    border: none;
}
#SideRail {
    background: #131627;
    border-right: 1px solid #1d2133;
}
#SideRailButton {
    border: none;
    border-left: 3px solid transparent;
    background: transparent;
    color: #8d95c8;
    font-weight: 600;
}
#SideRailButton:checked {
    color: #eef1ff;
    border-left-color: #4a5ed4;
    background: #1f2740;
}
#CollapseButton {
    border: none;
    background: transparent;
    color: #9aa5df;
    font-size: 12px;
}
#CollapseButton:checked {
    color: #eef1ff;
}
#NotebookFilter {
    background: #181c2c;
    border: 1px solid #232842;
    border-radius: 8px;
    color: #c8d0ff;
}
#SideLabel {
    font-size: 11px;
    font-weight: 600;
    text-transform: uppercase;
    color: #8d95c8;
    letter-spacing: 1px;
}
#NewFileButton {
    background: #1f253b;
    border: 1px solid #2b3350;
    color: #b2c5ff;
    border-radius: 8px;
}
#ActiveDocLabel {
    color: #9aa2d4;
    font-weight: 600;
    background: transparent;
}
QTreeWidget, QListWidget {
    background: transparent;
    color: #cad0ff;
}
QTreeWidget::item:selected, QListWidget::item:selected {
    background: #253059;
    color: #ffffff;
}
QPushButton {
    background: #3850b2;
    border-radius: 8px;
    color: #f5f6ff;
    padding: 6px 16px;
    border: none;
}
QPushButton:hover {
    background: #4a5ed4;
}
QLineEdit, QComboBox {
    background: #161a2b;
    border: 1px solid #2a3150;
    border-radius: 6px;
    color: #d6daff;
    padding: 4px 8px;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: transparent;
    width: 10px;
    margin: 4px;
}
QScrollBar::handle {
    background: #2b3350;
    border-radius: 5px;
}
QScrollBar::handle:hover {
    background: #3c4570;
}
#MarkdownWebView {
    background: #0f111a;
    border: none;
}
)";

static const char *SOLAR_DUSK_THEME = R"(
QMainWindow {
    background-color: #fdf4ec;
    color: #3f2b24;
}
QMenuBar {
    background: #fff8f1;
    border-bottom: 1px solid #f3dccb;
}
QMenuBar::item {
    padding: 6px 12px;
    margin: 0 4px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #ffe8d3;
    color: #3f2b24;
}
QMenu {
    background: #fff8f1;
    border: 1px solid #f3dccb;
    color: #3f2b24;
}
QMenu::item:selected {
    background: #ffe0c2;
    color: #2b1c17;
}
QStatusBar {
    background: #fff8f1;
    border-top: 1px solid #f3dccb;
    color: #7a5442;
}
QToolButton#StatsButton {
    background: #fff8f1;
    border: 1px solid #f2d8c5;
    border-radius: 14px;
    padding: 4px 12px;
    color: #7a5442;
}
#StatsPanelTitle {
    color: #9b6a4f;
}
#StatsValueLabel {
    color: #3f2b24;
}
#StatsUnitLabel {
    color: #9b7663;
}
#SideContainer {
    background: #fdf4ec;
    border-right: 1px solid #f0d6c3;
}
#SideStack {
    background: #fff8f1;
}
#MarkdownWebView {
    background: #fdf4ec;
    border: none;
}
#SideRail {
    background: #f8eedf;
    border-right: 1px solid #f0d6c3;
}
#SideRailButton {
    border: none;
    border-left: 3px solid transparent;
    background: transparent;
    color: #b17a5d;
    font-weight: 600;
}
#SideRailButton:checked {
    color: #3f2b24;
    border-left-color: #d87a3f;
    background: #ffe8d5;
}
#CollapseButton {
    border: none;
    background: transparent;
    color: #a06d54;
    font-size: 12px;
}
#CollapseButton:checked {
    color: #3f2b24;
}
#NotebookFilter {
    background: #fffaf4;
    border: 1px solid #f2d8c5;
    border-radius: 8px;
    padding: 6px 10px;
}
#SideLabel {
    font-size: 11px;
    font-weight: 600;
    text-transform: uppercase;
    color: #b17a5d;
    letter-spacing: 1px;
}
#NewFileButton {
    background: #ffe6cf;
    border: 1px solid #f4c7a1;
    color: #b05d2c;
    border-radius: 8px;
}
#ActiveDocLabel {
    color: #a36b4c;
    font-weight: 600;
    background: transparent;
}
QTreeWidget, QListWidget {
    background: transparent;
    color: #51372f;
}
QTreeWidget::item:selected, QListWidget::item:selected {
    background: #ffe0c2;
    color: #2b1c17;
}
QPushButton {
    background: #d87a3f;
    border-radius: 8px;
    color: #fff3eb;
    padding: 6px 16px;
    border: none;
}
QPushButton:hover {
    background: #ea8a46;
}
QLineEdit, QComboBox {
    background: #fffaf4;
    border: 1px solid #f0d6c3;
    border-radius: 6px;
    padding: 4px 8px;
    color: #51372f;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: transparent;
    width: 10px;
    margin: 4px;
}
QScrollBar::handle {
    background: #f0d6c3;
    border-radius: 5px;
}
QScrollBar::handle:hover {
    background: #e2c2ae;
}
#MarkdownWebView {
    background: #fff8f1;
    border: none;
}
)";

static const char *ELEGANT_NOIR_THEME = R"(
QMainWindow {
    background-color: #1a1c24;
    color: #d9d8e3;
}
QMenuBar {
    background: #1f2230;
    border-bottom: 1px solid #2a2e3e;
}
QMenuBar::item {
    padding: 6px 12px;
    margin: 0 4px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #2d3144;
    color: #f6f2e9;
}
QMenu {
    background: #202331;
    border: 1px solid #2f3347;
    color: #e2e1ec;
}
QMenu::item {
    padding: 6px 18px;
    border-radius: 4px;
}
QMenu::item:selected {
    background: #3f445c;
    color: #f6f2e9;
}
QStatusBar {
    background: #1f2230;
    border-top: 1px solid #2a2e3e;
    color: #a7a9b9;
}
QToolButton#StatsButton {
    background: #23263a;
    border: 1px solid #353850;
    border-radius: 14px;
    padding: 4px 12px;
    color: #d5d3e0;
}
#StatsPanelTitle {
    color: #c0baa4;
}
#StatsValueLabel {
    color: #f5f1e4;
}
#StatsUnitLabel {
    color: #a79f89;
}
#SideContainer {
    background: #1a1c24;
    border-right: 1px solid #262a3a;
}
#SideStack {
    background: #1f2230;
}
#MarkdownWebView {
    background: #1a1c24;
    border: none;
}
#SideRail {
    background: #191c28;
    border-right: 1px solid #262a3a;
}
#SideRailButton {
    border: none;
    border-left: 3px solid transparent;
    background: transparent;
    color: #9ea1b5;
    font-weight: 600;
}
#SideRailButton:checked {
    color: #f6f2e9;
    border-left-color: #b98c54;
    background: #2d3144;
}
#CollapseButton {
    border: none;
    background: transparent;
    color: #a7a9b9;
    font-size: 12px;
}
#CollapseButton:checked {
    color: #f6f2e9;
}
#NotebookFilter {
    background: #1f2230;
    border: 1px solid #353850;
    border-radius: 8px;
    color: #d5d3e0;
}
#SideLabel {
    font-size: 11px;
    font-weight: 600;
    text-transform: uppercase;
    color: #bfbcb0;
    letter-spacing: 1px;
}
#NewFileButton {
    background: #2a2f42;
    border: 1px solid #3c415d;
    color: #eabb7a;
    border-radius: 8px;
}
#ActiveDocLabel {
    color: #d5d3e0;
    font-weight: 600;
    background: transparent;
}
QTreeWidget, QListWidget {
    background: transparent;
    color: #d9d8e3;
}
QTreeWidget::item:selected, QListWidget::item:selected {
    background: #2f3450;
    color: #f6f2e9;
}
QPushButton {
    background: #b98c54;
    border-radius: 8px;
    color: #1d1f2b;
    padding: 6px 16px;
    border: none;
}
QPushButton:hover {
    background: #d1a468;
}
QLineEdit, QComboBox {
    background: #1d1f2b;
    border: 1px solid #34384c;
    border-radius: 6px;
    color: #f5f1e4;
    padding: 4px 8px;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: transparent;
    width: 10px;
    margin: 4px;
}
QScrollBar::handle {
    background: #3c4056;
    border-radius: 5px;
}
QScrollBar::handle:hover {
    background: #4d5170;
}
#MarkdownWebView {
    background: #1a1c24;
    border: none;
}
)";

static const char *FOREST_MIST_THEME = R"(
QMainWindow {
    background-color: #ecf4ef;
    color: #223529;
}
QMenuBar {
    background: #f7fbf9;
    border-bottom: 1px solid #d3e4db;
}
QMenuBar::item {
    padding: 6px 12px;
    margin: 0 4px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #dfeee6;
    color: #1f2f25;
}
QMenu {
    background: #f8fbf9;
    border: 1px solid #d4e4dc;
    color: #223529;
}
QMenu::item:selected {
    background: #d8ede3;
    color: #122017;
}
QStatusBar {
    background: #f8fbf9;
    border-top: 1px solid #d3e4db;
    color: #4a5f51;
}
QToolButton#StatsButton {
    background: #f8fbf9;
    border: 1px solid #cfded5;
    border-radius: 14px;
    padding: 4px 12px;
    color: #4a5f51;
}
#StatsPanelTitle {
    color: #588068;
}
#StatsValueLabel {
    color: #1b2a21;
}
#StatsUnitLabel {
    color: #5f7767;
}
#SideContainer {
    background: #ecf4ef;
    border-right: 1px solid #d3e4db;
}
#SideStack {
    background: #f7fbf9;
}
#MarkdownWebView {
    background: #ecf4ef;
    border: none;
}
#SideRail {
    background: #e6f2ea;
    border-right: 1px solid #d3e4db;
}
#SideRailButton {
    border: none;
    border-left: 3px solid transparent;
    background: transparent;
    color: #4a5f51;
    font-weight: 600;
}
#SideRailButton:checked {
    color: #1e2f24;
    border-left-color: #3d8560;
    background: #dcefe4;
}
#CollapseButton {
    border: none;
    background: transparent;
    color: #4a5f51;
    font-size: 12px;
}
#CollapseButton:checked {
    color: #1e2f24;
}
#NotebookFilter {
    background: #f8fbf9;
    border: 1px solid #cfded5;
    border-radius: 8px;
    padding: 6px 10px;
}
#SideLabel {
    font-size: 11px;
    font-weight: 600;
    text-transform: uppercase;
    color: #4a5f51;
    letter-spacing: 1px;
}
#NewFileButton {
    background: #e0f2e6;
    border: 1px solid #b6d8c3;
    color: #2f6b4c;
    border-radius: 8px;
}
#ActiveDocLabel {
    color: #3c5245;
    font-weight: 600;
    background: transparent;
}
QTreeWidget, QListWidget {
    background: transparent;
    color: #234031;
}
QTreeWidget::item:selected, QListWidget::item:selected {
    background: #d2eadf;
    color: #123020;
}
QPushButton {
    background: #3d8560;
    border-radius: 8px;
    color: #ecf4ef;
    padding: 6px 16px;
    border: none;
}
QPushButton:hover {
    background: #4b9a71;
}
QLineEdit, QComboBox {
    background: #f8fbf9;
    border: 1px solid #c3d8cd;
    border-radius: 6px;
    padding: 4px 8px;
    color: #223529;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: transparent;
    width: 10px;
    margin: 4px;
}
QScrollBar::handle {
    background: #c3d8cd;
    border-radius: 5px;
}
QScrollBar::handle:hover {
    background: #b2cbbf;
}
#MarkdownWebView {
    background: #f7fbf9;
    border: none;
}
)";

static const char *SAKURA_BREEZE_THEME = R"(
QMainWindow {
    background-color: #fff6f8;
    color: #412534;
}
QMenuBar {
    background: #ffffff;
    border-bottom: 1px solid #f3d5de;
}
QMenuBar::item {
    padding: 6px 12px;
    margin: 0 4px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #ffe5ef;
    color: #3a1c2c;
}
QMenu {
    background: #ffffff;
    border: 1px solid #f3d5de;
    color: #412534;
}
QMenu::item:selected {
    background: #ffd6e5;
    color: #2c111f;
}
QStatusBar {
    background: #ffffff;
    border-top: 1px solid #f3d5de;
    color: #8e5a75;
}
QToolButton#StatsButton {
    background: #ffffff;
    border: 1px solid #f0ccda;
    border-radius: 14px;
    padding: 4px 12px;
    color: #8e5a75;
}
#StatsPanelTitle {
    color: #b4688b;
}
#StatsValueLabel {
    color: #3a1c2c;
}
#StatsUnitLabel {
    color: #a56a87;
}
#SideContainer {
    background: #fff6f8;
    border-right: 1px solid #f4d5e1;
}
#SideStack {
    background: #ffffff;
}
#MarkdownWebView {
    background: #fff6f8;
    border: none;
}
#SideRail {
    background: #ffeef4;
    border-right: 1px solid #f4d5e1;
}
#SideRailButton {
    border: none;
    border-left: 3px solid transparent;
    background: transparent;
    color: #b4688b;
    font-weight: 600;
}
#SideRailButton:checked {
    color: #3a1c2c;
    border-left-color: #e0729c;
    background: #ffdbe8;
}
#CollapseButton {
    border: none;
    background: transparent;
    color: #b4688b;
    font-size: 12px;
}
#CollapseButton:checked {
    color: #3a1c2c;
}
#NotebookFilter {
    background: #ffffff;
    border: 1px solid #f0ccda;
    border-radius: 8px;
    padding: 6px 10px;
}
#SideLabel {
    font-size: 11px;
    font-weight: 600;
    text-transform: uppercase;
    color: #b4688b;
    letter-spacing: 1px;
}
#NewFileButton {
    background: #ffd7e6;
    border: 1px solid #f0bfd0;
    color: #b44d7a;
    border-radius: 8px;
}
#ActiveDocLabel {
    color: #a25c7c;
    font-weight: 600;
    background: transparent;
}
QTreeWidget, QListWidget {
    background: transparent;
    color: #4f2639;
}
QTreeWidget::item:selected, QListWidget::item:selected {
    background: #ffccdd;
    color: #2c111f;
}
QPushButton {
    background: #e0729c;
    border-radius: 8px;
    color: #ffffff;
    padding: 6px 16px;
    border: none;
}
QPushButton:hover {
    background: #f186ad;
}
QLineEdit, QComboBox {
    background: #ffffff;
    border: 1px solid #f0ccda;
    border-radius: 6px;
    padding: 4px 8px;
    color: #412534;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: transparent;
    width: 10px;
    margin: 4px;
}
QScrollBar::handle {
    background: #f0ccda;
    border-radius: 5px;
}
QScrollBar::handle:hover {
    background: #e1b7c8;
}
#MarkdownWebView {
    background: #fff6f8;
    border: none;
}
)";

QString themeStylesheet(ThemeId id)
{
    switch (id) {
    case ThemeId::Light:
        return QString::fromUtf8(CLASSIC_LIGHT_THEME);
    case ThemeId::Dark:
        return QString::fromUtf8(MIDNIGHT_INK_THEME);
    case ThemeId::Solarized:
        return QString::fromUtf8(SOLAR_DUSK_THEME);
    case ThemeId::Elegant:
        return QString::fromUtf8(ELEGANT_NOIR_THEME);
    case ThemeId::Forest:
        return QString::fromUtf8(FOREST_MIST_THEME);
    case ThemeId::Sakura:
        return QString::fromUtf8(SAKURA_BREEZE_THEME);
    default:
        return QString::fromUtf8(CLASSIC_LIGHT_THEME);
    }
}

ThemePalette themePalette(ThemeId id)
{
    switch (id) {
    case ThemeId::Light:
        return ThemePalette{
            QStringLiteral("#f6f7fb"),
            QStringLiteral("#ffffff"),
            QStringLiteral("#2b2d3c"),
            QStringLiteral("#7b7f91"),
            QStringLiteral("#d64545"),
            QStringLiteral("#e2e6f0"),
            QStringLiteral("#f5f6fa"),
            QStringLiteral("#c43c67"),
            QStringLiteral("#11131a"),
            QStringLiteral("#e9edf5"),
            QStringLiteral("rgba(214, 69, 69, 0.06)"),
            QStringLiteral("#d64545"),
            QStringLiteral("#5c5f6f"),
            QStringLiteral("rgba(214, 69, 69, 0.22)"),
            QStringLiteral("#f4f5f9"),
            QStringLiteral("#4a4e64"),
            QStringLiteral("rgba(214, 69, 69, 0.4)")
        };
    case ThemeId::Dark:
        return ThemePalette{
            QStringLiteral("#0f111a"),
            QStringLiteral("#141826"),
            QStringLiteral("#e3e8ff"),
            QStringLiteral("#9aa5df"),
            QStringLiteral("#4a5ed4"),
            QStringLiteral("#232842"),
            QStringLiteral("#1a1f33"),
            QStringLiteral("#d5dbff"),
            QStringLiteral("#0c101f"),
            QStringLiteral("#cfd5ff"),
            QStringLiteral("rgba(74, 94, 212, 0.12)"),
            QStringLiteral("#4a5ed4"),
            QStringLiteral("#b8c0ff"),
            QStringLiteral("rgba(74, 94, 212, 0.35)"),
            QStringLiteral("#1f253f"),
            QStringLiteral("#c8d1ff"),
            QStringLiteral("rgba(74, 94, 212, 0.5)")
        };
    case ThemeId::Solarized:
        return ThemePalette{
            QStringLiteral("#fdf4ec"),
            QStringLiteral("#fff8f1"),
            QStringLiteral("#3f2b24"),
            QStringLiteral("#9b6a4f"),
            QStringLiteral("#d87a3f"),
            QStringLiteral("#f2d8c5"),
            QStringLiteral("#fff1e4"),
            QStringLiteral("#b05d2c"),
            QStringLiteral("#2e1b15"),
            QStringLiteral("#ffe7d4"),
            QStringLiteral("#ffe8d5"),
            QStringLiteral("#d87a3f"),
            QStringLiteral("#6a4032"),
            QStringLiteral("rgba(216, 122, 63, 0.2)"),
            QStringLiteral("#ffe0c2"),
            QStringLiteral("#5a3728"),
            QStringLiteral("rgba(216, 122, 63, 0.45)")
        };
    case ThemeId::Elegant:
        return ThemePalette{
            QStringLiteral("#1a1c24"),
            QStringLiteral("#1f2230"),
            QStringLiteral("#d9d8e3"),
            QStringLiteral("#a7a9b9"),
            QStringLiteral("#b98c54"),
            QStringLiteral("#353850"),
            QStringLiteral("#2a2f42"),
            QStringLiteral("#eabb7a"),
            QStringLiteral("#141624"),
            QStringLiteral("#f6f2e9"),
            QStringLiteral("rgba(185, 140, 84, 0.18)"),
            QStringLiteral("#b98c54"),
            QStringLiteral("#d5d3e0"),
            QStringLiteral("rgba(185, 140, 84, 0.35)"),
            QStringLiteral("#2a2d3f"),
            QStringLiteral("#e0ded4"),
            QStringLiteral("rgba(185, 140, 84, 0.6)")
        };
    case ThemeId::Forest:
        return ThemePalette{
            QStringLiteral("#ecf4ef"),
            QStringLiteral("#f8fbf9"),
            QStringLiteral("#223529"),
            QStringLiteral("#5f7767"),
            QStringLiteral("#3d8560"),
            QStringLiteral("#cfded5"),
            QStringLiteral("#edf5ef"),
            QStringLiteral("#2f6b4c"),
            QStringLiteral("#1c3c2c"),
            QStringLiteral("#cce7d9"),
            QStringLiteral("rgba(61, 133, 96, 0.14)"),
            QStringLiteral("#3d8560"),
            QStringLiteral("#34523f"),
            QStringLiteral("rgba(61, 133, 96, 0.2)"),
            QStringLiteral("#dcefe4"),
            QStringLiteral("#224535"),
            QStringLiteral("rgba(61, 133, 96, 0.4)")
        };
    case ThemeId::Sakura:
        return ThemePalette{
            QStringLiteral("#fff6f8"),
            QStringLiteral("#ffffff"),
            QStringLiteral("#412534"),
            QStringLiteral("#a56a87"),
            QStringLiteral("#e0729c"),
            QStringLiteral("#f3d5de"),
            QStringLiteral("#fff0f5"),
            QStringLiteral("#b44d7a"),
            QStringLiteral("#2b0f1e"),
            QStringLiteral("#ffdbe8"),
            QStringLiteral("rgba(224, 114, 156, 0.12)"),
            QStringLiteral("#e0729c"),
            QStringLiteral("#613047"),
            QStringLiteral("rgba(224, 114, 156, 0.25)"),
            QStringLiteral("#ffe3ef"),
            QStringLiteral("#7b3c57"),
            QStringLiteral("rgba(224, 114, 156, 0.55)")
        };
    default:
        return themePalette(ThemeId::Light);
    }
}
