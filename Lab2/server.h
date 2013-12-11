#ifndef SERVER_H
#define SERVER_H

#include <QObject>
class QTcpServer;
class QTcpSocket;

/**
 * @brief Handles receiving of text communication
 */
class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    void listen();
    bool isConnected; /**< Wether the server has a connection to a client or not */
signals:
    void didReceiveMessage(QString);

public slots:
    void on_newConnection();
    void on_readyRead();
    void on_disconnected();
private:
    QTcpServer* server; /**< The server that is used for communication */
    QTcpSocket* socket; /**< The socket that is used for communication */

};

#endif // SERVER_H
