#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile QSS(":/css/css.qss");
    QSS.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(QSS.readAll());
    QSS.close();

    MainWindow w;
    w.setStyleSheet(StyleSheet);
    w.show();
    return a.exec();
}
