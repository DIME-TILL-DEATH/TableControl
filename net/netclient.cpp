#include <QNetworkInterface>

#include "netclient.h"

#ifdef __ANDROID__
#include "androidutils.h"
#endif

NetClient::NetClient(QObject *parent)
    : QObject{parent}
{
#ifdef __ANDROID__
    AndroidUtils::bindProcessToNetwork(); //to WIFI
#endif

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected,this, &NetClient::connected);
    connect(socket, &QTcpSocket::disconnected,this, &NetClient::disconnected);
    connect(socket, &QTcpSocket::bytesWritten,this, &NetClient::bytesWritten);
    connect(socket, &QTcpSocket::readyRead,this, &NetClient::readyRead);


    QList<QNetworkAddressEntry> addresses;
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    QString itemName;
    QList<QNetworkInterface>::Iterator i;
    QList<QNetworkAddressEntry>::Iterator a;
    for(i=interfaces.begin(); i!=interfaces.end(); i++)
    {
        addresses = (*i).addressEntries();
        for(a=addresses.begin(); a!=addresses.end(); a++)
        {
            if((*a).ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                itemName = (*i).name() + "::"+(*a).ip().toString();
                // m_itfList.insert(itemName, (*a).ip());
                // namesList.append(itemName);
                //qDebug() << itemName;

                if((*i).name() ==  "wlan0")
                {

                    // TODO для Android'а вызывать из Java метод bindProcessToNetwork
                   bool result = socket->bind(a->ip(), targetPort);

                    qDebug() << "Binding result:" << result;

                   //qintptr socketDescriptor = socket->socketDescriptor();
                }
            }
        }
    }
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
