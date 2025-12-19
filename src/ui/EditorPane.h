#pragma once

#include <QWidget>
#include <QVariantMap>
#include <QWebEnginePage>

#include "theme/Theme.h"

class QWebEngineView;
class QWebChannel;
class EditorBridge;
class QWebEnginePage;

// EditorPane 封装 WebEngine 编辑器、桥接及命令执行。
class EditorPane : public QWidget
{
    Q_OBJECT

public:
    explicit EditorPane(QWidget *parent = nullptr);

    void setMarkdown(const QString &markdown);
    void applyTheme(ThemeId theme);
    void setPlainMode(bool enabled);
    void executeCommand(const QString &command, const QVariantMap &extra = QVariantMap());
    void triggerPageAction(QWebEnginePage::WebAction action);

signals:
    void contentChanged(const QString &markdown);
    void initialReady();
    void cursorStateChanged(int line, int words);

private:
    QString jsonStringLiteral(const QString &text) const;
    void applyPendingTheme();
    void applyPendingMarkdown();
    void applyPendingPlainMode();

    QWebEngineView *webView{};
    QWebChannel *webChannel{};
    EditorBridge *editorBridge{};
    bool editorReady{false};
    QString pendingMarkdown;
    QVariantMap pendingEditorTheme;
    bool currentPlainTextMode{false};
};
