#include "videoclient.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <QDebug>

/**
 * @brief Is responsible for receiving video from other VideoServers
 * @param parent
 */
VideoClient::VideoClient(QObject *parent) :
    QObject(parent)
{
}

/**
 * @brief Starts listening for video on the specified port, and inserts the video feed into the window with the windowID provided.
 * @param port The port to start listening to.
 * @param windowID The windowID of the window that the video should be inserted into.
 */
void VideoClient::addListenPort(int port, WId windowID)
{
    gst_init (NULL, NULL);
    QString str("udpsrc port="+ QString::number(port) + " ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! xvimagesink force-aspect-ratio=true name=output");
    GstElement *clientBin = gst_parse_bin_from_description(str.toStdString().c_str(), true, NULL);
    GstElement *output = gst_bin_get_by_name(GST_BIN(clientBin), "output");

    if(output && GST_IS_X_OVERLAY(output)){
        GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(clientBin), GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS, "receiving-pipeline");
        gst_x_overlay_set_window_handle(GST_X_OVERLAY(output),windowID);
    } else {
        qDebug() << "It's not an xoverlay thing!";
    }

    gst_element_set_state(clientBin, GST_STATE_PLAYING);

    if(GST_IS_ELEMENT(clientBin)){
        qDebug() << "Now listening to port " << port;
    }
}

/**
 * @brief Stop listening to a certain port. Not currently used.
 * @param port The port to stop listening to.
 */
void VideoClient::removeListenPort(int port)
{
    if(GST_IS_ELEMENT(listenPorts[port])){
        listenPorts.remove(port);
        qDebug() << "Did remove listening port";
    }
}
