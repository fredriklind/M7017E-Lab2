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

    GstElement *pipeline;
    gst_init (NULL, NULL);

    pipeline = gst_parse_launch ("playbin2 uri=http://docs.gstreamer.com/media/sintel_trailer-480p.webm", NULL);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    if(GST_IS_X_OVERLAY(pipeline)){
        gst_x_overlay_set_window_handle(GST_X_OVERLAY(pipeline),this->winId());
    } else {
        qDebug() << "Video is not an x-voersalsdak thing";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
