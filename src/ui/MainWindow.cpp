#include "MainWindow.h"

#include <QApplication>
#include <QtGlobal>

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStatusBar>
#include <QCloseEvent>
#include <QSignalBlocker>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QSettings>
#include <QListWidget>
#include <QTreeWidget>
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QActionGroup>
#include <QDir>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>
#include <QUrl>
#include <QTimer>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>
#include <QWidgetAction>

#include "theme/Theme.h"
#include "EditorBridge.h"

namespace {
constexpr int kMaxRecentEntries = 5;
const char *kRecentFilesKey = "recentFiles";
const char *kRecentFoldersKey = "recentFolders";

bool isCjkCharacter(const QChar &ch)
{
    const uint code = ch.unicode();
    return (code >= 0x3400 && code <= 0x4DBF)
        || (code >= 0x4E00 && code <= 0x9FFF)
        || (code >= 0x3040 && code <= 0x30FF)
        || (code >= 0xAC00 && code <= 0xD7AF);
}

bool isAsciiWordChar(const QChar &ch)
{
    return ch.isLetterOrNumber() || ch == '_' || ch == '\'' || ch == '-';
}

bool isMathSymbolCharacter(const QChar &ch)
{
    return ch.category() == QChar::Symbol_Math;
}

bool pruneHistoryList(QStringList &list, bool requireDirectory)
{
    bool modified = false;
    for (int i = list.size() - 1; i >= 0; --i) {
        QFileInfo info(list.at(i));
        if (!info.exists() || (requireDirectory ? !info.isDir() : !info.isFile())) {
            list.removeAt(i);
            modified = true;
            continue;
        }
        const QString canonical = info.canonicalFilePath();
        const QString normalized = canonical.isEmpty() ? info.absoluteFilePath() : canonical;
        if (list.at(i) != normalized) {
            list[i] = normalized;
            modified = true;
        }
    }

    while (list.size() > kMaxRecentEntries) {
        list.removeLast();
        modified = true;
    }
    return modified;
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , lastDirectory(QDir::homePath())
{
    resize(1080, 680);
    setWindowTitle("Md Editor");

    QLabel *subtitleLeft = new QLabel("专注写作 · 所见即所得", this);
    subtitleLeft->setObjectName("StatusSubtitle");
    subtitleLeft->setAlignment(Qt::AlignCenter);
    statusBar()->addWidget(subtitleLeft, 1);

    statsButton = new QToolButton(this);
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
    panelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    panelLayout->addWidget(panelTitle);

    QGridLayout *grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(6);

    auto createValueLabel = [statsPanel]() -> QLabel* {
        QLabel *label = new QLabel("0", statsPanel);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setObjectName("StatsValueLabel");
        return label;
    };

    auto createUnitLabel = [statsPanel](const QString &text) -> QLabel* {
        QLabel *label = new QLabel(text, statsPanel);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        label->setObjectName("StatsUnitLabel");
        return label;
    };

    statsMinutesLabel = createValueLabel();
    statsLinesLabel = createValueLabel();
    statsWordsLabel = createValueLabel();
    statsCharsLabel = createValueLabel();

    // grid->addWidget(statsMinutesLabel, 0, 0);
    // grid->addWidget(createUnitLabel("分钟"), 0, 1);
    // grid->addWidget(statsLinesLabel, 1, 0);
    // grid->addWidget(createUnitLabel("行"), 1, 1);
    // grid->addWidget(statsWordsLabel, 2, 0);
    // grid->addWidget(createUnitLabel("词"), 2, 1);
    // grid->addWidget(statsCharsLabel, 3, 0);
    // grid->addWidget(createUnitLabel("字符"), 3, 1);

    panelLayout->addLayout(grid);

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
        updateActionText(action, mode);
        QObject::connect(action, &QAction::triggered, this, [this, mode, action]() {
            statsDisplayMode = mode;
            updateStatusInfo();
            updateActionText(action, mode);
        });
        return action;
    };



    statsMinutesAction = addDisplayAction("显示分钟: 0分钟", StatsDisplayMode::Minutes);
    statsLinesAction   = addDisplayAction("显示行数: 0行", StatsDisplayMode::Lines);
    statsWordsAction   = addDisplayAction("显示词数: 0词", StatsDisplayMode::Words);
    statsCharsAction   = addDisplayAction("显示字符: 0字符", StatsDisplayMode::Characters);
    statsWordsAction->setChecked(true);
    statsButton->setMenu(statsMenu);

    statusBar()->addPermanentWidget(statsButton);

    QSettings settings("md-editor", "app");
    const auto decodeTheme = [](int value) -> ThemeId {
        switch (value) {
        case static_cast<int>(ThemeId::Light): return ThemeId::Light;
        case static_cast<int>(ThemeId::Dark): return ThemeId::Dark;
        case static_cast<int>(ThemeId::Solarized): return ThemeId::Solarized;
        case static_cast<int>(ThemeId::Elegant): return ThemeId::Elegant;
        case static_cast<int>(ThemeId::Forest): return ThemeId::Forest;
        case static_cast<int>(ThemeId::Sakura): return ThemeId::Sakura;
        default: return ThemeId::Elegant;
        }
    };
    const int storedTheme = settings.value("themeId", static_cast<int>(ThemeId::Elegant)).toInt();
    currentTheme = decodeTheme(storedTheme);
    qApp->setStyleSheet(themeStylesheet(currentTheme));

    setupMenu();
    menuBar()->setVisible(false);
    setupEditor();
    setupConnections();
    applyEditorTheme();
    selectThemeAction(currentTheme);

    loadRecentHistory();
    resetToNewDocument();

    if (!restoreSessionFromHistory()) {
        QTimer::singleShot(0, this, &MainWindow::promptInitialOpen);
    }
}

void MainWindow::setupEditor()
{
    centralContainer = new QWidget(this);
    QVBoxLayout *rootLayout = new QVBoxLayout(centralContainer);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QWidget *bodyWidget = new QWidget(centralContainer);
    QHBoxLayout *bodyLayout = new QHBoxLayout(bodyWidget);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    sideContainer = new QWidget(bodyWidget);
    sideContainer->setObjectName("SideContainer");
    sideContainer->setFixedWidth(260);
    QHBoxLayout *sideOuterLayout = new QHBoxLayout(sideContainer);
    sideOuterLayout->setContentsMargins(0, 0, 0, 0);
    sideOuterLayout->setSpacing(0);

    sideRail = new QWidget(sideContainer);
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

    sideStack = new QStackedWidget(sideContainer);
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

    newFileButton = new QPushButton("+ 新建", notebookPage);
    newFileButton->setObjectName("NewFileButton");
    newFileButton->setCursor(Qt::PointingHandCursor);
    filesHeaderLayout->addWidget(newFileButton, 0, Qt::AlignRight);
    notebookLayout->addLayout(filesHeaderLayout);

    activeDocLabel = new QLabel("未命名.md", notebookPage);
    activeDocLabel->setObjectName("ActiveDocLabel");
    notebookLayout->addWidget(activeDocLabel);

    fileTreeWidget = new QTreeWidget(notebookPage);
    fileTreeWidget->setObjectName("FileTreeWidget");
    fileTreeWidget->setHeaderHidden(true);
    fileTreeWidget->setIndentation(16);
    fileTreeWidget->setAnimated(true);
    fileTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    notebookLayout->addWidget(fileTreeWidget, 1);
    connect(fileTreeWidget, &QTreeWidget::itemActivated, this, &MainWindow::handleFileTreeItem);
    connect(fileTreeWidget, &QTreeWidget::itemClicked, this, &MainWindow::handleFileTreeItem);
    connect(fileTreeWidget, &QWidget::customContextMenuRequested, this, &MainWindow::showFileTreeContextMenu);
    connect(newFileButton, &QPushButton::clicked, this, &MainWindow::createNewSidebarFile);

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

    connect(outlineList, &QListWidget::itemActivated, this, [this](QListWidgetItem *item) {
        focusOutlineItem(item);
    });
    connect(outlineList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        focusOutlineItem(item);
    });

