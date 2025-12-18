#include "MainWindow.h"

#include <QApplication>  
#include <QtGlobal>

#include <QTextEdit>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSplitter>
#include <QTextBrowser>
#include <QTimer>
#include <QLabel>
#include <QScrollBar>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextCursor>
#include <QTextBlock>
#include <QDebug>
#include <QCloseEvent>
#include <QSignalBlocker>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QTextDocument>
#include <QSettings>

#include "theme/Theme.h"

static QColor CURRENT_BLOCK_BG = QColor(60, 60, 60, 80);

// 段落背景清空
void clearAllBlockBackground(QTextEdit *editor)
{
    QTextDocument *doc = editor->document();
    QTextCursor cursor(doc);

    cursor.beginEditBlock();

    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        QTextCursor c(block);
        c.select(QTextCursor::BlockUnderCursor);

        QTextCharFormat fmt;
        fmt.setBackground(Qt::NoBrush);
        c.mergeCharFormat(fmt);
    }

    cursor.endEditBlock();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1080, 680);
    setWindowTitle("Md Editor");

    renderTimer = new QTimer(this);
    renderTimer->setSingleShot(true);
    connect(renderTimer, &QTimer::timeout, this, &MainWindow::renderPreview);

    statusInfoLabel = new QLabel(this);
    statusInfoLabel->setText("行 1 列 1 | 字数 0");
    statusBar()->addPermanentWidget(statusInfoLabel);
    syncingScroll = false;

    setupEditor();
    QSettings settings("md-editor", "app");
    const ThemeId theme = static_cast<ThemeId>(settings.value("themeId", static_cast<int>(ThemeId::Light)).toInt());
    qApp->setStyleSheet(themeStylesheet(theme));
    currentTheme = theme;

    setupMenu();
    setupConnections();

    markDocumentClean(QString{});
    renderPreview();
    updateStatusInfo();
    statusBar()->showMessage("就绪");
}

void MainWindow::setupEditor()
{
    splitter = new QSplitter(Qt::Horizontal, this);

    editor = new QTextEdit(splitter);
    // editor->setPlainText("Hello Markdown Editor");
    editor->setPlaceholderText("在此输入 Markdown 内容...");
    editor->setStyleSheet(
        "QTextEdit {"
        "  padding: 12px;"
        "  font-size: 14px;"
        "  font-family: 'JetBrains Mono', 'Consolas', monospace;"
        "}"
    );

    preview = new QTextBrowser(splitter);
    preview->setOpenExternalLinks(true);
    preview->setHtml(
        "<h3>预览区域</h3>"
        "<p>Markdown 渲染将在下一阶段接入。</p>"
    );

    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    setCentralWidget(splitter);
}

void MainWindow::setupMenu()
{
    QMenuBar *menuBar = this->menuBar();

    QMenu *fileMenu = menuBar->addMenu("文件");
    newAction = fileMenu->addAction("新建");
    openAction = fileMenu->addAction("打开");
    saveAction = fileMenu->addAction("保存");
    saveAsAction = fileMenu->addAction("另存为");
    exitAction = fileMenu->addAction("退出");

    QMenu *themeMenu = menuBar->addMenu("主题");
    lightThemeAction = themeMenu->addAction("浅色");
    darkThemeAction = themeMenu->addAction("深色");

    QMenu *formatMenu = menuBar->addMenu("格式");
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
}

