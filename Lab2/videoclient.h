#ifndef VIDEOCLIENT_H
#define VIDEOCLIENT_H

#include <QObject>

class VideoClient : public QObject
{
    Q_OBJECT
public:
    explicit VideoClient(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // VIDEOCLIENT_H
