#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
using namespace std;

int main(int argc, char *argv[])
{
    CubeValue::initValue();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.move ( (QApplication :: desktop ( ) ->width ( ) - w. width ( ) ) / 2 ,
              (QApplication :: desktop ( ) ->height ( ) - w. height ( ) ) / 3 ) ;
    return a.exec();
}
