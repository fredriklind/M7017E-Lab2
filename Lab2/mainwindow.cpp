#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->videocontainer->hide();

    server = new Server(this);
    server->listen();

    client = new Client(this);
    client->connectToServer();

    connect(server, SIGNAL(didReceiveMessage(QString)), this, SLOT(serverDidReceiveMessage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::serverDidReceiveMessage(QString str)
{
    QByteArray ba(str.toStdString().c_str());
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QVariantMap arr = obj.toVariantMap();

    qDebug() << arr["command"].toString();

    // Convert to prettficated JSON qDebug() << QJsonDocument::fromVariant(arr).toJson();
}

void MainWindow::on_pushButton_clicked()
{
    QVariantMap arr;
    arr["command"] = "Testar lite";
    arr["Tjoho"] = "Horerier";
    client->sendMessage(arr);
}

void MainWindow::on_pushButton_2_clicked()
{

}

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
}
