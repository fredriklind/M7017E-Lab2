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
    void addNewClient(QString, int);
    void removeClient(QString);
    void setTextOverlay(QString);

private:
    GstElement *pipeline, *outboundSink, *overlay, *mixer;
    void print_caps(const GstCaps*, const gchar*);
    void print_pad_capabilities (GstElement *, gchar *);
    static gboolean print_field(GQuark , const GValue * , gpointer);

signals:
    void windowClosed();
    
public slots:
    
};

#endif // VIDEOSERVER_H
