#include "client.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <cstdio>
#include <QAbstractSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostInfo>

#define COMMUNICATION_PORT 5000
#define TIMEOUT_TIME 2000

Client::Client(QObject *parent) :
    QObject(parent)
{
    messageBuffer = QVariantMap();
}

/**
 * @brief Creates a socket, connects its signals to slots in the Client and connects to the specified ip, on the COMMUNICATION_PORT
 * @param ip
 */
void Client::createNewSocket(QHostAddress ip)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(error(QAbstractSocket::SocketError)));
    socket->connectToHost(ip, COMMUNICATION_PORT);
}

/**
 * @brief Signal fired when socket is connected. This trigger a internalSendMessage() because we want to send whatever we have in the messageBuffer.
 */
void Client::on_connected()
{
    internalSendMessage();
}

/**
 * @brief Notify observers on socket error. Not really used, but could be good to have in some cases.
 * @param error
 */
void Client::error(QAbstractSocket::SocketError error)
{
    emit couldNotConnect(socket->errorString() + " Error code: " + QString::number(error))  ;
}

/**
 * @brief The send method that is public, sets the buffer and stars a socket connection. The messageBuffer is used because
 * it takes time between instantiating a socket and it being connected. So we need a way to not lose the message we are to send.
 * @param arr The array to send
 * @param ip The ip to send the array to
 */
void Client::sendMessage(QVariantMap arr, QHostAddress ip)
{
    messageBuffer = arr;
    createNewSocket(ip);
}

/**
 * @brief This is the method that actually sends the message in the messageBuffer, at the right time.
 */
void Client::internalSendMessage(){
    // Convert dictionary to JSON object, then to JSON-string
    if(!messageBuffer.isEmpty() && socket->waitForConnected(4000)){
        QVariantMap arr = messageBuffer;
        QJsonDocument doc;
        QJsonObject obj = QJsonObject::fromVariantMap(arr);
        doc.setObject(obj);
        QString message(doc.toJson().replace("\n",""));

        qDebug() << "SENDING THIS: ";
        qDebug() << doc.toJson();

        // Send string!
        QString data = message + "\n";
        socket->write(data.toStdString().c_str());
        socket->flush();
        socket->disconnectFromHost();
        socket->deleteLater();
        messageBuffer = QVariantMap();
    }
}

/**
 * @brief Sends an update-participants command to all participants provided, excluding myIP
 * @param participants The participants to send the command to
 * @param myIP The IP to exclude from the sending.
 */
void Client::sendUpdateToParticipants(QStringList participants, QString myIP)
{
    QVariantMap request;
    QString participantStr = participants.join(",");
    request["command"] = "update-participants";
    request["participants"] = QVariant(participantStr);

    foreach(QString ip, participants){
        if(ip != myIP){
            sendMessage(request, QHostAddress(ip));
            socket->waitForDisconnected();
        }
    }
}