void MainWindow::setupConnections()
{
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    connect(newAction, &QAction::triggered, this, [this]() {
        if (!promptSaveIfDirty())
            return;
        resetToNewDocument();
        statusBar()->showMessage("已创建新文档", 2000);
    });

    connect(openAction, &QAction::triggered, this, [this]() {
        if (!promptSaveIfDirty())
            return;

        QString fileName = QFileDialog::getOpenFileName(
            this, "打开文件", "",
            "Markdown (*.md);;Text (*.txt);;All Files (*)"
        );

        if (fileName.isEmpty()) return;

        loadDocumentFromFile(fileName);
    });

    connect(saveAction, &QAction::triggered, this, [this]() {
        saveDocument(false);
    });

    connect(saveAsAction, &QAction::triggered, this, [this]() {
        saveDocument(true);
    });

    connect(editor, &QTextEdit::cursorPositionChanged, this, [this]() {
        // 1) 清理所有段落背景
        clearAllBlockBackground(editor);

        // 2) 选中当前段落并设置背景
        QTextCursor c = editor->textCursor();
        c.select(QTextCursor::BlockUnderCursor);

        QTextCharFormat fmt;
        fmt.setBackground(CURRENT_BLOCK_BG);
        c.mergeCharFormat(fmt);

        updateStatusInfo();
    });

    connect(editor, &QTextEdit::textChanged, this, [this]() {
        // 节流渲染，防止频繁解析 Markdown
        renderTimer->start(180);
        markDocumentDirty();
        updateStatusInfo();
    });

    connect(boldAction, &QAction::triggered, this, [this]() {
        applyInlineFormat("**");
    });

    connect(italicAction, &QAction::triggered, this, [this]() {
        applyInlineFormat("*");
    });

    connect(codeAction, &QAction::triggered, this, [this]() {
        applyInlineFormat("`", "`");
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

    QScrollBar *editorScrollBar = editor->verticalScrollBar();
    QScrollBar *previewScrollBar = preview->verticalScrollBar();

    connect(editorScrollBar, &QScrollBar::valueChanged, this, [this, editorScrollBar, previewScrollBar](int value) {
        if (syncingScroll) return;
        syncingScroll = true;

        const int editorMax = editorScrollBar->maximum();
        const int previewMax = previewScrollBar->maximum();
        const int target = (editorMax > 0)
            ? static_cast<int>((static_cast<double>(value) / editorMax) * previewMax)
            : 0;
        previewScrollBar->setValue(target);

        syncingScroll = false;
    });

    connect(previewScrollBar, &QScrollBar::valueChanged, this, [this, editorScrollBar, previewScrollBar](int value) {
        if (syncingScroll) return;
        syncingScroll = true;

        const int previewMax = previewScrollBar->maximum();
        const int editorMax = editorScrollBar->maximum();
        const int target = (previewMax > 0)
            ? static_cast<int>((static_cast<double>(value) / previewMax) * editorMax)
            : 0;
        editorScrollBar->setValue(target);

        syncingScroll = false;
    });

    // 主题菜单
    auto applyTheme = [this](ThemeId id) {
        currentTheme = id;
        qApp->setStyleSheet(themeStylesheet(currentTheme));
        QSettings settings("md-editor", "app");
        settings.setValue("themeId", static_cast<int>(currentTheme));
    };

    connect(lightThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Light);
    });

    connect(darkThemeAction, &QAction::triggered, this, [applyTheme]() {
        applyTheme(ThemeId::Dark);
    });
}

