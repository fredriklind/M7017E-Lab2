#include "client.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <cstdio>
#include <QAbstractSocket>
#include <QJsonDocument>
#include <QJsonObject>

#define LISTEN_PORT 5000

Client::Client(QObject *parent) :
    QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()),
            this, SLOT(on_connected()));
    isConnected = false;
}

void Client::on_connected()
{
    isConnected = true;
    internalSendMessage();
}

void Client::sendMessage(QVariantMap arr, QHostAddress ip)
{
    messageBuffer = arr;

    if(isConnected){
        internalSendMessage();

    } else {
        connectToServer(ip);
    }
}

void Client::internalSendMessage(){
    // Convert dictionary to JSON object, then to JSON-string
    if(!messageBuffer.isEmpty()){
        QVariantMap arr = messageBuffer;
        QJsonDocument doc;
        QJsonObject obj = QJsonObject::fromVariantMap(arr);
        doc.setObject(obj);
        QString message(doc.toJson());

        // Send string!
        QString data = message + "\n";
        socket->write(data.toStdString().c_str());
        socket->flush();
    }
}

void Client::connectToServer(QHostAddress ip)
{
    if(!isConnected) socket->connectToHost(ip, LISTEN_PORT);
}
