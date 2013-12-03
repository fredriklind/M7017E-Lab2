#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QAbstractSocket>

class QTcpSocket;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    void connectToServer();
    void sendMessage(QVariantMap);
    bool isConnected;
signals:

public slots:
    void on_connected();
private:
    QTcpSocket* socket;
};

#endif // CLIENT_H