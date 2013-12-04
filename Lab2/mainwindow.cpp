#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    client = new Client(this);
}

void MainWindow::serverDidReceiveMessage(QString str)
{
    QByteArray ba(str.toStdString().c_str());
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QVariantMap arr = obj.toVariantMap();


    qDebug() << QJsonDocument::fromVariant(arr).toJson();

    QMessageBox Msgbox;
    Msgbox.setText("Got this shit: "+ arr["message"].toString());
    Msgbox.exec();
}

// Gst stuff
/*
void MainWindow::on_sendButton_clicked()
{
    GstElement *video;
    gst_init (NULL, NULL);
    GError *err = NULL;

    //video = gst_parse_launch ("autovideosrc ! video/x-raw-yuv, width=320,height=240,framerate=10/1 ! ffmpegcolorspace ! smokeenc keyframe=8 qmax=40 ! udpsink host=127.0.0.1 port=5000", &err);

    video = gst_parse_launch("udpsrc port=5000 ! smokedec ! ffmpegcolorspace ! autovideosink ! xvimagesink", &err);
    //qDebug() << err->message;
    gst_element_set_state (video, GST_STATE_PLAYING);

    if(GST_IS_X_OVERLAY(video)){
        gst_x_overlay_set_window_handle(GST_X_OVERLAY(video),ui->videocontainer->winId());
    } else {
        qDebug() << "Video is not an x-voersalsdak thing";
    }
}*/

void MainWindow::on_sendButton_clicked()
{
    //while(true){ if(client->isConnected) break;}
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
