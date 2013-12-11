#ifndef VIDEOCLIENT_H
#define VIDEOCLIENT_H

#include <QObject>
#include <QMap>
#include <gst/gst.h>
#include <QWidget>

class VideoClient : public QObject
{
    Q_OBJECT
public:
    explicit VideoClient(QObject *parent = 0);
    void addListenPort(int, WId);
    void removeListenPort(int);
private:
     QMap<int, GstElement*> listenPorts; /**< List of ports and their corresponding GStElement. Not currently used */

signals:
    
public slots:
    
};

#endif // VIDEOCLIENT_H
