#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playButton_clicked()
{
    GstElement *video;
    gst_init (NULL, NULL);
    GError *err = NULL;

    video = gst_parse_launch("udpsrc port=5000 ! smokedec ! ffmpegcolorspace ! autovideosink ! xvimagesink", &err);
    qDebug() << err->message;
    gst_element_set_state (video, GST_STATE_PLAYING);

    if(GST_IS_X_OVERLAY(video)){
        gst_x_overlay_set_window_handle(GST_X_OVERLAY(video),ui->videocontainer->winId());
    } else {
        qDebug() << "Video is not an x-voersalsdak thing";
    }
}
