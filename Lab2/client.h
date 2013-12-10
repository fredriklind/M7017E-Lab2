#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QStringList>

class QTcpSocket;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    void sendMessage(QVariantMap, QHostAddress);
    void sendUpdateToParticipants(QStringList, QString);
    QVariantMap messageBuffer;
signals:
    void couldNotConnect(QString);

public slots:
    void on_connected();
    void error(QAbstractSocket::SocketError);
private:
    QTcpSocket* socket;
    void internalSendMessage();
    void createNewSocket(QHostAddress);
};

#endif // CLIENT_H
