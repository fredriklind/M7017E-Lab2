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
    Msgbox.setText("Got this bajs,shit,pop: "+ arr["message"].toString());
    Msgbox.exec();
}

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

void MainWindow::on_gst1_clicked()
{
    GstElement *video;
    gst_init (NULL, NULL);
    GError *err = NULL;
    video = gst_parse_launch ("autovideosrc ! ffmpegcolorspace ! smokeenc keyframe=8 qmax=40 ! multiudpsink clients=\"130.240.94.92:5000\"", &err);
    gst_element_set_state (video, GST_STATE_PLAYING);
}

void MainWindow::on_gst2_clicked()
{

    GstElement *video;
    gst_init (NULL, NULL);
    GError *err = NULL;
    video = gst_parse_launch("udpsrc port=6000 ! smokedec ! ffmpegcolorspace ! autovideosink", &err);
    //qDebug() << err->message;
    gst_element_set_state (video, GST_STATE_PLAYING);
}

void MainWindow::on_pushButton_clicked()
{
    GstElement *pipeline;
    GstElement *source, *overlay, *sink;
    /* init */
    gst_init (NULL, NULL);

    /* create pipeline */
    pipeline = gst_pipeline_new ("my-pipeline");



    /* create elements */
    source = gst_element_factory_make ("autovideosrc", "source");
    overlay = gst_element_factory_make("textoverlay", "overlay");
    sink = gst_element_factory_make ("autovideosink", "sink");

    // Set attributes
    g_object_set(G_OBJECT(overlay), "font-desc","Sans 24", "text","CAM1", "valign","top", "halign","left", "shaded-background", true, NULL);
    gst_bin_add_many (GST_BIN (pipeline), source, overlay, sink, NULL);


    // Get pads
    //GstPad *autovideosrc_src, *textoverlay_video_sink, *textoverlay_src, *autovideosink_src;

   /* autovideosrc_src = gst_element_get_static_pad(source, "src");
    textoverlay_video_sink = gst_element_get_static_pad(overlay, "video-sink");
    textoverlay_src = gst_element_get_static_pad(overlay, "src");
    autovideosink_src = gst_element_get_static_pad(sink, "src");*/

    gst_element_link_pads(source, "src", overlay, "video-sink");
    gst_element_link_pads(overlay, "src", sink, "sink");

    // Link pads
    //gst_pad_link(autovideosrc_src, textoverlay_video_sink);


    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_, "pipeline");
    //g_object_set(G_OBJECT(), "port", "6000", NULL);
}





// For later
/*if(GST_IS_X_OVERLAY(video)){
    gst_x_overlay_set_window_handle(GST_X_OVERLAY(video),ui->videocontainer->winId());
} else {
    qDebug() << "Video is not an x-voersalsdak thing";
}*/

