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

/**
 * @brief Handles all user interaction and manipulates the sending and receiving object for both text and video communication.
 */
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
    Ui::MainWindow *ui; /**< The user interface */
    Server *server; /**< The text receiving server */
    Client *client; /**< The text sending client */
    GstElement *overlay; /**< The text overlay object on the local video feed */
    VideoServer *videoServer; /**< The video sending object */
    VideoClient *videoClient; /**< The video receiving object */
    void delegateMessage(QVariantMap);
    QStringList participants; /**< This is the local list of current participants in the call. All messaging is based on this. This list is synchronized between clients with messaging */
    QString myIP; /**< The IP of the local client */
    WId addVideoToInterface();
    QListWidget *participantListWidget; /**< The list of participants */
    QDialog *participantListWindow; /**< The window that the list of participants is shown in */
    void rescaleWindow();
    int getPortNumberForConnectionBetweenSlots(int, int);
    int mySlot();
    int myTempSlot; /**< The temporary slot that the local client uses when it needs to establish connections to other clients without itself being in the participants list. */

    //Participant methods
    void addParticipant(QString);
    void removeParticipant(QString);
    void setParticipants(QStringList);
};

#endif // MAINWINDOW_H
