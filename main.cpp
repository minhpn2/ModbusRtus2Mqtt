#include "maincontroller.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    maincontroller _maincontroler;
    _maincontroler.start();

    return a.exec();
}
