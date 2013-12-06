#ifndef VIDEOSERVER_H
#define VIDEOSERVER_H

#include <QObject>
#include <gst/gst.h>

class VideoServer : public QObject
{
    Q_OBJECT
public:
    explicit VideoServer(QObject *parent = 0);

private:
    GstElement *pipeline, *outboundSink;

signals:
    
public slots:
    
};

#endif // VIDEOSERVER_H
