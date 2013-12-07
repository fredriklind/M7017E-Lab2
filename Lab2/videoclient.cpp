#include "videoclient.h"
#include <gst/gst.h>
#include <QDebug>

VideoClient::VideoClient(QObject *parent) :
    QObject(parent)
{
}

void VideoClient::addListenPort(QString port)
{
    gst_init (NULL, NULL);
    QString str("udpsrc port="+ port + " ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink");
   // GError *err = NULL;
    GstElement *el = gst_parse_launch(str.toStdString().c_str(), NULL);

    //qDebug() << QString(err->message);

    if(GST_IS_ELEMENT(el)){
        qDebug() << "Now listening to port " << port;
    }
}
