#include "mainwindow.h"
#include <QApplication>
#include "qssloader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSSLoader::setStyle(":/style.qss");
    MainWindow w;
    w.show();

    return a.exec();
}
