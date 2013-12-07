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

// The listen command!
// udpsrc port=6002 ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink

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

    if(currentMainState == MAIN_STATE_IDLE)

    qDebug() << command;
}

void MainWindow::on_callButton_clicked()
{
    QVariantMap arr;
    arr["command"] = "initiate-call";
    QHostAddress ip(ui->ipField->text());
    client->sendMessage(arr, ip);

    videoServer = new VideoServer(this);
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

MainWindow::~MainWindow()
{
    delete ui;
}
