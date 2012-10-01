#include <QApplication>
#include "mainwindow.h"
#include "parametr.h"
#include <QTranslator>

Parametr* par;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name());
    a.installTranslator(&qtTranslator);
    QTranslator myappTranslator;
    myappTranslator.load("font2lcd_" + QLocale::system().name());
    a.installTranslator(&myappTranslator);
    par=new Parametr(a.font());
    MainWindow w;
    w.show();
    return a.exec();
}
