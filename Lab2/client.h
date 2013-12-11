#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QStringList>

class QTcpSocket;

/**
 * @brief Handles sending of text communication.
 */
class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    void sendMessage(QVariantMap, QHostAddress);
    void sendUpdateToParticipants(QStringList, QString);
    QVariantMap messageBuffer; /**< This is set when someone wants to send a message, and the used when that message really can be sent. */
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
