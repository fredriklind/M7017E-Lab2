#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void serverDidReceiveMessage(QString);
    void on_sendButton_clicked();
    void on_listenButton_clicked();

private:
    Ui::MainWindow *ui;
    Server *server;
    Client *client;
};

#endif // MAINWINDOW_H
