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
    //qDebug() << (int)socket->state();
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
        QString str(ba);
        ba.chop(1);
        emit didReceiveMessage(str);
    }
}

void Server::on_disconnected()
{
    qDebug() << "Connection disconnected.\n";
    disconnect(socket, SIGNAL(disconnected()));
    disconnect(socket, SIGNAL(readyRead()));
    socket->deleteLater();
}
