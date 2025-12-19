#pragma once

#include <QMessageBox>
#include <QString>

#include "theme/Theme.h"

class QWidget;

// 统一处理文件/目录选择、保存对话框以及通用消息框的主题应用。
class DialogHelper
{
public:
    static QString showOpenFileDialog(QWidget *parent, const QString &lastDirectory, bool useNative, ThemeId theme, QString *outLastDirectory);
    static QString showOpenDirectoryDialog(QWidget *parent, const QString &lastDirectory, bool useNative, ThemeId theme, QString *outLastDirectory);
    static QString showSaveFileDialog(QWidget *parent, const QString &title, const QString &lastDirectory, bool useNative, ThemeId theme, QString *outLastDirectory);
    static QMessageBox::StandardButton showUnsavedWarning(QWidget *parent, ThemeId theme);
    static void applyDialogTheme(QWidget *dialog, ThemeId theme);
};
