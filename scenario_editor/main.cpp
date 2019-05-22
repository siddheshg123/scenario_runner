#include "mainwindow.h"
#include"coordinates.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    CarlaStreamThread c;
    //c.plot_map();
    w.show();

    return a.exec();
}
