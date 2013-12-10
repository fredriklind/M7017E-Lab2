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
    void on_showCamera_clicked();
    void hostLookupResult(QHostInfo);

private:
    Ui::MainWindow *ui;
    Server *server;
    Client *client;
    GstElement *overlay;
    VideoServer *videoServer;
    VideoClient *videoClient;
    void delegateMessage(QVariantMap);
    QMap<QString, int> participants;
    QHostAddress myIP;
    WId addVideoToInterface();
    void rescaleWindow();
    int getPortNumberForConnectionBetweenSlots(int, int);

    //Participant methods
    void addParticipant(QString);
    void removeParticipant(QString);
    void setParticipants(QMap<QString, int>);
    QString getLastParticipant();
};

#endif // MAINWINDOW_H
