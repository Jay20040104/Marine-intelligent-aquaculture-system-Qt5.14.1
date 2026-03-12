#include "mainwindow.h"

#include <QApplication>

#include <QLoggingCategory> // 需包含此头文件
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
