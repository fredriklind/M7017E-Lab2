#ifndef VIDEOCLIENT_H
#define VIDEOCLIENT_H

#include <QObject>

class VideoClient : public QObject
{
    Q_OBJECT
public:
    explicit VideoClient(QObject *parent = 0);
    void addListenPort(QString);
    void removeListenPort(QString);
private:
     QMap<int, GstElement> listenPorts;

signals:
    
public slots:
    
};

#endif // VIDEOCLIENT_H
