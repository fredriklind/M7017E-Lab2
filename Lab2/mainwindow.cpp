#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QHostAddress>
#include "videoserver.h"
#include <QNetworkInterface>

// The listen command!
// GST_DEBUG=2,udpsrc:5 gst-launch-0.10 -v udpsrc port=6002 ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new Client(this);
    server = new Server(this);
    server->listen();
    currentMainState = MAIN_STATE_IDLE;
    connect(server, SIGNAL(didReceiveMessage(QString)), this, SLOT(serverDidReceiveMessage(QString)));
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            myIP = address.toString();
    }
}

void MainWindow::serverDidReceiveMessage(QString str)
{
    QByteArray ba(str.toStdString().c_str());
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QVariantMap arr = obj.toVariantMap();
    delegateMessage(arr);
}

void MainWindow::delegateMessage(QVariantMap arr)
{
    QString command = arr["command"].toString();
    qDebug() << command;

   /* if(command == "initiate-call"){

        // IF_CALLEE_HAS_PARTICIPANTS Add self to existing participants, send update-participants to all
        // IF IDLE: Add self to participants with STARTING_PORT
        if(participants.isEmpty()){
            if(arr.contains("participants")){
                setParticipants(arr["participants"]);
            }
            addParticipant(myIP);
        }

        // Add caller to participants with LAST_IP of participants
        addParticipant(arr["sender-ip"]);

        // Send update-participants command to all participants
        client->updateParticipants();
    }

    if(command == "update-participants"){
        // Set local participant array to received one
        setParticipants(arr["participants"]);
    }*/
}

void MainWindow::on_callButton_clicked()
{
    QVariantMap arr;
    arr["command"] = "initiate-call";
    QHostAddress ip(ui->ipField->text());
    client->sendMessage(arr, ip);

    //videoServer = new VideoServer(this);
    //videoServer->addNewClient("130.240.93.175", 6000);

    //addParticipant("130.240.53.164", 6000);
}

void MainWindow::on_messageField_textChanged(const QString &arg1)
{
    videoServer->setTextOverlay(arg1);
}

// Add client
/*void MainWindow::on_pushButton_2_clicked()
{
    QHostAddress ip(ui->ipField->text());
    videoServer->addNewClient(ip);
}*/


/* ---- Participant list methods ---- */
void MainWindow::addParticipant(QString ip, int port)
{

    if(!participants[ip].isValid()){
            participants.insert(ip, port);
            videoClient->addListenPort(port);
            qDebug() << "Did add participant";
    } else {
        qDebug() << "Did not add: Participant already in list";
    }
}

void MainWindow::removeParticipant(QString ip)
{
    if(participants[ip].isValid()){
            participants.remove(ip);
            qDebug() << "Did remove participant";
    } else {
        qDebug() << "Could not remove: Participant not in list";
    }
}

void MainWindow::setParticipants(QVariantMap newParticipantList)
{
    participants = newParticipantList;
}

MainWindow::~MainWindow()
{
    delete ui;
}
