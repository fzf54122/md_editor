#pragma once

#include <QMainWindow>
#include <QString>

class QAction;
class QLabel;
class QSplitter;
class QTextBrowser;
class QTextEdit;
class QTimer;
class QCloseEvent;
enum class ThemeId;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupEditor();
    void setupMenu();
    void setupConnections();
    void renderPreview();
    void updateStatusInfo();
    void updateWindowTitle();
    void markDocumentClean(const QString &path);
    void markDocumentDirty();
    bool promptSaveIfDirty();
    bool saveDocument(bool forceSelectPath);
    bool writeToFile(const QString &fileName);
    void loadDocumentFromFile(const QString &fileName);
    void resetToNewDocument();
    void applyInlineFormat(const QString &prefix, const QString &suffix = QString());
    void insertLink();
    void insertCodeBlock();
    void insertUnorderedList();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QSplitter *splitter{};
    QTextEdit *editor{};
    QTextBrowser *preview{};
    QTimer *renderTimer{};
    QLabel *statusInfoLabel{};
    bool syncingScroll{};
    QString currentFilePath{};
    bool documentDirty{false};
    ThemeId currentTheme{ThemeId::Light};

    QAction *newAction{};
    QAction *openAction{};
    QAction *saveAction{};
    QAction *saveAsAction{};
    QAction *exitAction{};
    QAction *boldAction{};
    QAction *italicAction{};
    QAction *codeAction{};
    QAction *codeBlockAction{};
    QAction *linkAction{};
    QAction *unorderedListAction{};
    QAction *lightThemeAction{};
    QAction *darkThemeAction{};
    QAction *solarizedThemeAction{};
};
