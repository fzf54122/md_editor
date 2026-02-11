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
#include <QStringList>
#include <QUrl>
#include <QTimer>
#include <QWebEnginePage>
#include <QWidgetAction>

#include "theme/Theme.h"
#include "EditorBridge.h"

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

    statsPanel = new StatsPanel(this);
    statusBar()->addPermanentWidget(statsPanel->button());

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
    currentMarkdownMode = markdownModeFromKey(settings.value("markdownMode", QStringLiteral("typora")).toString());
    qApp->setStyleSheet(themeStylesheet(currentTheme));

    setupMenu();
    menuBar()->setVisible(false);
    setupEditor();
    setupConnections();
    applyEditorTheme();
    applyEditorMarkdownMode();
    selectThemeAction(currentTheme);
    selectMarkdownModeAction(currentMarkdownMode);

    recentHistory.load();
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

    notebookPane = new NotebookPane(bodyWidget);
    fileTreeWidget = notebookPane->fileTreeWidget();
    outlineList = notebookPane->outlineListWidget();
    activeDocLabel = notebookPane->activeDocLabelWidget();

    connect(notebookPane, &NotebookPane::fileItemActivated, this, &MainWindow::handleFileTreeItem);
    connect(notebookPane, &NotebookPane::fileContextMenuRequested, this, &MainWindow::showFileTreeContextMenu);
    connect(notebookPane, &NotebookPane::newFileRequested, this, &MainWindow::createNewSidebarFile);
    connect(notebookPane, &NotebookPane::outlineItemActivated, this, [this](QListWidgetItem *item) {
        focusOutlineItem(item);
    });

    editorPane = new EditorPane(bodyWidget);

    connect(editorPane, &EditorPane::contentChanged, this, [this](const QString &markdown) {
        currentMarkdown = markdown;
        markDocumentDirty();
        updateStatusInfo();
        updateOutline();
    });

    connect(editorPane, &EditorPane::initialReady, this, [this]() {
        applyEditorTheme();
        applyEditorMarkdownMode();
        applyEditorPlainMode();
        pushMarkdownToEditor();
    });

    connect(editorPane, &EditorPane::cursorStateChanged, this, [this](int line, int words) {
        if (line < 1)
            line = 1;
        if (words < 0)
            words = 0;
        currentCursorLine = line;
        Q_UNUSED(words);
        updateStatusInfo();
        
    });

    bodyLayout->addWidget(notebookPane);
    bodyLayout->addWidget(editorPane, 1);

    rootLayout->addWidget(bodyWidget, 1);
    setCentralWidget(centralContainer);

    notebookCurrentPath.clear();
    refreshNotebookList();
}

void MainWindow::setupMenu()
{
    menuActions = MenuBuilder::build(this);
}