    sideStack->addWidget(notebookPage);
    sideStack->addWidget(outlinePage);

    QPushButton *collapseBtn = new QPushButton(QChar(0x25C0), sideRail);
    collapseBtn->setObjectName("CollapseButton");
    collapseBtn->setCheckable(true);
    railLayout->addWidget(collapseBtn);

    sideOuterLayout->addWidget(sideRail);
    sideOuterLayout->addWidget(sideStack, 1);

    webView = new QWebEngineView(bodyWidget);
    webView->setContextMenuPolicy(Qt::NoContextMenu);
    webView->setObjectName("MarkdownWebView");
    webView->setStyleSheet("border: none; background: #fbfbfd;");
    webView->setUrl(QUrl("qrc:/editor/editor.html"));

    webChannel = new QWebChannel(webView);
    editorBridge = new EditorBridge(this);
    webChannel->registerObject("editorBridge", editorBridge);
    webView->page()->setWebChannel(webChannel);

    connect(editorBridge, &EditorBridge::contentChanged, this, [this](const QString &markdown) {
        currentMarkdown = markdown;
        markDocumentDirty();
        updateStatusInfo();
        updateOutline();
    });

    connect(editorBridge, &EditorBridge::initialContentRequested, this, [this]() {
        editorReady = true;
        if (!pendingMarkdown.isEmpty())
            currentMarkdown = pendingMarkdown;
        pushMarkdownToEditor();
        applyEditorTheme();
        applyEditorPlainMode();
    });

    connect(editorBridge, &EditorBridge::cursorStateChanged, this, [this](int line, int words) {
        if (line < 1)
            line = 1;
        if (words < 0)
            words = 0;
        currentCursorLine = line;
        Q_UNUSED(words);
        updateStatusInfo();
        
    });

    bodyLayout->addWidget(sideContainer);
    bodyLayout->addWidget(webView, 1);

    rootLayout->addWidget(bodyWidget, 1);
    setCentralWidget(centralContainer);

    connect(sideTabGroup, &QButtonGroup::idClicked, this, [this](int index) {
        sideStack->setCurrentIndex(index);
    });

    connect(collapseBtn, &QPushButton::toggled, this, [this](bool checked) {
        sideStack->setVisible(!checked);
        sideContainer->setFixedWidth(checked ? 48 : 260);
    });

    notebookCurrentPath.clear();
    refreshNotebookList();
}

