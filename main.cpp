#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    QString dir = a.applicationDirPath() + "/" + "translations";

    if (qtTranslator.load(QLocale(), "language", ".", dir, ".qm"))
    {
        a.installTranslator(&qtTranslator);
    }


    MainWindow w;
    w.show();

    return a.exec();
}