void MainWindow::setupConnections()
{
    connect(menuActions.exitAction, &QAction::triggered, this, &QWidget::close);

    auto triggerAction = [this](QWebEnginePage::WebAction action) {
        if (editorPane)
            editorPane->triggerPageAction(action);
    };

    connect(menuActions.undoAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Undo);
    });
    connect(menuActions.redoAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Redo);
    });
    connect(menuActions.cutAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Cut);
    });
    connect(menuActions.copyAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Copy);
    });
    connect(menuActions.pasteAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::Paste);
    });
    connect(menuActions.selectAllAction, &QAction::triggered, this, [this, triggerAction]() {
        triggerAction(QWebEnginePage::SelectAll);
    });

    connect(menuActions.newAction, &QAction::triggered, this, [this]() {
        if (!promptSaveIfDirty())
            return;
        resetToNewDocument();
        statusBar()->showMessage("已创建新文档", 2000);
    });

    connect(menuActions.openAction, &QAction::triggered, this, [this]() {
        if (!promptSaveIfDirty())
            return;

        const QString fileName = showOpenFileDialog();
        if (fileName.isEmpty()) return;

        loadDocumentFromFile(fileName);
    });

    connect(menuActions.openFolderAction, &QAction::triggered, this, [this]() {
        const QString folder = showOpenDirectoryDialog();
        if (folder.isEmpty())
            return;
        notebookCurrentPath = folder;
        refreshNotebookList();
        recentHistory.rememberFolder(folder);
        statusBar()->showMessage(QString("已打开文件夹: %1").arg(folder), 2000);
    });

    connect(menuActions.saveAction, &QAction::triggered, this, [this]() {
        saveDocument(false);
    });

    connect(menuActions.saveAsAction, &QAction::triggered, this, [this]() {
        saveDocument(true);
    });

    connect(menuActions.boldAction, &QAction::triggered, this, [this]() {
        executeEditorCommand("bold");
    });

    connect(menuActions.italicAction, &QAction::triggered, this, [this]() {
        executeEditorCommand("italic");
    });

    connect(menuActions.codeAction, &QAction::triggered, this, [this]() {
        executeEditorCommand("inline-code");
    });

    connect(menuActions.codeBlockAction, &QAction::triggered, this, [this]() {
        insertCodeBlock();
    });

    connect(menuActions.linkAction, &QAction::triggered, this, [this]() {
        insertLink();
    });

    connect(menuActions.unorderedListAction, &QAction::triggered, this, [this]() {
        insertUnorderedList();
    });

    connect(menuActions.heading1Action, &QAction::triggered, this, [this]() { applyHeadingLevel(1); });
    connect(menuActions.heading2Action, &QAction::triggered, this, [this]() { applyHeadingLevel(2); });
    connect(menuActions.heading3Action, &QAction::triggered, this, [this]() { applyHeadingLevel(3); });
    connect(menuActions.blockquoteAction, &QAction::triggered, this, [this]() {
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

    connect(menuActions.lightThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Light);
    });

    connect(menuActions.darkThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Dark);
    });

    connect(menuActions.solarizedThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Solarized);
    });

    connect(menuActions.elegantThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Elegant);
    });

    connect(menuActions.forestThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Forest);
    });

    connect(menuActions.sakuraThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Sakura);
    });

    connect(menuActions.commonMarkModeAction, &QAction::triggered, this, [this]() {
        setMarkdownMode(MarkdownMode::CommonMark);
    });
    connect(menuActions.gfmModeAction, &QAction::triggered, this, [this]() {
        setMarkdownMode(MarkdownMode::Gfm);
    });
    connect(menuActions.typoraModeAction, &QAction::triggered, this, [this]() {
        setMarkdownMode(MarkdownMode::Typora);
    });

    connect(menuActions.focusModeAction, &QAction::toggled, this, &MainWindow::toggleFocusMode);
    connect(menuActions.outlineViewAction, &QAction::triggered, this, &MainWindow::openOutlineFromMenu);
    connect(menuActions.shortcutsAction, &QAction::triggered, this, &MainWindow::showShortcutHelp);
    connect(menuActions.aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::updateStatusInfo()
{
    if (statsPanel)
        statsPanel->updateStats(currentMarkdown);
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
    setPlainTextMode(!FileService::isMarkdownFile(path));
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

    QMessageBox::StandardButton ret = DialogHelper::showUnsavedWarning(this, currentTheme);

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
        const QString fileName = DialogHelper::showSaveFileDialog(this, forceSelectPath ? "另存为" : "保存文件", lastDirectory, useNativeFileDialogs, currentTheme, &lastDirectory);
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
    if (!FileService::writeTextFile(fileName, currentMarkdown)) {
        QMessageBox::warning(this, "错误", "无法保存文件");
        return false;
    }
    return true;
}

void MainWindow::loadDocumentFromFile(QString fileName, bool updateNotebookPath)
{
    QString content;
    if (!FileService::readTextFile(fileName, content)) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        return;
    }

    currentMarkdown = content;
    currentFilePath = fileName;
    setPlainTextMode(!FileService::isMarkdownFile(fileName));
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
    recentHistory.rememberFile(fileName);
    statusBar()->showMessage(QString("已打开: %1").arg(fileName), 2000);
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
        recentHistory.rememberFolder(folderPath);
        statusBar()->showMessage(QString("已打开文件夹: %1").arg(folderPath), 2000);
        return;
    }

    statusBar()->showMessage("未选择文件或文件夹", 2000);
}