void MainWindow::setupMenu()
{
    QMenuBar *menuBar = this->menuBar();

    fileMenu = menuBar->addMenu("文件");
    newAction = fileMenu->addAction("新建");
    newAction->setShortcut(QKeySequence::New);
    openAction = fileMenu->addAction("打开");
    openAction->setShortcut(QKeySequence::Open);
    openFolderAction = fileMenu->addAction("打开文件夹");
    openFolderAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
    saveAction = fileMenu->addAction("保存");
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction = fileMenu->addAction("另存为");
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    exitAction = fileMenu->addAction("退出");

    editMenu = menuBar->addMenu("编辑");
    undoAction = editMenu->addAction("撤销");
    undoAction->setShortcut(QKeySequence::Undo);
    redoAction = editMenu->addAction("重做");
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addSeparator();
    cutAction = editMenu->addAction("剪切");
    cutAction->setShortcut(QKeySequence::Cut);
    copyAction = editMenu->addAction("复制");
    copyAction->setShortcut(QKeySequence::Copy);
    pasteAction = editMenu->addAction("粘贴");
    pasteAction->setShortcut(QKeySequence::Paste);
    selectAllAction = editMenu->addAction("全选");
    selectAllAction->setShortcut(QKeySequence::SelectAll);

    paragraphMenu = menuBar->addMenu("段落");
    heading1Action = paragraphMenu->addAction("标题 H1");
    heading2Action = paragraphMenu->addAction("标题 H2");
    heading3Action = paragraphMenu->addAction("标题 H3");
    paragraphMenu->addSeparator();
    blockquoteAction = paragraphMenu->addAction("引用");

    formatMenu = menuBar->addMenu("格式");
    boldAction = formatMenu->addAction("加粗");
    boldAction->setShortcut(QKeySequence::Bold);

    italicAction = formatMenu->addAction("斜体");
    italicAction->setShortcut(QKeySequence::Italic);

    codeAction = formatMenu->addAction("行内代码");
    codeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_QuoteLeft));

    codeBlockAction = formatMenu->addAction("代码块");
    codeBlockAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_QuoteLeft));

    linkAction = formatMenu->addAction("插入链接");
    linkAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_K));

    unorderedListAction = formatMenu->addAction("无序列表");
    unorderedListAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));

    viewMenu = menuBar->addMenu("视图");
    focusModeAction = viewMenu->addAction("聚焦模式");
    focusModeAction->setCheckable(true);
    outlineViewAction = viewMenu->addAction("显示大纲");

    themeMenu = menuBar->addMenu("主题");
    QActionGroup *themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);
    lightThemeAction = themeMenu->addAction("晨光浅色");
    darkThemeAction = themeMenu->addAction("墨夜深色");
    solarizedThemeAction = themeMenu->addAction("暮光暖日");
    elegantThemeAction = themeMenu->addAction("优雅黑金");
    forestThemeAction = themeMenu->addAction("雾森青黛");
    sakuraThemeAction = themeMenu->addAction("樱云粉雾");
    for (QAction *action : {lightThemeAction, darkThemeAction, solarizedThemeAction, elegantThemeAction, forestThemeAction, sakuraThemeAction}) {
        action->setCheckable(true);
        action->setActionGroup(themeGroup);
    }

    helpMenu = menuBar->addMenu("帮助");
    shortcutsAction = helpMenu->addAction("快捷键参考");
    aboutAction = helpMenu->addAction("关于");
}

void MainWindow::setupConnections()
{
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    auto triggerAction = [this](QWebEnginePage::WebAction action) {
        if (webView && webView->page())
            webView->page()->triggerAction(action);
    };

    connect(undoAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Undo);
    });
    connect(redoAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Redo);
    });
    connect(cutAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Cut);
    });
    connect(copyAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Copy);
    });
    connect(pasteAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Paste);
    });
    connect(selectAllAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::SelectAll);
    });

    connect(newAction, &QAction::triggered, this, [this]() {
        if (!promptSaveIfDirty())
            return;
        resetToNewDocument();
        statusBar()->showMessage("已创建新文档", 2000);
    });

    connect(openAction, &QAction::triggered, this, [this]() {
        if (!promptSaveIfDirty())
            return;

        const QString fileName = showOpenFileDialog();
        if (fileName.isEmpty()) return;

        loadDocumentFromFile(fileName);
    });

    connect(openFolderAction, &QAction::triggered, this, [this]() {
        const QString folder = showOpenDirectoryDialog();
        if (folder.isEmpty())
            return;
        notebookCurrentPath = folder;
        refreshNotebookList();
        rememberRecentFolder(folder);
        statusBar()->showMessage(QString("已打开文件夹: %1").arg(folder), 2000);
    });

    connect(saveAction, &QAction::triggered, this, [this]() {
        saveDocument(false);
    });

    connect(saveAsAction, &QAction::triggered, this, [this]() {
        saveDocument(true);
    });

    connect(boldAction, &QAction::triggered, this, [this]() {
        executeEditorCommand("bold");
    });

    connect(italicAction, &QAction::triggered, this, [this]() {
        executeEditorCommand("italic");
    });

    connect(codeAction, &QAction::triggered, this, [this]() {
        executeEditorCommand("inline-code");
    });

    connect(codeBlockAction, &QAction::triggered, this, [this]() {
        insertCodeBlock();
    });

    connect(linkAction, &QAction::triggered, this, [this]() {
        insertLink();
    });

    connect(unorderedListAction, &QAction::triggered, this, [this]() {
        insertUnorderedList();
    });

    connect(heading1Action, &QAction::triggered, this, [this]() { applyHeadingLevel(1); });
    connect(heading2Action, &QAction::triggered, this, [this]() { applyHeadingLevel(2); });
    connect(heading3Action, &QAction::triggered, this, [this]() { applyHeadingLevel(3); });
    connect(blockquoteAction, &QAction::triggered, this, [this]() {
        executeEditorCommand("blockquote");
    });

    // 主题菜单
    auto applyTheme = [this](ThemeId id) {
        currentTheme = id;
        qApp->setStyleSheet(themeStylesheet(currentTheme));
        QSettings settings("md-editor", "app");
        settings.setValue("themeId", static_cast<int>(currentTheme));
        selectThemeAction(currentTheme);
        applyEditorTheme();
    };

    connect(lightThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Light);
    });

    connect(darkThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Dark);
    });

    connect(solarizedThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Solarized);
    });

    connect(elegantThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Elegant);
    });

    connect(forestThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Forest);
    });

    connect(sakuraThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Sakura);
    });

    connect(focusModeAction, &QAction::toggled, this, &MainWindow::toggleFocusMode);
    connect(outlineViewAction, &QAction::triggered, this, &MainWindow::openOutlineFromMenu);
    connect(shortcutsAction, &QAction::triggered, this, &MainWindow::showShortcutHelp);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::updateActionText(QAction *action, StatsDisplayMode mode)
{
    if (!action)
        return;

    QString newText;
    switch (mode) {
    case StatsDisplayMode::Minutes:
        newText = QString("显示分钟: %1分钟").arg(statsMinutesLabel ? statsMinutesLabel->text() : QStringLiteral("0"));
        break;
    case StatsDisplayMode::Lines:
        newText = QString("显示行数: %1行").arg(statsLinesLabel ? statsLinesLabel->text() : QStringLiteral("0"));
        break;
    case StatsDisplayMode::Words:
        newText = QString("显示词数: %1词").arg(statsWordsLabel ? statsWordsLabel->text() : QStringLiteral("0"));
        break;
    case StatsDisplayMode::Characters:
        newText = QString("显示字符: %1字符").arg(statsCharsLabel ? statsCharsLabel->text() : QStringLiteral("0"));
        break;
    }
    action->setText(newText);
}

