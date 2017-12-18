#include "mainwindow.h"
#include "game.h"

#include <QApplication>
#include <vector>

int main(int argc, char *argv[])
{
    // Testing the GUI
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
