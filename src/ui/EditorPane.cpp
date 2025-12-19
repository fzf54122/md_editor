#include "EditorPane.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QUrl>
#include <QVariantMap>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QVBoxLayout>

#include "EditorBridge.h"
#include "theme/Theme.h"

EditorPane::EditorPane(QWidget *parent)
    : QWidget(parent)
{
    webView = new QWebEngineView(this);
    webView->setContextMenuPolicy(Qt::NoContextMenu);
    webView->setObjectName("MarkdownWebView");
    webView->setStyleSheet("border: none; background: #fbfbfd;");
    webView->setUrl(QUrl("qrc:/editor/editor.html"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(webView);

    webChannel = new QWebChannel(webView);
    editorBridge = new EditorBridge(this);
    webChannel->registerObject("editorBridge", editorBridge);
    webView->page()->setWebChannel(webChannel);

    connect(editorBridge, &EditorBridge::contentChanged, this, &EditorPane::contentChanged);

    connect(editorBridge, &EditorBridge::initialContentRequested, this, [this]() {
        editorReady = true;
        applyPendingMarkdown();
        applyPendingTheme();
        applyPendingPlainMode();
        emit initialReady();
    });

    connect(editorBridge, &EditorBridge::cursorStateChanged, this, [this](int line, int words) {
        emit cursorStateChanged(line, words);
    });
}

void EditorPane::setMarkdown(const QString &markdown)
{
    if (!editorReady || !webView || !webView->page()) {
        pendingMarkdown = markdown;
        return;
    }

    const QString literal = jsonStringLiteral(markdown);
    const QString script = QStringLiteral("window.TyporaEditor && TyporaEditor.setMarkdown(%1);").arg(literal);
    webView->page()->runJavaScript(script);
    pendingMarkdown.clear();
}

void EditorPane::applyTheme(ThemeId theme)
{
    const ThemePalette palette = themePalette(theme);
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
    applyPendingTheme();
}

void EditorPane::setPlainMode(bool enabled)
{
    currentPlainTextMode = enabled;
    applyPendingPlainMode();
}

void EditorPane::executeCommand(const QString &command, const QVariantMap &extra)
{
    if (!webView || command.isEmpty())
        return;

    QVariantMap payload = extra;
    payload.insert("command", command);
    const QString json = QString::fromUtf8(QJsonDocument::fromVariant(payload).toJson(QJsonDocument::Compact));
    const QString script = QStringLiteral("window.TyporaEditor && TyporaEditor.execCommand(%1);").arg(json);
    webView->page()->runJavaScript(script);
}

void EditorPane::triggerPageAction(QWebEnginePage::WebAction action)
{
    if (webView && webView->page())
        webView->page()->triggerAction(action);
}

QString EditorPane::jsonStringLiteral(const QString &text) const
{
    QJsonArray wrapper;
    wrapper.append(text);
    QString json = QString::fromUtf8(QJsonDocument(wrapper).toJson(QJsonDocument::Compact));
    if (json.size() >= 2) {
        return json.mid(1, json.size() - 2); // strip '[' and ']'
    }
    return QStringLiteral("\"\"");
}

void EditorPane::applyPendingTheme()
{
    if (!editorReady || !webView || !webView->page() || pendingEditorTheme.isEmpty())
        return;

    const QString json = QString::fromUtf8(QJsonDocument::fromVariant(pendingEditorTheme).toJson(QJsonDocument::Compact));
    const QString script = QStringLiteral("window.TyporaEditor && TyporaEditor.applyTheme(%1);").arg(json);
    webView->page()->runJavaScript(script);
    pendingEditorTheme.clear();
}

void EditorPane::applyPendingMarkdown()
{
    if (pendingMarkdown.isEmpty())
        return;
    setMarkdown(pendingMarkdown);
}

void EditorPane::applyPendingPlainMode()
{
    if (!editorReady || !webView || !webView->page())
        return;

    const QString script = QStringLiteral("window.TyporaEditor && TyporaEditor.setPlainMode(%1);")
        .arg(currentPlainTextMode ? "true" : "false");
    webView->page()->runJavaScript(script);
}
