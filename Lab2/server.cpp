#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <cstdio>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

#define COMMUNICATION_PORT 5000

Server::Server(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()),
            this, SLOT(on_newConnection()));
}

void Server::listen()
{
    if(!server->listen(QHostAddress::Any,COMMUNICATION_PORT))
    {
        qDebug() << "Couldn't listen to port" << server->serverPort() << ":" << server->errorString();
    }
}

void Server::on_newConnection()
{
    socket = server->nextPendingConnection();
    if(socket->state() == QTcpSocket::ConnectedState)
    {
        isConnected = true;
    }
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(on_disconnected()));
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(on_readyRead()));
}

void Server::on_readyRead()
{
        // note that QByteArray can be casted to char * and const char *
        QByteArray ba = socket->readAll();

        //To QString
        QString str(ba);
        ba.chop(1);

        // To JSON ojbect
        QByteArray ba2(str.toStdString().c_str());
        QJsonObject obj = QJsonDocument::fromJson(ba2).object();
        QVariantMap arr = obj.toVariantMap();

        // Add sender
        arr["sender-ip"] = socket->peerAddress().toString();
        arr["sender-port"] = QString::number(socket->peerPort());

        // Back to QString
        QJsonObject obj2 = QJsonObject::fromVariantMap(arr);
        QJsonDocument doc;
        doc.setObject(obj2);
        QString str2(doc.toJson().replace("\n",""));

        emit didReceiveMessage(str2);
}

void Server::on_disconnected()
{
    disconnect(socket, SIGNAL(disconnected()));
    disconnect(socket, SIGNAL(readyRead()));
    socket->deleteLater();
}
