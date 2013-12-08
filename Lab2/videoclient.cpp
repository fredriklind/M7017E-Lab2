#include "videoclient.h"
#include <gst/gst.h>
#include <QDebug>

VideoClient::VideoClient(QObject *parent) :
    QObject(parent)
{
}

void VideoClient::addListenPort(int port)
{
    gst_init (NULL, NULL);
    QString str("udpsrc port="+ QString(port) + " ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink");
    // GError *err = NULL;
    GstElement *el = gst_parse_launch(str.toStdString().c_str(), NULL);
    gst_element_set_state(el, GST_STATE_PLAYING);

    listenPorts.insert(port, el);

    if(GST_IS_ELEMENT(el)){
        qDebug() << "Now listening to port " << port;
    }
}

void VideoClient::removeListenPort(int port)
{
    if(GST_IS_ELEMENT(listenPorts[port])){
            listenPorts.remove(port);
            qDebug() << "Did remove listening port";
    }
}
