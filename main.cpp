#include "mainwindow.h"
#include "startwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("Тестирование алгоритма");
    StartWidget startWidget(&w);
    startWidget.setWindowTitle("Начало работы");

    startWidget.show();

    return a.exec();
}
