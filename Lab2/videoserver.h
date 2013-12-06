#ifndef VIDEOSERVER_H
#define VIDEOSERVER_H

#include <QObject>
#include <gst/gst.h>
#include <QHostAddress>

class VideoServer : public QObject
{
    Q_OBJECT
public:
    explicit VideoServer(QObject *parent = 0);
    void addNewClient(QHostAddress);
    void removeClient(QHostAddress);

private:
    GstElement *pipeline, *outboundSink;

signals:
    
public slots:
    
};

#endif // VIDEOSERVER_H
