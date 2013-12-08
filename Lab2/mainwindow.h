#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "client.h"
#include <gst/gst.h>
#include "videoserver.h"
#include "videoclient.h"

typedef enum {
    MAIN_STATE_IDLE     =0,
    MAIN_STATE_CLIENT   =1,
    MAIN_STATE_SERVER   =2
} MainState;

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
    void on_messageField_textChanged(const QString &arg1);
    void on_callButton_clicked();

private:
    Ui::MainWindow *ui;
    Server *server;
    Client *client;
    GstElement *overlay;
    VideoServer *videoServer;
    VideoClient *videoClient;
    void delegateMessage(QVariantMap);
    MainState currentMainState;
    QVariantMap participants;
    QHostAddress myIP;

    //Participant methods
    void addParticipant(QString, int);
    void removeParticipant(QString);
    void setParticipants(QVariantMap);
};

#endif // MAINWINDOW_H
