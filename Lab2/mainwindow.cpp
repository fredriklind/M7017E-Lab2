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
#include <QMap>
#include <QVBoxLayout>
#include <QGridLayout>

#define STARTING_PORT 5000

// The listen command!
// GST_DEBUG=2,udpsrc:5 gst-launch-0.10 -v udpsrc port=6002 ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink
// gst-launch-0.10 videotestsrc ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink

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
    videoServer = NULL;

    QHBoxLayout *layout = new QHBoxLayout;
    //layout->setSizeConstraint(QLayout::SetFixedSize);
    ui->videoWidget->setStyleSheet("height:200px");
    ui->videoWidget->setLayout(layout);

}

WId MainWindow::addVideoToInterface()
{
    QWidget *w = new QWidget();
    ui->videoWidget->layout()->addWidget(w);
    w->setStyleSheet("height:200px;background:green;");
    return w->winId();
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

   if(command == "initiate-call"){
        // IF_CALLEE_HAS_PARTICIPANTS Add self to existing participants, send update-participants to all
        // IF IDLE: Add self to participants with STARTING_PORT
        if(participants.isEmpty()){
            if(arr.contains("participants")){
                //setParticipants((QVariantMap)arr["participants"]);
            }
            addParticipant(myIP.toString(), STARTING_PORT);
        }

        // Add caller to participants with LAST_PORT + 2 of participants
        int lastFreePort = participants[getLastParticipant()].toInt() + 1;
        addParticipant(arr["sender-ip"].toString(), lastFreePort);

        qDebug() << participants[getLastParticipant()];
        // Send update-participants command to all participants
       // client->updateParticipants();
    }

    if(command == "update-participants"){
        // Set local participant array to received one
        //setParticipants((QVariantMap)arr["participants"]);
    }
}

void MainWindow::on_callButton_clicked()
{
    /*QVariantMap arr;
    arr["command"] = "initiate-call";
    QHostAddress ip(ui->ipField->text());
    client->sendMessage(arr, ip);*/

    //videoClient->addListenPort(5000,addVideoToInterface());

    addVideoToInterface();

    //videoServer = new VideoServer(this);
    //videoServer->addNewClient("130.240.93.175", 5001);

    //addParticipant("130.240.53.164", 6000);
    //addParticipant("THE LAST ONE", 1337);
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
            if(ip != myIP.toString()) videoClient->addListenPort(port, ui->videoWidget->winId());
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
    QMapIterator<QString, QVariant> i(participants);
    while(i.hasNext()){
        i.next();
        if(i.key() == myIP.toString()){
            //videoServer->addNewClient();
        }

    }
}

QString MainWindow::getLastParticipant()
{
    QMapIterator<QString, QVariant> i(participants);
    QString keyWithHighestIP;
    int ip = 0;
    while(i.hasNext()){
        i.next();
        if(i.value().toInt() > ip){
            ip = i.value().toInt();
            keyWithHighestIP = i.key();
        }
    }
    return keyWithHighestIP;
}

void MainWindow::on_showCamera_clicked()
{
    if(videoServer == NULL){
        videoServer = new VideoServer();
        ui->showCamera->hide();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


