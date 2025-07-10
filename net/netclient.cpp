#include <QNetworkInterface>

#include "netclient.h"

#ifdef __ANDROID__
#include "androidutils.h"
#endif

NetClient::NetClient(QObject *parent)
    : QObject{parent}
{
#ifdef __ANDROID__
    // AndroidUtils::bindProcessToNetwork(); //to WIFI
#endif

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected,this, &NetClient::connected);
    connect(socket, &QTcpSocket::disconnected,this, &NetClient::disconnected);
    connect(socket, &QTcpSocket::bytesWritten,this, &NetClient::bytesWritten);
    connect(socket, &QTcpSocket::readyRead,this, &NetClient::readyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &NetClient::errorOccured);

    socket->connectToHost(targetAddress, targetPort);
    //socket->waitForConnected(-1);
}

void NetClient::doConnect()
{

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
    emit sgConnected();
}

void NetClient::disconnected()
{
    disconnect(this);
    emit sgDisconnected();
}

void NetClient::bytesWritten(qint64 bytes)
{

}

void NetClient::readyRead()
{
    QByteArray readedData = socket->readAll();
    emit sgDataRecieved(readedData);
}

void NetClient::errorOccured(QAbstractSocket::SocketError socketError)
{
    qDebug() << __FUNCTION__ << socketError;
    //disconnect(this);
}
