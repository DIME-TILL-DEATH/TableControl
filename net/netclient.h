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

private slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

private:
    QTcpSocket* socket;

    QString targetAddress{"192.168.1.1"};
    quint16 targetPort{3333};
};

#endif // NETCLIENT_H
