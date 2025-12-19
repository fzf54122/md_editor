#include "NotebookPane.h"

#include <QAbstractItemView>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>

NotebookPane::NotebookPane(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SideContainer");
    setFixedWidth(260);

    QHBoxLayout *sideOuterLayout = new QHBoxLayout(this);
    sideOuterLayout->setContentsMargins(0, 0, 0, 0);
    sideOuterLayout->setSpacing(0);

    QWidget *sideRail = new QWidget(this);
    sideRail->setObjectName("SideRail");
    sideRail->setFixedWidth(48);
    sideRail->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout *railLayout = new QVBoxLayout(sideRail);
    railLayout->setContentsMargins(0, 0, 0, 0);
    railLayout->setSpacing(12);

    sideTabGroup = new QButtonGroup(this);
    sideTabGroup->setExclusive(true);

    auto createRailButton = [&](const QString &text, const QString &objName, int index) -> QToolButton* {
        QToolButton *btn = new QToolButton(sideRail);
        btn->setText(text);
        btn->setObjectName(objName);
        btn->setCheckable(true);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setMinimumHeight(36);
        railLayout->addWidget(btn);
        sideTabGroup->addButton(btn, index);
        return btn;
    };

    notebookTabBtn = createRailButton("文", "SideRailButton", 0);
    outlineTabBtn  = createRailButton("纲", "SideRailButton", 1);
    notebookTabBtn->setChecked(true);
    railLayout->addStretch();

    sideStack = new QStackedWidget(this);
    sideStack->setObjectName("SideStack");

    QWidget *notebookPage = new QWidget(sideStack);
    QVBoxLayout *notebookLayout = new QVBoxLayout(notebookPage);
    notebookLayout->setContentsMargins(18, 18, 18, 18);
    notebookLayout->setSpacing(10);

    QHBoxLayout *filesHeaderLayout = new QHBoxLayout();
    filesHeaderLayout->setContentsMargins(0, 0, 0, 0);
    filesHeaderLayout->setSpacing(6);

    QLabel *notebookLabel = new QLabel("文件", notebookPage);
    notebookLabel->setObjectName("SideLabel");
    filesHeaderLayout->addWidget(notebookLabel, 1);

    QPushButton *newFileButton = new QPushButton("+ 新建", notebookPage);
    newFileButton->setObjectName("NewFileButton");
    newFileButton->setCursor(Qt::PointingHandCursor);
    filesHeaderLayout->addWidget(newFileButton, 0, Qt::AlignRight);
    notebookLayout->addLayout(filesHeaderLayout);

    activeDocLabel = new QLabel("未命名.md", notebookPage);
    activeDocLabel->setObjectName("ActiveDocLabel");
    notebookLayout->addWidget(activeDocLabel);

    fileTree = new QTreeWidget(notebookPage);
    fileTree->setObjectName("FileTreeWidget");
    fileTree->setHeaderHidden(true);
    fileTree->setIndentation(16);
    fileTree->setAnimated(true);
    fileTree->setContextMenuPolicy(Qt::CustomContextMenu);
    notebookLayout->addWidget(fileTree, 1);

    QWidget *outlinePage = new QWidget(sideStack);
    QVBoxLayout *outlineLayout = new QVBoxLayout(outlinePage);
    outlineLayout->setContentsMargins(18, 18, 18, 18);
    outlineLayout->setSpacing(10);

    QLabel *outlineLabel = new QLabel("大纲", outlinePage);
    outlineLabel->setObjectName("SideLabel");
    outlineLayout->addWidget(outlineLabel);

    outlineList = new QListWidget(outlinePage);
    outlineList->setObjectName("OutlineList");
    outlineList->setSelectionMode(QAbstractItemView::SingleSelection);
    outlineLayout->addWidget(outlineList, 1);

    sideStack->addWidget(notebookPage);
    sideStack->addWidget(outlinePage);

    QPushButton *collapseBtn = new QPushButton(QChar(0x25C0), sideRail);
    collapseBtn->setObjectName("CollapseButton");
    collapseBtn->setCheckable(true);
    railLayout->addWidget(collapseBtn);

    sideOuterLayout->addWidget(sideRail);
    sideOuterLayout->addWidget(sideStack, 1);

    // 信号转发
    connect(fileTree, &QTreeWidget::itemActivated, this, [this](QTreeWidgetItem *item, int) {
        emit fileItemActivated(item);
    });
    connect(fileTree, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem *item, int) {
        emit fileItemActivated(item);
    });
    connect(fileTree, &QWidget::customContextMenuRequested, this, &NotebookPane::fileContextMenuRequested);
    connect(newFileButton, &QPushButton::clicked, this, &NotebookPane::newFileRequested);
    connect(outlineList, &QListWidget::itemActivated, this, &NotebookPane::outlineItemActivated);
    connect(outlineList, &QListWidget::itemClicked, this, &NotebookPane::outlineItemActivated);

    connect(sideTabGroup, &QButtonGroup::idClicked, this, [this](int index) {
        setCurrentTab(index);
    });

    connect(collapseBtn, &QPushButton::toggled, this, [this](bool checked) {
        setCollapsed(checked);
    });
}

void NotebookPane::setActiveDocumentLabel(const QString &text)
{
    if (activeDocLabel)
        activeDocLabel->setText(text);
}

void NotebookPane::showOutlineTab()
{
    setCurrentTab(1);
}

void NotebookPane::showNotebookTab()
{
    setCurrentTab(0);
}

void NotebookPane::setCollapsed(bool collapsed)
{
    if (!sideStack)
        return;
    sideStack->setVisible(!collapsed);
    setFixedWidth(collapsed ? 48 : 260);
}

void NotebookPane::setCurrentTab(int index)
{
    if (sideStack)
        sideStack->setCurrentIndex(index);
    if (sideTabGroup) {
        if (auto *btn = sideTabGroup->button(index))
            btn->setChecked(true);
    }
}