void MainWindow::updateStatusInfo()
{
    if (!statsButton)
        return;

    const int lineDisplay = currentCursorLine > 0 ? currentCursorLine : 1;
    const int words = calculateWordCount(currentMarkdown);
    const int totalLines = currentMarkdown.isEmpty() ? 0 : currentMarkdown.count('\n') + 1;
    const int characters = currentMarkdown.length();
    const int minutes = words <= 0 ? 0 : ((words - 1) / 350) + 1;

    if (statsMinutesLabel)
        statsMinutesLabel->setText(QString::number(minutes));
    if (statsLinesLabel)
        statsLinesLabel->setText(QString::number(totalLines));
    if (statsWordsLabel)
        statsWordsLabel->setText(QString::number(words));
    if (statsCharsLabel)
        statsCharsLabel->setText(QString::number(characters));

    QString displayText;
    switch (statsDisplayMode) {
    case StatsDisplayMode::Minutes:
        displayText = QString("%1 分钟").arg(minutes);
        break;
    case StatsDisplayMode::Lines:
        displayText = QString("%1 行").arg(totalLines);
        break;
    case StatsDisplayMode::Words:
        displayText = QString("%1 词").arg(words);
        break;
    case StatsDisplayMode::Characters:
        displayText = QString("%1 字符").arg(characters);
        break;
    }
    statsButton->setText(displayText);

    updateActionText(statsMinutesAction, StatsDisplayMode::Minutes);
    updateActionText(statsLinesAction, StatsDisplayMode::Lines);
    updateActionText(statsWordsAction, StatsDisplayMode::Words);
    updateActionText(statsCharsAction, StatsDisplayMode::Characters);
}

void MainWindow::updateWindowTitle()
{
    QString displayName = currentFilePath.isEmpty()
        ? QStringLiteral("未命名.md")
        : QFileInfo(currentFilePath).fileName();

    const QString dirtyMarker = documentDirty ? "*" : "";
    QMainWindow::setWindowTitle(
        QString("%1%2 - Md Editor").arg(displayName, dirtyMarker)
    );
}

void MainWindow::markDocumentClean(const QString &path)
{
    currentFilePath = path;
    documentDirty = false;
    updateWindowTitle();
    updateActiveDocumentLabel();
    if (!path.isEmpty())
        setPlainTextMode(!isMarkdownFile(path));
}

void MainWindow::markDocumentDirty()
{
    if (documentDirty)
        return;

    documentDirty = true;
    updateWindowTitle();
}

bool MainWindow::promptSaveIfDirty()
{
    if (!documentDirty)
        return true;

    QMessageBox::StandardButton ret = showUnsavedWarning();

    if (ret == QMessageBox::Save) {
        return saveDocument(false);
    }

    if (ret == QMessageBox::Cancel) {
        return false;
    }

    return true;
}

bool MainWindow::saveDocument(bool forceSelectPath)
{
    QString targetPath = currentFilePath;
    if (forceSelectPath || targetPath.isEmpty()) {
        const QString fileName = showSaveFileDialog(forceSelectPath ? "另存为" : "保存文件");
        if (fileName.isEmpty())
            return false;
        targetPath = fileName;
    }

    if (!writeToFile(targetPath))
        return false;

    markDocumentClean(targetPath);
    statusBar()->showMessage(QString("已保存: %1").arg(targetPath), 2000);
    return true;
}

bool MainWindow::writeToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法保存文件");
        return false;
    }

    QTextStream out(&file);
    out << currentMarkdown;
    return true;
}

void MainWindow::loadDocumentFromFile(QString fileName, bool updateNotebookPath)
{
    // fileName is by-value so later history mutations do not invalidate the reference in this scope.
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        return;
    }

    QTextStream in(&file);
    currentMarkdown = in.readAll();
    setPlainTextMode(!isMarkdownFile(fileName));
    pushMarkdownToEditor();
    markDocumentClean(fileName);
    currentCursorLine = 1;
    updateStatusInfo();
    updateOutline();
    if (updateNotebookPath) {
        notebookCurrentPath = QFileInfo(fileName).absolutePath();
        refreshNotebookList();
    }
    selectFileTreePath(fileName);
    rememberRecentFile(fileName);
    statusBar()->showMessage(QString("已打开: %1").arg(fileName), 2000);
}

void MainWindow::loadRecentHistory()
{
    QSettings settings("md-editor", "app");
    recentFiles = settings.value(kRecentFilesKey).toStringList();
    recentFolders = settings.value(kRecentFoldersKey).toStringList();

    const bool filesChanged = pruneHistoryList(recentFiles, false);
    const bool foldersChanged = pruneHistoryList(recentFolders, true);

    if (filesChanged)
        settings.setValue(kRecentFilesKey, recentFiles);
    if (foldersChanged)
        settings.setValue(kRecentFoldersKey, recentFolders);
}

void MainWindow::promptInitialOpen()
{
    const QString filePath = showOpenFileDialog();
    if (!filePath.isEmpty()) {
        loadDocumentFromFile(filePath);
        return;
    }

    const QString folderPath = showOpenDirectoryDialog();
    if (!folderPath.isEmpty()) {
        notebookCurrentPath = folderPath;
        refreshNotebookList();
        rememberRecentFolder(folderPath);
        statusBar()->showMessage(QString("已打开文件夹: %1").arg(folderPath), 2000);
        return;
    }

    statusBar()->showMessage("未选择文件或文件夹", 2000);
}

