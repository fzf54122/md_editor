#include "StatsPanel.h"

#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

StatsPanel::StatsPanel(QWidget *parent)
    : QObject(parent)
{
    statsButton = new QToolButton(parent);
    statsButton->setObjectName("StatsButton");
    statsButton->setText("0 词");
    statsButton->setAutoRaise(true);
    statsButton->setPopupMode(QToolButton::InstantPopup);
    statsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    statsButton->setArrowType(Qt::DownArrow);

    statsMenu = new QMenu(statsButton);
    QWidget *statsPanel = new QWidget(statsMenu);
    QVBoxLayout *panelLayout = new QVBoxLayout(statsPanel);
    panelLayout->setContentsMargins(12, 12, 12, 12);
    panelLayout->setSpacing(8);

    QLabel *panelTitle = new QLabel("字数统计", statsPanel);
    panelTitle->setObjectName("StatsPanelTitle");
    panelTitle->setAlignment(Qt::AlignCenter); 
    panelLayout->addWidget(panelTitle);

    QWidgetAction *panelAction = new QWidgetAction(statsMenu);
    panelAction->setDefaultWidget(statsPanel);
    statsMenu->addAction(panelAction);
    statsMenu->addSeparator();

    QActionGroup *statsDisplayGroup = new QActionGroup(statsMenu);
    statsDisplayGroup->setExclusive(true);
    auto addDisplayAction = [&](const QString &text, StatsDisplayMode mode) -> QAction* {
        QAction *action = new QAction(text, statsMenu);
        action->setCheckable(true);
        statsDisplayGroup->addAction(action);
        statsMenu->addAction(action);
        connect(action, &QAction::triggered, this, [this, mode, action]() {
            statsDisplayMode = mode;
            updateActionText(action, mode, currentStats);
            refreshDisplayText();
        });
        return action;
    };

    // 在初始文本里增加数字与单位之间的空格，避免数字与中文单位粘连。
    statsMinutesAction = addDisplayAction("显示分钟: 0 分钟", StatsDisplayMode::Minutes);
    statsLinesAction   = addDisplayAction("显示行数: 0 行", StatsDisplayMode::Lines);
    statsWordsAction   = addDisplayAction("显示词数: 0 词", StatsDisplayMode::Words);
    statsCharsAction   = addDisplayAction("显示字符: 0 字符", StatsDisplayMode::Characters);
    statsWordsAction->setChecked(true);
    statsButton->setMenu(statsMenu);
}

void StatsPanel::setDisplayMode(StatsDisplayMode mode)
{
    statsDisplayMode = mode;
    refreshDisplayText();
}

void StatsPanel::refreshDisplayText()
{
    if (!statsButton)
        return;

    QString displayText;
    switch (statsDisplayMode) {
    case StatsDisplayMode::Minutes:
        displayText = QString("%1 分钟").arg(currentStats.minutes);
        break;
    case StatsDisplayMode::Lines:
        displayText = QString("%1 行").arg(currentStats.lines);
        break;
    case StatsDisplayMode::Words:
        displayText = QString("%1 词").arg(currentStats.words);
        break;
    case StatsDisplayMode::Characters:
        displayText = QString("%1 字符").arg(currentStats.characters);
        break;
    }

    statsButton->setText(displayText);
}

void StatsPanel::updateActionText(QAction *action, StatsDisplayMode mode, const TextStats &stats)
{
    if (!action)
        return;

    QString newText;
    switch (mode) {
    case StatsDisplayMode::Minutes:
        newText = QString("显示分钟: %1 分钟").arg(stats.minutes);
        break;
    case StatsDisplayMode::Lines:
        newText = QString("显示行数: %1 行").arg(stats.lines);
        break;
    case StatsDisplayMode::Words:
        newText = QString("显示词数: %1 词").arg(stats.words);
        break;
    case StatsDisplayMode::Characters:
        newText = QString("显示字符: %1 字符").arg(stats.characters);
        break;
    }
    action->setText(newText);
}

void StatsPanel::updateStats(const QString &markdown)
{
    currentStats = TextStatistics::compute(markdown);

    refreshDisplayText();

    updateActionText(statsMinutesAction, StatsDisplayMode::Minutes, currentStats);
    updateActionText(statsLinesAction, StatsDisplayMode::Lines, currentStats);
    updateActionText(statsWordsAction, StatsDisplayMode::Words, currentStats);
    updateActionText(statsCharsAction, StatsDisplayMode::Characters, currentStats);
}
