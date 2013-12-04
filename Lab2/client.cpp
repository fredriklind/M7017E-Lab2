#include "client.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <cstdio>
#include <QAbstractSocket>
#include <QJsonDocument>
#include <QJsonObject>

Client::Client(QObject *parent) :
    QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()),
            this, SLOT(on_connected()));
}

void Client::on_connected()
{
    isConnected = true;
    internalSendMessage();
}

void Client::sendMessage(QVariantMap arr)
{
    if(isConnected){
        messageBuffer = arr;
        internalSendMessage();

    } else {
        messageBuffer = arr;
        connectToServer();
    }
}

void Client::internalSendMessage(){
    // Convert dictionary to JSON object, then to JSON-string
    if(!messageBuffer.isEmpty()){
        QVariantMap arr = messageBuffer;
        QJsonDocument doc;
        QJsonObject obj = QJsonObject::fromVariantMap(arr);
        doc.setObject(obj);
        QString message(doc.toJson(QJsonDocument::Compact));

        // Send string!
        QString data = message + "\n";
        socket->write(data.toStdString().c_str());
        socket->flush();
    }
}

void Client::connectToServer()
{
    if(!isConnected) socket->connectToHost(QHostAddress::LocalHost, 5000);
}