void MainWindow::rememberRecentFile(const QString &path)
{
    if (path.isEmpty())
        return;

    QFileInfo info(path);
    if (!info.exists() || !info.isFile())
        return;

    const QString normalized = info.canonicalFilePath().isEmpty()
        ? info.absoluteFilePath()
        : info.canonicalFilePath();

    recentFiles.removeAll(normalized);
    recentFiles.prepend(normalized);
    while (recentFiles.size() > kMaxRecentEntries) {
        recentFiles.removeLast();
    }

    QSettings settings("md-editor", "app");
    settings.setValue(kRecentFilesKey, recentFiles);
}

void MainWindow::rememberRecentFolder(const QString &path)
{
    if (path.isEmpty())
        return;

    QFileInfo info(path);
    if (!info.exists() || !info.isDir())
        return;

    const QString normalized = info.canonicalFilePath().isEmpty()
        ? info.absoluteFilePath()
        : info.canonicalFilePath();

    recentFolders.removeAll(normalized);
    recentFolders.prepend(normalized);
    while (recentFolders.size() > kMaxRecentEntries) {
        recentFolders.removeLast();
    }

    QSettings settings("md-editor", "app");
    settings.setValue(kRecentFoldersKey, recentFolders);
}

bool MainWindow::isMarkdownFile(const QString &path) const
{
    if (path.isEmpty())
        return true;

    const QString ext = QFileInfo(path).suffix().toLower();
    static const QStringList markdownExt = {
        "md", "markdown", "mdown", "mkd", "mdtext", "mdtxt"
    };
    return markdownExt.contains(ext);
}

bool MainWindow::restoreSessionFromHistory()
{
    for (const QString &filePath : recentFiles) {
        QFileInfo info(filePath);
        if (!info.exists() || !info.isFile())
            continue;

        lastDirectory = info.absolutePath();
        loadDocumentFromFile(filePath);
        return true;
    }

    for (const QString &folderPath : recentFolders) {
        QDir dir(folderPath);
        if (!dir.exists())
            continue;

        lastDirectory = dir.absolutePath();
        notebookCurrentPath = dir.absolutePath();
        refreshNotebookList();
        statusBar()->showMessage(QString("已打开最近文件夹: %1").arg(dir.absolutePath()), 2000);
        return true;
    }

    return false;
}

void MainWindow::resetToNewDocument()
{
    currentMarkdown.clear();
    pushMarkdownToEditor();
    markDocumentClean(QString{});
    currentCursorLine = 1;
    updateStatusInfo();
    updateOutline();
    refreshNotebookList();
    updateActiveDocumentLabel();
    setPlainTextMode(false);
}

void MainWindow::insertLink()
{
    bool ok = false;
    const QString url = QInputDialog::getText(
        this,
        "插入链接",
        "URL:",
        QLineEdit::Normal,
        "https://",
        &ok
    );

    if (!ok)
        return;

    const QString trimmed = url.trimmed();
    if (trimmed.isEmpty())
        return;

    QVariantMap extra;
    extra.insert("value", trimmed);
    executeEditorCommand("insert-link", extra);
}

void MainWindow::insertCodeBlock()
{
    executeEditorCommand("code-block");
}

void MainWindow::insertUnorderedList()
{
    executeEditorCommand("unordered-list");
}

void MainWindow::updateOutline()
{
    if (!outlineList)
        return;

    const QSignalBlocker blocker(outlineList);
    outlineList->clear();

    const QString content = currentMarkdown;
    if (content.trimmed().isEmpty()) {
        auto *placeholder = new QListWidgetItem("暂无标题", outlineList);
        placeholder->setFlags(Qt::NoItemFlags);
        return;
    }

    const QStringList lines = content.split('\n');
    int headingCount = 0;

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (!trimmed.startsWith('#')) {
            continue;
        }

        int level = 0;
        while (level < trimmed.size() && trimmed.at(level) == '#') {
            ++level;
        }

        QString title = trimmed.mid(level).trimmed();
        if (title.isEmpty()) {
            continue;
        }

        QString indent = (level > 1) ? QString((level - 1) * 2, ' ') : QString();
        auto *item = new QListWidgetItem(indent + title, outlineList);
        item->setData(Qt::UserRole, title);
        item->setData(Qt::UserRole + 1, level);
        ++headingCount;
    }

    if (outlineList->count() == 0) {
        auto *placeholder = new QListWidgetItem("暂无标题", outlineList);
        placeholder->setFlags(Qt::NoItemFlags);
        return;
    }

    outlineList->setCurrentRow(0);
}

void MainWindow::focusOutlineItem(QListWidgetItem *item)
{
    if (!item)
        return;

    const QString title = item->data(Qt::UserRole).toString();
    if (title.isEmpty())
        return;

    QVariantMap extra;
    extra.insert("value", title);
    extra.insert("level", item->data(Qt::UserRole + 1).toInt());
    executeEditorCommand("scroll-to-heading", extra);
}

void MainWindow::refreshNotebookList()
{
    if (!fileTreeWidget)
        return;

    const QSignalBlocker blocker(fileTreeWidget);
    fileTreeWidget->clear();

    auto addPlaceholder = [this](const QString &text) {
        QTreeWidgetItem *placeholder = new QTreeWidgetItem(fileTreeWidget, QStringList(text));
        placeholder->setFlags(Qt::NoItemFlags);
    };

    if (notebookCurrentPath.isEmpty()) {
        addPlaceholder("暂无内容");
        return;
    }

    QDir dir(notebookCurrentPath);
    if (!dir.exists()) {
        addPlaceholder("路径不存在");
        return;
    }

    QString rootLabel = QFileInfo(dir.absolutePath()).fileName();
    if (rootLabel.isEmpty())
        rootLabel = dir.absolutePath();

    QTreeWidgetItem *rootItem = new QTreeWidgetItem(fileTreeWidget, QStringList(rootLabel));
    rootItem->setData(0, Qt::UserRole, dir.absolutePath());
    rootItem->setData(0, Qt::UserRole + 1, true);
    populateFileTree(rootItem, dir.absolutePath());
    rootItem->setExpanded(true);
    fileTreeWidget->setCurrentItem(rootItem);
}

