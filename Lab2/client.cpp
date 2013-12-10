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

void Client::createNewSocket(QHostAddress ip)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(error(QAbstractSocket::SocketError)));
    socket->connectToHost(ip, COMMUNICATION_PORT);
}

void Client::on_connected()
{
    internalSendMessage();
}

void Client::error(QAbstractSocket::SocketError error)
{
    emit couldNotConnect(socket->errorString() + " Error code: " + QString::number(error))  ;
}

void Client::sendMessage(QVariantMap arr, QHostAddress ip)
{
    messageBuffer = arr;
    createNewSocket(ip);
}

void Client::internalSendMessage(){
    // Convert dictionary to JSON object, then to JSON-string
    if(!messageBuffer.isEmpty()){
        QVariantMap arr = messageBuffer;
        QJsonDocument doc;
        QJsonObject obj = QJsonObject::fromVariantMap(arr);
        doc.setObject(obj);
        QString message(doc.toJson().replace("\n",""));

        // Send string!
        QString data = message + "\n";
        socket->write(data.toStdString().c_str());
        socket->flush();
        socket->disconnectFromHost();
        socket->deleteLater();
        messageBuffer = QVariantMap();
    }
}

void Client::sendUpdateToParticipants(QStringList participants, QString myIP)
{
    QVariantMap request;
    QString participantStr = participants.join(",");
    request["command"] = "update-participants";
    request["participants"] = QVariant(participantStr);

    foreach(QString ip, participants){
        if(ip != myIP){
            sendMessage(request, QHostAddress(ip));
        }
    }
}
