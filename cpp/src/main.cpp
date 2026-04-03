#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GF_rootAnalysis w;
    w.show();
    return a.exec();
}
