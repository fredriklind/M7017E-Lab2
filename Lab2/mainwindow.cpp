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
    GstElement *source, *sink, *sink2, *capsfilter, *tee, *q1, *q2, *faceoverlay;
    GstCaps *caps;
    /* init */
    gst_init (NULL, NULL);

    /* create pipeline */
    pipeline = gst_pipeline_new ("Ma1n Spr4ying H0se");

    /* create elements */
    source = gst_element_factory_make ("autovideosrc", NULL);
    overlay = gst_element_factory_make("textoverlay", NULL);
    sink = gst_element_factory_make ("autovideosink", NULL);
    sink2 = gst_element_factory_make ("autovideosink", NULL);
    capsfilter = gst_element_factory_make ("capsfilter", NULL);
    tee = gst_element_factory_make("tee", NULL);
    q1 = gst_element_factory_make("queue", NULL);
    q2 = gst_element_factory_make("queue", NULL);
    faceoverlay = gst_element_factory_make("faceoverlay", NULL);

    // Setup caps
    caps = gst_caps_new_simple ("video/x-raw-yuv",
            "width", G_TYPE_INT, 640,
            "height", G_TYPE_INT, 480,
            "framerate", GST_TYPE_FRACTION, 30, 1,
            NULL);

    // Set attributes
    g_object_set(G_OBJECT(overlay), "font-desc","Sans 24", "text","Kamura 1", "valign","top", "halign","left", "shaded-background", true, NULL);
    //g_object_set(G_OBJECT(faceoverlay), "location", "/Users/fredriklind/face_mask.svg", NULL);

    // Add to bin
    gst_bin_add_many (GST_BIN (pipeline), source, capsfilter, tee, overlay, sink, sink2, q1, q2, faceoverlay, NULL);

    // Custom linking
    GstPad *pad1, *pad2;
    gchar *pad1name, *pad2name;
    pad1 = gst_element_get_request_pad (tee, "src%d");
    pad1name = gst_pad_get_name (pad1);
    pad2 = gst_element_get_request_pad (tee, "src%d");
    pad2name = gst_pad_get_name (pad2);

    gst_element_link_pads(source, "src", capsfilter, "sink");

    //Text overlay
    gst_element_link_pads (capsfilter, "src", overlay, "video_sink");
    gst_element_link_pads (overlay, "src", tee, "sink");

    // Face overlay
    //gst_element_link_pads (capsfilter, "src", faceoverlay, "sink");
    //gst_element_link_pads (faceoverlay, "src", tee, "sink");


    gst_element_link_pads (tee, pad1name, q1, "sink");
    gst_element_link_pads (tee, pad2name, q2, "sink");

    gst_element_link_pads (q1, "src", sink, "sink");
    gst_element_link_pads (q2, "src", sink2, "sink");

    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS, "pipeline");
    //g_object_set(G_OBJECT(), "port", "6000", NULL);
}

void MainWindow::on_change_overlay_clicked()
{


}

void MainWindow::on_messageField_textChanged(const QString &arg1)
{
    const char* str = ui->messageField->text().toHtmlEscaped().toUtf8();
    g_object_set(G_OBJECT(overlay), "text", str, NULL);
}
