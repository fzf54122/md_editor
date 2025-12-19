#include "DialogHelper.h"

#include <QFileDialog>
#include <QSettings>
#include <QPushButton>

QString DialogHelper::showOpenFileDialog(QWidget *parent, const QString &lastDirectory, bool useNative, ThemeId theme, QString *outLastDirectory)
{
    QFileDialog dialog(parent, "打开文件", lastDirectory.isEmpty() ? QDir::homePath() : lastDirectory);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setNameFilters({
        "Markdown (*.md)",
        "Text (*.txt)",
        "All Files (*)"
    });
    if (!useNative) {
        dialog.setOption(QFileDialog::DontUseNativeDialog, true);
        dialog.resize(720, 480);
        applyDialogTheme(&dialog, theme);
    }

    if (dialog.exec() == QDialog::Accepted) {
        const QString file = dialog.selectedFiles().value(0);
        if (outLastDirectory && !file.isEmpty()) {
            *outLastDirectory = QFileInfo(file).absolutePath();
        }
        return file;
    }
    return {};
}

QString DialogHelper::showOpenDirectoryDialog(QWidget *parent, const QString &lastDirectory, bool useNative, ThemeId theme, QString *outLastDirectory)
{
    QFileDialog dialog(parent, "打开文件夹", lastDirectory.isEmpty() ? QDir::homePath() : lastDirectory);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.setViewMode(QFileDialog::Detail);
    if (!useNative) {
        dialog.setOption(QFileDialog::DontUseNativeDialog, true);
        dialog.resize(720, 480);
        applyDialogTheme(&dialog, theme);
    }

    if (dialog.exec() == QDialog::Accepted) {
        const QString dir = dialog.selectedFiles().value(0);
        if (outLastDirectory && !dir.isEmpty()) {
            *outLastDirectory = QFileInfo(dir).absolutePath();
        }
        return dir;
    }
    return {};
}

QString DialogHelper::showSaveFileDialog(QWidget *parent, const QString &title, const QString &lastDirectory, bool useNative, ThemeId theme, QString *outLastDirectory)
{
    QFileDialog dialog(parent, title, lastDirectory.isEmpty() ? QDir::homePath() : lastDirectory);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setNameFilters({
        "Markdown (*.md)",
        "Text (*.txt)",
        "All Files (*)"
    });
    dialog.selectNameFilter("Markdown (*.md)");
    if (!useNative) {
        dialog.setOption(QFileDialog::DontUseNativeDialog, true);
        dialog.resize(720, 480);
        applyDialogTheme(&dialog, theme);
    }

    if (dialog.exec() == QDialog::Accepted) {
        const QString file = dialog.selectedFiles().value(0);
        if (outLastDirectory && !file.isEmpty()) {
            *outLastDirectory = QFileInfo(file).absolutePath();
        }
        return file;
    }
    return {};
}

QMessageBox::StandardButton DialogHelper::showUnsavedWarning(QWidget *parent, ThemeId theme)
{
    QMessageBox box(parent);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle("未保存的更改");
    box.setText("文档有未保存的更改。");
    box.setInformativeText("请选择你希望执行的操作：");

    QAbstractButton *saveBtn    = box.addButton("保存", QMessageBox::AcceptRole);
    QAbstractButton *discardBtn = box.addButton("放弃", QMessageBox::DestructiveRole);
    QAbstractButton *cancelBtn  = box.addButton("取消", QMessageBox::RejectRole);

    applyDialogTheme(&box, theme);

    box.exec();
    QAbstractButton *clicked = box.clickedButton();
    if (clicked == saveBtn)    return QMessageBox::Save;
    if (clicked == discardBtn) return QMessageBox::Discard;
    if (clicked == cancelBtn)  return QMessageBox::Cancel;
    return QMessageBox::Cancel;
}

void DialogHelper::applyDialogTheme(QWidget *dialog, ThemeId theme)
{
    if (!dialog)
        return;
    dialog->setStyleSheet(themeStylesheet(theme));
}
