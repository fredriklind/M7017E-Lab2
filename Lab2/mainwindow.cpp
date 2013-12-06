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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    client = new Client(this);
}

void MainWindow::serverDidReceiveMessage(QString str)
{
    qDebug() << str;
    QByteArray ba(str.toStdString().c_str());
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QVariantMap arr = obj.toVariantMap();


    //qDebug() << QJsonDocument::fromVariant(arr).toJson();

    QMessageBox Msgbox;
    Msgbox.setText("Got this bajs,shit,pop: "+ arr["message"].toString());
    Msgbox.exec();
}

void MainWindow::on_sendButton_clicked()
{
    QVariantMap arr;
    arr["message"] = ui->messageField->text();
    arr["Other non-related stuff"] = "Booyah";
    QHostAddress ip(ui->ipField->text());
    client->sendMessage(arr, ip);
}

void MainWindow::on_listenButton_clicked()
{
    server = new Server(this);
    server->listen();
    connect(server, SIGNAL(didReceiveMessage(QString)), this, SLOT(serverDidReceiveMessage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_gst2_clicked()
{

    GstElement *video;
    gst_init (NULL, NULL);
    GError *err = NULL;
    video = gst_parse_launch("udpsrc port=6002 ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink", &err);
    //qDebug() << err->message;
    gst_element_set_state (video, GST_STATE_PLAYING);
}

void MainWindow::on_pushButton_clicked()
{
    videoServer = new VideoServer(this);
}

void MainWindow::on_messageField_textChanged(const QString &arg1)
{
    //const char* str = ui->messageField->text().toHtmlEscaped().toUtf8();
    //g_object_set(G_OBJECT(overlay), "text", str, NULL);
}

// Add client
void MainWindow::on_pushButton_2_clicked()
{
    QHostAddress ip(ui->ipField->text());
    videoServer->addNewClient(ip);
}
