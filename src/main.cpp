#include <QApplication>
#include <QStyleFactory>
#include <QIcon>
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    MainWindow window;
    QIcon appIcon(":/editor/icon.svg");
    app.setWindowIcon(appIcon);

    window.show();
    return app.exec();
}
