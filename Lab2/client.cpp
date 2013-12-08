#include "client.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <cstdio>
#include <QAbstractSocket>
#include <QJsonDocument>
#include <QJsonObject>

#define LISTEN_PORT 6000

Client::Client(QObject *parent) :
    QObject(parent)
{
    messageBuffer = QVariantMap();
}

void Client::createNewSocket(QHostAddress ip)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()),
            this, SLOT(on_connected()));
    socket->connectToHost(ip, LISTEN_PORT);
}


void Client::on_connected()
{
    internalSendMessage();
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
