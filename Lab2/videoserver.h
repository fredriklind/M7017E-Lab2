#ifndef VIDEOSERVER_H
#define VIDEOSERVER_H

#include <QObject>
#include <gst/gst.h>
#include <QHostAddress>
#include <QWidget>

class VideoServer : public QObject
{
    Q_OBJECT
public:
    explicit VideoServer(QObject *parent = 0);
    void sendToNewClient(QString, int);
    void removeClient(QString);
    void setTextOverlay(QString);
    void init(WId);

private:
    GstElement *pipeline, *outboundSink, *overlay, *mixer; /**< The elements that need to be used outside the init() method */
    void print_caps(const GstCaps*, const gchar*);
    void print_pad_capabilities (GstElement *, gchar *);
    static gboolean print_field(GQuark , const GValue * , gpointer);

signals:
    void windowClosed();
    
public slots:
    
};

#endif // VIDEOSERVER_H
