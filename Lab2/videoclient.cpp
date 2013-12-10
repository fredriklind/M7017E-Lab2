#include "videoclient.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <QDebug>

VideoClient::VideoClient(QObject *parent) :
    QObject(parent)
{
}

void VideoClient::addListenPort(int port, WId windowID)
{
    gst_init (NULL, NULL);
    //QString str("udpsrc port="+ QString::number(port) + " ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! xvimagesink force-aspect-ratio=true name=output");
    QString str("videotestsrc ! xvimagesink force-aspect-ratio=true name=output sync=true");
    // GError *err = NULL;
    GstElement *clientBin = gst_parse_bin_from_description(str.toStdString().c_str(), true, NULL);

    GstElement *output = gst_bin_get_by_name(GST_BIN(clientBin), "output");

    if(output && GST_IS_X_OVERLAY(output)){
           gst_x_overlay_set_window_handle(GST_X_OVERLAY(output),windowID);
    } else {
        qDebug() << "It's not an xoverlay thing!";
    }


    gst_element_set_state(clientBin, GST_STATE_PLAYING);

    //listenPorts.insert(port, el);

    if(GST_IS_ELEMENT(clientBin)){
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
