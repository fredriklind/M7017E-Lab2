#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "client.h"
#include <gst/gst.h>
#include "videoserver.h"
#include "videoclient.h"
#include <QHostInfo>
#include <QHostAddress>
#include <QStringList>
#include <QListWidget>

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
    void couldNotConnectToCallee(QString);
    void toggleParticipantListWindow();

private:
    Ui::MainWindow *ui;
    Server *server;
    Client *client;
    GstElement *overlay;
    VideoServer *videoServer;
    VideoClient *videoClient;
    void delegateMessage(QVariantMap);
    QStringList participants;
    QString myIP;
    WId addVideoToInterface();
    QListWidget *participantListWidget;
    QDialog *participantListWindow;
    void rescaleWindow();
    int getPortNumberForConnectionBetweenSlots(int, int);
    int mySlot();
    int myTempSlot;

    //Participant methods
    void addParticipant(QString);
    void removeParticipant(QString);
    void setParticipants(QStringList);
};

#endif // MAINWINDOW_H
