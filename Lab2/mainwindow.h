#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "client.h"
#include <gst/gst.h>
#include "videoserver.h"
//#include "videoclient.h"

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
    void on_gst2_clicked();
    void on_pushButton_clicked();
    void on_messageField_textChanged(const QString &arg1);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    Server *server;
    Client *client;
    GstElement *overlay;
    VideoServer *videoServer;
};

#endif // MAINWINDOW_H
