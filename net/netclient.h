#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>

#include "qtcpsocket.h"

class NetClient : public QObject
{
    Q_OBJECT
public:
    explicit NetClient(QObject *parent = nullptr);

    void doConnect();
    void sendData(const QByteArray& data);

    bool isSocketReady();
signals:
    void sgDataRecieved(QByteArray data);

    void sgConnected();
    void sgDisconnected();

private slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void errorOccured(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* socket;

    QHostAddress targetAddress{"192.168.1.1"};
    quint16 targetPort{9000};
};

#endif // NETCLIENT_H
