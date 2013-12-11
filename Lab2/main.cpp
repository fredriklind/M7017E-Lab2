#include "mainwindow.h"
#include <QApplication>

/**
 * @brief Sets up a new QApplication
 * @param argc
 * @param argv
 * @return Application loop
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