bool MainWindow::restoreSessionFromHistory()
{
    QString filePath;
    QString folderPath;
    if (!recentHistory.findLatestExisting(filePath, folderPath))
        return false;

    if (!filePath.isEmpty()) {
        QFileInfo info(filePath);
        lastDirectory = info.absolutePath();
        loadDocumentFromFile(filePath);
        return true;
    }

    if (!folderPath.isEmpty()) {
        QDir dir(folderPath);
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
    NotebookManager::refreshTree(fileTreeWidget, notebookCurrentPath);
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
    return NotebookManager::normalizePath(path);
}

void MainWindow::deleteFileAtPath(const QString &filePath)
{
    QString normalized;
    const auto result = NotebookManager::deleteFile(this, filePath, recentHistory, &normalized);
    if (result == NotebookManager::DeleteResult::NotFound) {
        refreshNotebookList();
        return;
    }
    if (result != NotebookManager::DeleteResult::Deleted)
        return;

    if (!currentFilePath.isEmpty() && normalizePath(currentFilePath) == normalized) {
        resetToNewDocument();
    } else {
        refreshNotebookList();
    }

    statusBar()->showMessage(QString("已删除文件: %1").arg(normalized), 2000);
}

void MainWindow::deleteFolderAtPath(const QString &folderPath)
{
    QString normalized;
    const auto result = NotebookManager::deleteFolder(this, folderPath, recentHistory, &normalized);
    if (result == NotebookManager::DeleteResult::NotFound) {
        refreshNotebookList();
        return;
    }
    if (result != NotebookManager::DeleteResult::Deleted)
        return;

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

void MainWindow::selectFileTreePath(const QString &path)
{
    NotebookManager::selectPath(fileTreeWidget, path);
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

    setChecked(menuActions.lightThemeAction, id == ThemeId::Light);
    setChecked(menuActions.darkThemeAction, id == ThemeId::Dark);
    setChecked(menuActions.solarizedThemeAction, id == ThemeId::Solarized);
    setChecked(menuActions.elegantThemeAction, id == ThemeId::Elegant);
    setChecked(menuActions.forestThemeAction, id == ThemeId::Forest);
    setChecked(menuActions.sakuraThemeAction, id == ThemeId::Sakura);
}

void MainWindow::selectMarkdownModeAction(MarkdownMode mode)
{
    auto setChecked = [](QAction *action, bool checked) {
        if (action) action->setChecked(checked);
    };

    setChecked(menuActions.commonMarkModeAction, mode == MarkdownMode::CommonMark);
    setChecked(menuActions.gfmModeAction, mode == MarkdownMode::Gfm);
    setChecked(menuActions.typoraModeAction, mode == MarkdownMode::Typora);
}

QString MainWindow::markdownModeKey(MarkdownMode mode)
{
    switch (mode) {
    case MarkdownMode::CommonMark:
        return QStringLiteral("commonmark");
    case MarkdownMode::Gfm:
        return QStringLiteral("gfm");
    case MarkdownMode::Typora:
        return QStringLiteral("typora");
    default:
        return QStringLiteral("typora");
    }
}

MainWindow::MarkdownMode MainWindow::markdownModeFromKey(const QString &value)
{
    const QString normalized = value.trimmed().toLower();
    if (normalized == QStringLiteral("commonmark"))
        return MarkdownMode::CommonMark;
    if (normalized == QStringLiteral("gfm"))
        return MarkdownMode::Gfm;
    return MarkdownMode::Typora;
}

QString MainWindow::showOpenFileDialog()
{
    QString updatedDir = lastDirectory;
    const QString path = DialogHelper::showOpenFileDialog(this, lastDirectory, useNativeFileDialogs, currentTheme, &updatedDir);
    if (!path.isEmpty())
        lastDirectory = updatedDir;
    return path;
}

QString MainWindow::showOpenDirectoryDialog()
{
    QString updatedDir = lastDirectory;
    const QString path = DialogHelper::showOpenDirectoryDialog(this, lastDirectory, useNativeFileDialogs, currentTheme, &updatedDir);
    if (!path.isEmpty())
        lastDirectory = updatedDir;
    return path;
}

QString MainWindow::showSaveFileDialog(const QString &title)
{
    QString updatedDir = lastDirectory;
    const QString path = DialogHelper::showSaveFileDialog(this, title, lastDirectory, useNativeFileDialogs, currentTheme, &updatedDir);
    if (!path.isEmpty())
        lastDirectory = updatedDir;
    return path;
}

void MainWindow::pushMarkdownToEditor()
{
    if (editorPane)
        editorPane->setMarkdown(currentMarkdown);
}

void MainWindow::applyEditorTheme()
{
    if (editorPane)
        editorPane->applyTheme(currentTheme);
}

void MainWindow::applyEditorMarkdownMode()
{
    if (editorPane)
        editorPane->setMarkdownMode(markdownModeKey(currentMarkdownMode));
}

void MainWindow::applyEditorPlainMode()
{
    if (editorPane)
        editorPane->setPlainMode(currentPlainTextMode);
}

void MainWindow::setMarkdownMode(MarkdownMode mode)
{
    if (currentMarkdownMode == mode)
        return;

    currentMarkdownMode = mode;
    QSettings settings("md-editor", "app");
    settings.setValue("markdownMode", markdownModeKey(currentMarkdownMode));
    selectMarkdownModeAction(currentMarkdownMode);
    applyEditorMarkdownMode();

    QString modeName;
    switch (currentMarkdownMode) {
    case MarkdownMode::CommonMark:
        modeName = QStringLiteral("CommonMark");
        break;
    case MarkdownMode::Gfm:
        modeName = QStringLiteral("GFM");
        break;
    case MarkdownMode::Typora:
    default:
        modeName = QStringLiteral("Typora 扩展");
        break;
    }
    statusBar()->showMessage(QStringLiteral("已切换兼容模式: %1").arg(modeName), 2000);
}

void MainWindow::setPlainTextMode(bool enabled)
{
    if (currentPlainTextMode == enabled)
    {
        if (enabled) {
            applyPlainLanguageForPath(currentFilePath);
        }
        return;
    }
    currentPlainTextMode = enabled;
    applyPlainLanguageForPath(currentFilePath);
    applyEditorPlainMode();
}

void MainWindow::applyPlainLanguageForPath(const QString &path)
{
    if (!editorPane)
        return;

    if (!currentPlainTextMode) {
        currentPlainLanguage.clear();
        editorPane->setPlainLanguage(QString());
        return;
    }

    currentPlainLanguage = FileService::languageHintForPath(path);
    editorPane->setPlainLanguage(currentPlainLanguage);
}

void MainWindow::executeEditorCommand(const QString &command, const QVariantMap &extra)
{
    if (editorPane)
        editorPane->executeCommand(command, extra);
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
    if (!notebookPane)
        return;

    notebookPane->setVisible(!enabled);
    if (!enabled)
        notebookPane->setCollapsed(false);
}

void MainWindow::openOutlineFromMenu()
{
    if (menuActions.focusModeAction && menuActions.focusModeAction->isChecked()) {
        menuActions.focusModeAction->setChecked(false);
        toggleFocusMode(false);
    }
    if (!notebookPane)
        return;
    notebookPane->setCollapsed(false);
    notebookPane->showOutlineTab();
    notebookPane->setVisible(true);
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
