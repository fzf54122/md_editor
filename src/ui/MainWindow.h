#pragma once

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QVariant>
#include <QStringList>

#include "theme/Theme.h"
#include "services/RecentHistory.h"
#include "services/TextStatistics.h"
#include "services/NotebookManager.h"
#include "services/FileService.h"
#include "NotebookPane.h"
#include "MenuBuilder.h"
#include "StatsPanel.h"
#include "EditorPane.h"
#include "DialogHelper.h"

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

private:
    enum class MarkdownMode {
        CommonMark,
        Gfm,
        Typora
    };

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
    void handleFileTreeItem(QTreeWidgetItem *item);
    void showFileTreeContextMenu(const QPoint &pos);
    void deleteFileAtPath(const QString &filePath);
    void deleteFolderAtPath(const QString &folderPath);
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
    void applyEditorMarkdownMode();
    void setPlainTextMode(bool enabled);
    void setMarkdownMode(MarkdownMode mode);
    void selectMarkdownModeAction(MarkdownMode mode);
    static QString markdownModeKey(MarkdownMode mode);
    static MarkdownMode markdownModeFromKey(const QString &value);
    void applyPlainLanguageForPath(const QString &path);
    void executeEditorCommand(const QString &command, const QVariantMap &extra = QVariantMap());
    void promptInitialOpen();
    bool restoreSessionFromHistory();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    NotebookPane *notebookPane{};
    MenuActions menuActions{};
    StatsPanel *statsPanel{};
    QLabel *activeDocLabel{};
    QString currentFilePath{};
    bool documentDirty{false};
    ThemeId currentTheme{ThemeId::Light};
    QString currentMarkdown{};
    int currentCursorLine{1};
    bool currentPlainTextMode{false};
    QString currentPlainLanguage{};
    MarkdownMode currentMarkdownMode{MarkdownMode::Typora};

    EditorPane *editorPane{};

    QWidget *centralContainer{};
    QTreeWidget *fileTreeWidget{};
    QListWidget *outlineList{};

    RecentHistory recentHistory{};
    QString lastDirectory;
    bool useNativeFileDialogs{true};
    QString notebookCurrentPath;
};