void MainWindow::populateFileTree(QTreeWidgetItem *parentItem, const QString &path)
{
    if (!parentItem)
        return;

    QDir dir(path);
    if (!dir.exists())
        return;

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst | QDir::Name);
    const QFileInfoList entries = dir.entryInfoList();
    for (const QFileInfo &info : entries) {
        QTreeWidgetItem *item = new QTreeWidgetItem(parentItem, QStringList(info.fileName()));
        item->setData(0, Qt::UserRole, info.absoluteFilePath());
        item->setData(0, Qt::UserRole + 1, info.isDir());
        if (info.isDir()) {
            populateFileTree(item, info.absoluteFilePath());
        }
    }
}

void MainWindow::handleFileTreeItem(QTreeWidgetItem *item)
{
    if (!item)
        return;

    const QString path = item->data(0, Qt::UserRole).toString();
    if (path.isEmpty())
        return;

    const bool isDir = item->data(0, Qt::UserRole + 1).toBool();
    QFileInfo info(path);
    if (isDir) {
        item->setExpanded(!item->isExpanded());
        return;
    }

    loadDocumentFromFile(info.absoluteFilePath(), false);
}

void MainWindow::showFileTreeContextMenu(const QPoint &pos)
{
    if (!fileTreeWidget)
        return;

    QTreeWidgetItem *item = fileTreeWidget->itemAt(pos);
    if (!item)
        return;

    const QString path = item->data(0, Qt::UserRole).toString();
    const bool isDir = item->data(0, Qt::UserRole + 1).toBool();
    if (path.isEmpty())
        return;

    QMenu menu(this);
    QAction *deleteFileAction = menu.addAction("删除文件");
    deleteFileAction->setEnabled(!isDir);
    QAction *deleteFolderAction = menu.addAction("删除文件夹");
    deleteFolderAction->setEnabled(isDir);

    QAction *selected = menu.exec(fileTreeWidget->viewport()->mapToGlobal(pos));
    if (!selected)
        return;

    if (selected == deleteFileAction && !isDir) {
        deleteFileAtPath(path);
    } else if (selected == deleteFolderAction && isDir) {
        deleteFolderAtPath(path);
    }
}

static QString normalizePath(const QString &path)
{
    QFileInfo info(path);
    QString canonical = info.canonicalFilePath();
    if (!canonical.isEmpty())
        return QDir::cleanPath(canonical);
    return QDir::cleanPath(info.absoluteFilePath());
}

void MainWindow::deleteFileAtPath(const QString &filePath)
{
    if (filePath.isEmpty())
        return;

    const QString normalized = normalizePath(filePath);
    const QString prompt = QString("确定要删除文件吗？\n%1\n此操作无法撤销。").arg(normalized);
    const QMessageBox::StandardButton ret = QMessageBox::warning(
        this,
        "删除文件",
        prompt,
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel
    );
    if (ret != QMessageBox::Yes)
        return;

    QFile file(normalized);
    if (!file.exists()) {
        QMessageBox::information(this, "提示", "文件不存在，可能已被移动。");
        refreshNotebookList();
        return;
    }

    if (!file.remove()) {
        QMessageBox::warning(this, "删除失败", "无法删除该文件，请检查权限。");
        return;
    }

    removeHistoryEntry(normalized, false);

    if (!currentFilePath.isEmpty() && normalizePath(currentFilePath) == normalized) {
        resetToNewDocument();
    } else {
        refreshNotebookList();
    }

    statusBar()->showMessage(QString("已删除文件: %1").arg(normalized), 2000);
}

void MainWindow::deleteFolderAtPath(const QString &folderPath)
{
    if (folderPath.isEmpty())
        return;

    const QString normalized = normalizePath(folderPath);
    QDir dir(normalized);
    if (!dir.exists()) {
        QMessageBox::information(this, "提示", "目标文件夹不存在，可能已被移动。");
        refreshNotebookList();
        return;
    }

    const QString prompt = QString("确定要删除整个文件夹吗？\n%1\n此操作会永久删除其中所有内容。").arg(normalized);
    const QMessageBox::StandardButton ret = QMessageBox::warning(
        this,
        "删除文件夹",
        prompt,
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel
    );
    if (ret != QMessageBox::Yes)
        return;

    if (!dir.removeRecursively()) {
        QMessageBox::warning(this, "删除失败", "无法删除该文件夹，请检查权限或是否被占用。");
        return;
    }

    removeHistoryEntry(normalized, true);

    auto affectsPath = [normalized](const QString &target) {
        if (target.isEmpty())
            return false;
        const QString cleanedTarget = normalizePath(target);
        return cleanedTarget == normalized
            || cleanedTarget.startsWith(normalized + QDir::separator());
    };

    if (affectsPath(currentFilePath)) {
        resetToNewDocument();
    }

    if (affectsPath(notebookCurrentPath)) {
        QFileInfo info(normalized);
        notebookCurrentPath = info.dir().absolutePath();
        if (notebookCurrentPath == normalized)
            notebookCurrentPath.clear();
    }

    refreshNotebookList();
    statusBar()->showMessage(QString("已删除文件夹: %1").arg(normalized), 2000);
}

void MainWindow::removeHistoryEntry(const QString &path, bool isFolder)
{
    const QString normalized = normalizePath(path);
    if (normalized.isEmpty())
        return;

    QSettings settings("md-editor", "app");
    if (isFolder) {
        if (recentFolders.removeAll(normalized) > 0) {
            settings.setValue(kRecentFoldersKey, recentFolders);
        }
    } else {
        if (recentFiles.removeAll(normalized) > 0) {
            settings.setValue(kRecentFilesKey, recentFiles);
        }
    }
}

