#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <cstdio>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

Server::Server(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()),
            this, SLOT(on_newConnection()));
}

void Server::listen()
{
    server->listen(QHostAddress::LocalHost, 5000);
    qDebug() << "Listening...";
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
    while(socket->canReadLine())
    {
        QByteArray ba = socket->readLine();
        if(strcmp(ba.constData(), "\n") == 0)
        {
            socket->disconnectFromHost();
            break;
        }

        //To QString
        QString str(ba);
        ba.chop(1);

        // To JSON ojbect
        QByteArray ba2(str.toStdString().c_str());
        QJsonObject obj = QJsonDocument::fromJson(ba2).object();
        QVariantMap arr = obj.toVariantMap();

        // Add sender
        arr["sender-ip"] = socket->peerAddress().toString();
        arr["sender-name"] = socket->peerName();
        arr["sender-port"] = QString::number(socket->peerPort());

        // Back to QString
        QJsonObject obj2 = QJsonObject::fromVariantMap(arr);
        QJsonDocument doc;
        doc.setObject(obj2);
        QString str2(doc.toJson(QJsonDocument::Compact));

        emit didReceiveMessage(str2);
    }
}

void Server::on_disconnected()
{
    qDebug() << "Connection disconnected.\n";
    disconnect(socket, SIGNAL(disconnected()));
    disconnect(socket, SIGNAL(readyRead()));
    socket->deleteLater();
}
