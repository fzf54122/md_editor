#include "EditorBridge.h"

EditorBridge::EditorBridge(QObject *parent)
    : QObject(parent)
{
}

void EditorBridge::notifyContentChanged(const QString &markdown)
{
    emit contentChanged(markdown);
}

void EditorBridge::requestInitialContent()
{
    emit initialContentRequested();
}

void EditorBridge::notifyCursorPosition(int line)
{
    emit cursorPositionChanged(line);
}

void EditorBridge::notifyCursorState(int line, int wordCount)
{
    emit cursorStateChanged(line, wordCount);
}
