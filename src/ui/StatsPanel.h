#pragma once

#include <QWidget>
#include <QMenu>
#include <QLabel>
#include <QToolButton>

#include "services/TextStatistics.h"

enum class StatsDisplayMode {
    Minutes,
    Lines,
    Words,
    Characters
};

// 状态栏统计面板：封装统计菜单与按钮展示。
class StatsPanel : public QObject
{
    Q_OBJECT

public:
    explicit StatsPanel(QWidget *parent = nullptr);

    QToolButton *button() const { return statsButton; }
    StatsDisplayMode displayMode() const { return statsDisplayMode; }
    void setDisplayMode(StatsDisplayMode mode);
    void updateStats(const QString &markdown);

private:
    void refreshDisplayText();
    void updateActionText(QAction *action, StatsDisplayMode mode, const TextStats &stats);

    QToolButton *statsButton{};
    QMenu *statsMenu{};
    QAction *statsMinutesAction{};
    QAction *statsLinesAction{};
    QAction *statsWordsAction{};
    QAction *statsCharsAction{};
    StatsDisplayMode statsDisplayMode{StatsDisplayMode::Words};
    TextStats currentStats{};
};
