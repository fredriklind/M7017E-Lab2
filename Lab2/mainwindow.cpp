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
#include <QScreen>
#include <QGuiApplication>

#define BASE_PORT 6000
#define SLOT_SIZE 10

// The listen command!
// GST_DEBUG=2,udpsrc:5 gst-launch-0.10 -v udpsrc port=6002 ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink
// gst-launch-0.10 videotestsrc ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Text communication
    client = new Client(this);
    server = new Server(this);
    server->listen();
    connect(server, SIGNAL(didReceiveMessage(QString)), this, SLOT(serverDidReceiveMessage(QString)));

    // Video communication
    videoClient = new VideoClient();
    videoServer = new VideoServer();

    // Get local IP
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            myIP = address.toString();
    }

    // Setup video layout
    QHBoxLayout *layout = new QHBoxLayout;
    ui->videoWidget->setStyleSheet("background:black;");
    ui->videoWidget->setLayout(layout);
    videoServer->init(addVideoToInterface());
}

WId MainWindow::addVideoToInterface()
{
    QWidget *w = new QWidget();
    ui->videoWidget->layout()->addWidget(w);
    w->show();
    rescaleWindow();
    return w->winId();
}

void MainWindow::serverDidReceiveMessage(QString str)
{
    QByteArray ba(str.toStdString().c_str());
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QVariantMap arr = obj.toVariantMap();
    delegateMessage(arr);
}

void MainWindow::hostLookupResult(QHostInfo info){
    if(info.error == NoError){
        qDebug() << "IP found, no error";
    } else {
        QMessageBox msgBox;
        msgBox.setText(info.errorString());
        msgBox.exec();
    }
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
            addParticipant(myIP.toString());
        }

        // Add caller to participants with LAST_PORT + 2 of participants
        // int lastFreePort = participants[getLastParticipant()].toInt() + 1;
        // addParticipant(arr["sender-ip"].toString(), lastFreePort);

        //qDebug() << participants[getLastParticipant()];
        // Send update-participants command to all participants
        // client->updateParticipants();
    }

    if(command == "update-participants"){
        // Set local participant array to received one
        // setParticipants((QVariantMap)arr["participants"]);
    }
}

void MainWindow::on_callButton_clicked()
{
    /*QVariantMap arr;
    arr["command"] = "initiate-call";
    QHostAddress ip(ui->ipField->text());
    client->sendMessage(arr, ip);
    //videoClient->addListenPort(5000, addVideoToInterface());
    addParticipant("130.240..." + QString::number(qrand()));*/

    QHostInfo::lookupHost(ui->ipField->text(), this, SLOT(hostLookupResult(QHostInfo)));
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
void MainWindow::addParticipant(QString ip)
{
    if(true){
        // Insert IP at end of participant list
        participants.insert(ip, participants.count() +1);

        // Start sending and listening to new newly added participant
        if(ip != myIP.toString()){
            videoClient->addListenPort(5000, addVideoToInterface());
            //videoServer->addListenPort(slot, ui->videoWidget->winId());
        }
        qDebug() << "Did add participant";
    } else {
        qDebug() << "Did not add: Participant already in list";
    }
}

void MainWindow::removeParticipant(QString ip)
{
    if(participants[ip]){
        participants.remove(ip);
        qDebug() << "Did remove participant";
    } else {
        qDebug() << "Could not remove: Participant not in list";
    }
}

void MainWindow::setParticipants(QMap<QString, int> newParticipantList)
{
    participants = newParticipantList;
    QMapIterator<QString, int> i(participants);
    while(i.hasNext()){
        i.next();
        if(i.key() == myIP.toString()){
            //videoServer->addNewClient();
        }
    }
}

QString MainWindow::getLastParticipant()
{
    QMapIterator<QString, int> i(participants);
    QString keyWithHighestIP;
    int ip = 0;
    while(i.hasNext()){
        i.next();
        if(i.value() > ip){
            ip = i.value();
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

void MainWindow::rescaleWindow()
{
    int newWidth = 370+participants.count()*480;
    QScreen *screen = QApplication::screens().first();
    if( newWidth < screen->size().width()){
        this->resize(newWidth,this->height());
        this->move(QApplication::screens().first()->geometry().center() - this->rect().center());
    }
}

// Get the port
int MainWindow::getPortNumberForConnectionBetweenSlots(int fromSlot, int toSlot)
{
    return BASE_PORT + SLOT_SIZE*fromSlot + toSlot;
}

MainWindow::~MainWindow()
{
    delete ui;
}
