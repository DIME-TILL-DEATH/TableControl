#include "netclient.h"

NetClient::NetClient(QObject *parent)
    : QObject{parent}
{
    socket = new QTcpSocket(this);
}

void NetClient::doConnect()
{
    connect(socket, &QTcpSocket::connected,this, &NetClient::connected);
    connect(socket, &QTcpSocket::disconnected,this, &NetClient::disconnected);
    connect(socket, &QTcpSocket::bytesWritten,this, &NetClient::bytesWritten);
    connect(socket, &QTcpSocket::readyRead,this, &NetClient::readyRead);


    socket->connectToHost(targetAddress, targetPort);
}

void NetClient::sendData(const QByteArray &data)
{
    quint64 result = socket->write(data);
    if(result == -1)
    {
        qDebug() << __FUNCTION__ << "data not written";
    }
}

bool NetClient::isSocketReady()
{
    return (socket->state() == QAbstractSocket::ConnectedState);
}

void NetClient::connected()
{
    qDebug()<<"Connected to server";
}

void NetClient::disconnected()
{
    disconnect(this);
}

void NetClient::bytesWritten(qint64 bytes)
{

}

void NetClient::readyRead()
{
    QByteArray readedData = socket->readAll();
    emit sgDataRecieved(readedData);
}