void MainWindow::selectFileTreePath(const QString &path)
{
    if (!fileTreeWidget || path.isEmpty())
        return;

    const QString normalizedTarget = normalizePath(path);
    if (normalizedTarget.isEmpty())
        return;

    QList<QTreeWidgetItem *> stack;
    for (int i = 0; i < fileTreeWidget->topLevelItemCount(); ++i) {
        stack.append(fileTreeWidget->topLevelItem(i));
    }

    QTreeWidgetItem *found = nullptr;
    while (!stack.isEmpty()) {
        QTreeWidgetItem *item = stack.takeLast();
        const QString itemPath = normalizePath(item->data(0, Qt::UserRole).toString());
        if (itemPath == normalizedTarget) {
            found = item;
            break;
        }
        for (int i = 0; i < item->childCount(); ++i) {
            stack.append(item->child(i));
        }
    }

    if (!found)
        return;

    QTreeWidgetItem *parent = found->parent();
    while (parent) {
        parent->setExpanded(true);
        parent = parent->parent();
    }

    fileTreeWidget->setCurrentItem(found);
    fileTreeWidget->scrollToItem(found, QAbstractItemView::PositionAtCenter);
}

void MainWindow::createNewSidebarFile()
{
    QString targetDir = notebookCurrentPath;
    if (targetDir.isEmpty())
        targetDir = lastDirectory.isEmpty() ? QDir::homePath() : lastDirectory;

    bool ok = false;
    QString fileName = QInputDialog::getText(
        this,
        "新建 Markdown 文件",
        "文件名：",
        QLineEdit::Normal,
        "未命名.md",
        &ok
    );

    if (!ok)
        return;

    fileName = fileName.trimmed();
    if (fileName.isEmpty())
        return;
    if (!fileName.endsWith(".md", Qt::CaseInsensitive))
        fileName += ".md";

    QDir dir(targetDir);
    if (!dir.exists())
        dir.mkpath(".");

    const QString fullPath = dir.filePath(fileName);
    if (QFile::exists(fullPath)) {
        QMessageBox::warning(this, "文件已存在", "目标文件已存在，请使用其他文件名。");
        return;
    }

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "创建失败", "无法写入文件，请检查权限。");
        return;
    }
    file.close();

    notebookCurrentPath = dir.absolutePath();
    refreshNotebookList();
    loadDocumentFromFile(fullPath);
    statusBar()->showMessage(QString("已创建文件: %1").arg(fullPath), 2000);
}

void MainWindow::selectThemeAction(ThemeId id)
{
    auto setChecked = [](QAction *action, bool checked) {
        if (action) action->setChecked(checked);
    };

    setChecked(lightThemeAction, id == ThemeId::Light);
    setChecked(darkThemeAction, id == ThemeId::Dark);
    setChecked(solarizedThemeAction, id == ThemeId::Solarized);
    setChecked(elegantThemeAction, id == ThemeId::Elegant);
    setChecked(forestThemeAction, id == ThemeId::Forest);
    setChecked(sakuraThemeAction, id == ThemeId::Sakura);
}

QString MainWindow::showOpenFileDialog()
{
    QFileDialog dialog(this, "打开文件", lastDirectory.isEmpty() ? QDir::homePath() : lastDirectory);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setNameFilters({
        "Markdown (*.md)",
        "Text (*.txt)",
        "All Files (*)"
    });
    if (!useNativeFileDialogs) {
        dialog.setOption(QFileDialog::DontUseNativeDialog, true);
        dialog.resize(720, 480);
        applyDialogTheme(&dialog);
    }

    if (dialog.exec() == QDialog::Accepted) {
        const QString file = dialog.selectedFiles().value(0);
        if (!file.isEmpty()) {
            lastDirectory = QFileInfo(file).absolutePath();
        }
        return file;
    }
    return {};
}

QString MainWindow::showOpenDirectoryDialog()
{
    QFileDialog dialog(this, "选择文件夹", lastDirectory.isEmpty() ? QDir::homePath() : lastDirectory);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (!useNativeFileDialogs) {
        dialog.setOption(QFileDialog::DontUseNativeDialog, true);
        dialog.resize(720, 480);
        applyDialogTheme(&dialog);
    }

    if (dialog.exec() == QDialog::Accepted) {
        const QString dir = dialog.selectedFiles().value(0);
        if (!dir.isEmpty()) {
            lastDirectory = dir;
        }
        return dir;
    }
    return {};
}

QString MainWindow::showSaveFileDialog(const QString &title)
{
    QString initialDir;
    QString defaultName = "未命名.md";
    if (!currentFilePath.isEmpty()) {
        QFileInfo info(currentFilePath);
        initialDir = info.absolutePath();
        defaultName = info.fileName();
    } else {
        initialDir = lastDirectory.isEmpty() ? QDir::homePath() : lastDirectory;
    }

    QFileDialog dialog(this, title, initialDir);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("md");
    dialog.setNameFilters({
        "Markdown (*.md)",
        "Text (*.txt)",
        "All Files (*)"
    });
    dialog.selectFile(defaultName);
    if (!useNativeFileDialogs) {
        dialog.setOption(QFileDialog::DontUseNativeDialog, true);
        dialog.resize(720, 480);
        applyDialogTheme(&dialog);
    }

    if (dialog.exec() == QDialog::Accepted) {
        const QString file = dialog.selectedFiles().value(0);
        if (!file.isEmpty()) {
            lastDirectory = QFileInfo(file).absolutePath();
        }
        return file;
    }
    return {};
}

QMessageBox::StandardButton MainWindow::showUnsavedWarning()
{
    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle("未保存的更改");
    box.setTextFormat(Qt::RichText);
    box.setText("<b>检测到未保存的内容</b><br/>你可以立即保存、放弃或取消操作。");
    box.setInformativeText("请选择你希望执行的操作：");

    QPushButton *saveBtn    = box.addButton("保存", QMessageBox::AcceptRole);
    QPushButton *discardBtn = box.addButton("放弃", QMessageBox::DestructiveRole);
    QPushButton *cancelBtn  = box.addButton("取消", QMessageBox::RejectRole);

    applyDialogTheme(&box);

    box.exec();
    if (box.clickedButton() == saveBtn)    return QMessageBox::Save;
    if (box.clickedButton() == discardBtn) return QMessageBox::Discard;
    if (box.clickedButton() == cancelBtn)  return QMessageBox::Cancel;
    return QMessageBox::Cancel;
}

