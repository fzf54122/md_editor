#pragma once

#include <QObject>

class EditorBridge : public QObject
{
    Q_OBJECT

public:
    explicit EditorBridge(QObject *parent = nullptr);

public slots:
    void notifyContentChanged(const QString &markdown);
    void requestInitialContent();
    void notifyCursorPosition(int line);
    void notifyCursorState(int line, int wordCount);

signals:
    void contentChanged(const QString &markdown);
    void initialContentRequested();
    void cursorPositionChanged(int line);
    void cursorStateChanged(int line, int wordCount);
};