void MainWindow::renderPreview()
{
    if (!preview) return;

    const QString text = editor ? editor->toPlainText() : QString{};
    if (text.trimmed().isEmpty()) {
        preview->setHtml(
            "<h3>预览区域</h3>"
            "<p>开始输入 Markdown 内容以查看实时预览。</p>"
        );
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    preview->setMarkdown(text);
#else
    preview->setPlainText(text);
#endif

    updateStatusInfo();
}

void MainWindow::updateStatusInfo()
{
    if (!statusInfoLabel || !editor)
        return;

    QTextCursor cursor = editor->textCursor();
    const int line = cursor.blockNumber() + 1;
    const int column = cursor.positionInBlock() + 1;

    const QString content = editor->toPlainText();
    int wordCount = 0;
    bool inWord = false;
    for (const QChar &ch : content) {
        if (!ch.isSpace()) {
            if (!inWord) {
                ++wordCount;
                inWord = true;
            }
        } else {
            inWord = false;
        }
    }

    statusInfoLabel->setText(
        QString("行 %1 列 %2 | 字数 %3")
            .arg(line)
            .arg(column)
            .arg(wordCount)
    );
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

    QMessageBox::StandardButton ret = QMessageBox::warning(
        this,
        "未保存的更改",
        "当前文档还有未保存的内容，是否现在保存？",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );

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
        const QString fileName = QFileDialog::getSaveFileName(
            this, forceSelectPath ? "另存为" : "保存文件", targetPath,
            "Markdown (*.md);;Text (*.txt);;All Files (*)"
        );
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
    out << editor->toPlainText();
    return true;
}

void MainWindow::loadDocumentFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        return;
    }

    QTextStream in(&file);
    const QString content = in.readAll();

    {
        QSignalBlocker blocker(editor);
        editor->setPlainText(content);
    }

    markDocumentClean(fileName);
    renderPreview();
    updateStatusInfo();
    statusBar()->showMessage(QString("已打开: %1").arg(fileName), 2000);
}

void MainWindow::resetToNewDocument()
{
    {
        QSignalBlocker blocker(editor);
        editor->clear();
    }

    markDocumentClean(QString{});
    renderPreview();
    updateStatusInfo();
}

void MainWindow::applyInlineFormat(const QString &prefix, const QString &suffix)
{
    if (!editor)
        return;

    const QString actualSuffix = suffix.isEmpty() ? prefix : suffix;
    QTextCursor cursor = editor->textCursor();

    if (!cursor.hasSelection()) {
        cursor.insertText(prefix + actualSuffix);
        const int move = actualSuffix.size();
        if (move > 0) {
            cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, move);
            editor->setTextCursor(cursor);
        }
        return;
    }

    const QString selected = cursor.selectedText();
    cursor.insertText(prefix + selected + actualSuffix);
}

void MainWindow::insertLink()
{
    if (!editor)
        return;

    QTextCursor cursor = editor->textCursor();
    const QString label = cursor.hasSelection()
        ? cursor.selectedText()
        : QStringLiteral("链接文本");

    bool ok = false;
    const QString url = QInputDialog::getText(
        this,
        "插入链接",
        "URL:",
        QLineEdit::Normal,
        "https://",
        &ok
    );

    if (!ok || url.trimmed().isEmpty())
        return;

    cursor.insertText(QString("[%1](%2)").arg(label, url.trimmed()));
}

void MainWindow::insertCodeBlock()
{
    if (!editor)
        return;

    QTextCursor cursor = editor->textCursor();
    const QString content = cursor.hasSelection()
        ? cursor.selectedText()
        : QStringLiteral("代码内容");

    QString block = QString("\n```\n%1\n```\n").arg(content);
    cursor.insertText(block);
}

void MainWindow::insertUnorderedList()
{
    if (!editor)
        return;

    QTextCursor selectionCursor = editor->textCursor();
    QTextDocument *doc = editor->document();

    if (!selectionCursor.hasSelection()) {
        QTextCursor lineCursor = selectionCursor;
        lineCursor.movePosition(QTextCursor::StartOfBlock);
        lineCursor.insertText("- ");
        return;
    }

    QTextBlock startBlock = doc->findBlock(selectionCursor.selectionStart());
    QTextBlock endBlock = doc->findBlock(selectionCursor.selectionEnd());

    QTextBlock block = startBlock;
    QTextCursor editCursor(doc);
    editCursor.beginEditBlock();

    while (block.isValid()) {
        QTextCursor lineCursor(block);
        lineCursor.movePosition(QTextCursor::StartOfBlock);
        lineCursor.insertText("- ");

        if (block == endBlock)
            break;
        block = block.next();
    }

    editCursor.endEditBlock();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (promptSaveIfDirty()) {
        event->accept();
    } else {
        event->ignore();
    }
}