void MainWindow::applyDialogTheme(QWidget *dialog)
{
    if (!dialog)
        return;

    dialog->setStyleSheet(themeStylesheet(currentTheme));
}

void MainWindow::pushMarkdownToEditor()
{
    if (!webView)
        return;

    if (!editorReady) {
        pendingMarkdown = currentMarkdown;
        return;
    }

    const QString literal = jsonStringLiteral(currentMarkdown);
    const QString script = QStringLiteral("window.TyporaEditor && TyporaEditor.setMarkdown(%1);").arg(literal);
    webView->page()->runJavaScript(script);
    pendingMarkdown.clear();
}

void MainWindow::applyEditorTheme()
{
    const ThemePalette palette = themePalette(currentTheme);
    QVariantMap payload;
    payload.insert("bodyBg", palette.bodyBackground);
    payload.insert("pageBg", palette.pageBackground);
    payload.insert("textColor", palette.textColor);
    payload.insert("mutedColor", palette.mutedTextColor);
    payload.insert("accentColor", palette.accentColor);
    payload.insert("borderColor", palette.borderColor);
    payload.insert("inlineCodeBg", palette.inlineCodeBackground);
    payload.insert("inlineCodeColor", palette.inlineCodeColor);
    payload.insert("codeBlockBg", palette.codeBlockBackground);
    payload.insert("codeBlockText", palette.codeBlockText);
    payload.insert("blockquoteBg", palette.blockquoteBackground);
    payload.insert("blockquoteBorder", palette.blockquoteBorder);
    payload.insert("blockquoteColor", palette.blockquoteText);
    payload.insert("selectionBg", palette.selectionBackground);
    payload.insert("tableHeadBg", palette.tableHeaderBackground);
    payload.insert("tableHeadColor", palette.tableHeaderText);
    payload.insert("linkUnderline", palette.linkUnderlineColor);
    pendingEditorTheme = payload;

    if (!editorReady || !webView || !webView->page())
        return;

    const QString json = QString::fromUtf8(QJsonDocument::fromVariant(pendingEditorTheme).toJson(QJsonDocument::Compact));
    const QString script = QStringLiteral("window.TyporaEditor && TyporaEditor.applyTheme(%1);").arg(json);
    webView->page()->runJavaScript(script);
    pendingEditorTheme.clear();
    applyEditorPlainMode();
}

void MainWindow::applyEditorPlainMode()
{
    if (!webView || !editorReady || !webView->page())
        return;

    const QString script = QStringLiteral("window.TyporaEditor && TyporaEditor.setPlainMode(%1);")
        .arg(currentPlainTextMode ? "true" : "false");
    webView->page()->runJavaScript(script);
}

void MainWindow::setPlainTextMode(bool enabled)
{
    if (currentPlainTextMode == enabled)
        return;
    currentPlainTextMode = enabled;
    applyEditorPlainMode();
}

void MainWindow::executeEditorCommand(const QString &command, const QVariantMap &extra)
{
    if (!webView || command.isEmpty())
        return;

    QVariantMap payload = extra;
    payload.insert("command", command);
    const QString json = QString::fromUtf8(QJsonDocument::fromVariant(payload).toJson(QJsonDocument::Compact));
    const QString script = QStringLiteral("window.TyporaEditor && TyporaEditor.execCommand(%1);").arg(json);
    webView->page()->runJavaScript(script);
}

void MainWindow::updateActiveDocumentLabel()
{
    if (!activeDocLabel)
        return;

    QString display = currentFilePath.isEmpty()
        ? QStringLiteral("文档库 / 未命名.md")
        : QStringLiteral("文档库 / %1").arg(QFileInfo(currentFilePath).fileName());
    activeDocLabel->setText(display);
}

void MainWindow::applyHeadingLevel(int level)
{
    if (level < 1)
        level = 1;
    if (level > 6)
        level = 6;
    QVariantMap extra;
    extra.insert("level", level);
    executeEditorCommand("heading", extra);
}

void MainWindow::toggleFocusMode(bool enabled)
{
    if (!sideContainer)
        return;

    sideContainer->setVisible(!enabled);
}

void MainWindow::openOutlineFromMenu()
{
    if (focusModeAction && focusModeAction->isChecked()) {
        focusModeAction->setChecked(false);
        toggleFocusMode(false);
    }
    if (sideTabGroup) {
        if (QAbstractButton *btn = sideTabGroup->button(1)) {
            btn->setChecked(true);
        }
    }
    if (sideStack) {
        sideStack->setVisible(true);
        sideStack->setCurrentIndex(1);
    }
}

void MainWindow::showShortcutHelp()
{
    QMessageBox::information(
        this,
        "快捷键参考",
        "<b>常用快捷键</b><br/>"
        "Ctrl+N 新建<br/>"
        "Ctrl+O 打开<br/>"
        "Ctrl+S 保存<br/>"
        "Ctrl+B 加粗 / Ctrl+I 斜体<br/>"
        "Ctrl+` 代码 / Ctrl+Shift+` 代码块"
    );
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(
        this,
        "关于 Md Editor",
        "Md Editor\nTypora 风格所见即所得编辑器\n基于 Qt WebEngine + 自定义 Markdown 引擎。"
    );
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (promptSaveIfDirty()) {
        event->accept();
    } else {
        event->ignore();
    }
}

QString MainWindow::jsonStringLiteral(const QString &text) const
{
    QJsonArray wrapper;
    wrapper.append(text);
    QString json = QString::fromUtf8(QJsonDocument(wrapper).toJson(QJsonDocument::Compact));
    if (json.size() >= 2) {
        return json.mid(1, json.size() - 2); // strip '[' and ']'
    }
    return QStringLiteral("\"\"");
}

int MainWindow::calculateWordCount(const QString &text) const
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
