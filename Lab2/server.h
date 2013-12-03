#ifndef SERVER_H
#define SERVER_H

#include <QObject>
class QTcpServer;
class QTcpSocket;

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    void listen();
    bool isConnected;
signals:
    void didReceiveMessage(QString);

public slots:
    void on_newConnection();
    void on_readyRead();
    void on_disconnected();
private:
    QTcpServer* server;
    QTcpSocket* socket;

};

#endif // SERVER_H
