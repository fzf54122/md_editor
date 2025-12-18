#pragma once

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QVariant>
#include <QStringList>

#include "theme/Theme.h"

class QAction;
class QMenu;
class QLabel;
class QCloseEvent;
class QListWidget;
class QListWidgetItem;
class QFileDialog;
class QWidget;
class QToolButton;
class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;
class QStackedWidget;
class QLineEdit;
class QPoint;
class QButtonGroup;
class QHBoxLayout;
class QWebEngineView;
class QWebChannel;
class EditorBridge;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    enum class StatsDisplayMode {
        Minutes,
        Lines,
        Words,
        Characters
    };

private:
    void setupEditor();
    void setupMenu();
    void setupConnections();
    void updateStatusInfo();
    void updateWindowTitle();
    void markDocumentClean(const QString &path);
    void markDocumentDirty();
    bool promptSaveIfDirty();
    bool saveDocument(bool forceSelectPath);
    bool writeToFile(const QString &fileName);
    void loadDocumentFromFile(QString fileName, bool updateNotebookPath = true);
    void resetToNewDocument();
    void insertLink();
    void insertCodeBlock();
    void insertUnorderedList();
    void createNewSidebarFile();
    void updateOutline();
    void focusOutlineItem(QListWidgetItem *item);
    void refreshNotebookList();
    void populateFileTree(QTreeWidgetItem *parentItem, const QString &path);
    void handleFileTreeItem(QTreeWidgetItem *item);
    void showFileTreeContextMenu(const QPoint &pos);
    void deleteFileAtPath(const QString &filePath);
    void deleteFolderAtPath(const QString &folderPath);
    void removeHistoryEntry(const QString &path, bool isFolder);
    void selectFileTreePath(const QString &path);
    void applyHeadingLevel(int level);
    void toggleFocusMode(bool enabled);
    void openOutlineFromMenu();
    void showShortcutHelp();
    void showAboutDialog();
    void updateActiveDocumentLabel();
    void selectThemeAction(ThemeId id);
    QString showOpenFileDialog();
    QString showOpenDirectoryDialog();
    QString showSaveFileDialog(const QString &title);
    QMessageBox::StandardButton showUnsavedWarning();
    void applyDialogTheme(QWidget *dialog);
    void pushMarkdownToEditor();
    void applyEditorTheme();
    void applyEditorPlainMode();
    void setPlainTextMode(bool enabled);
    void updateActionText(QAction *action, StatsDisplayMode mode);
    void executeEditorCommand(const QString &command, const QVariantMap &extra = QVariantMap());
    QString jsonStringLiteral(const QString &text) const;
    int calculateWordCount(const QString &text) const;
    void loadRecentHistory();
    void promptInitialOpen();
    void rememberRecentFile(const QString &path);
    void rememberRecentFolder(const QString &path);
    bool restoreSessionFromHistory();
    bool isMarkdownFile(const QString &path) const;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QToolButton *statsButton{};
    QMenu *statsMenu{};
    QLabel *statsMinutesLabel{};
    QLabel *statsLinesLabel{};
    QLabel *statsWordsLabel{};
    QLabel *statsCharsLabel{};
    QAction *statsMinutesAction{};
    QAction *statsLinesAction{};
    QAction *statsWordsAction{};
    QAction *statsCharsAction{};
    StatsDisplayMode statsDisplayMode{StatsDisplayMode::Words};
    QLabel *activeDocLabel{};
    QString currentFilePath{};
    bool documentDirty{false};
    ThemeId currentTheme{ThemeId::Light};
    QString currentMarkdown{};
    bool editorReady{false};
    QString pendingMarkdown{};
    int currentCursorLine{1};
    QVariantMap pendingEditorTheme;
    bool currentPlainTextMode{false};

    QWebEngineView *webView{};
    QWebChannel *webChannel{};
    EditorBridge *editorBridge{};

    QWidget *centralContainer{};
    QWidget *sideContainer{};
    QWidget *sideRail{};
    QStackedWidget *sideStack{};
    QButtonGroup *sideTabGroup{};
    QToolButton *notebookTabBtn{};
    QToolButton *outlineTabBtn{};
    QTreeWidget *fileTreeWidget{};
    QListWidget *outlineList{};
    QPushButton *newFileButton{};

    QAction *newAction{};
    QAction *openAction{};
    QAction *openFolderAction{};
    QAction *saveAction{};
    QAction *saveAsAction{};
    QAction *exitAction{};
    QAction *boldAction{};
    QAction *italicAction{};
    QAction *codeAction{};
    QAction *codeBlockAction{};
    QAction *linkAction{};
    QAction *unorderedListAction{};
    QAction *undoAction{};
    QAction *redoAction{};
    QAction *cutAction{};
    QAction *copyAction{};
    QAction *pasteAction{};
    QAction *selectAllAction{};
    QAction *heading1Action{};
    QAction *heading2Action{};
    QAction *heading3Action{};
    QAction *blockquoteAction{};
    QAction *focusModeAction{};
    QAction *outlineViewAction{};
    QAction *shortcutsAction{};
    QAction *aboutAction{};
    QAction *lightThemeAction{};
    QAction *darkThemeAction{};
    QAction *solarizedThemeAction{};
    QAction *elegantThemeAction{};
    QAction *forestThemeAction{};
    QAction *sakuraThemeAction{};

    QMenu *fileMenu{};
    QMenu *editMenu{};
    QMenu *paragraphMenu{};
    QMenu *formatMenu{};
    QMenu *viewMenu{};
    QMenu *themeMenu{};
    QMenu *helpMenu{};

    QString lastDirectory;
    bool useNativeFileDialogs{true};
    QString notebookCurrentPath;
    QStringList recentFiles;
    QStringList recentFolders;
};
