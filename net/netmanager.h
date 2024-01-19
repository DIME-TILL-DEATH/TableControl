#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>
#include <QTimer>

#include "requestactions.h"
#include "frames.h"
#include "netclient.h"

class NetManager : public QObject
{
    Q_OBJECT
public:
    explicit NetManager(QObject *parent = nullptr);

signals:
    void sgTransportDataUpdated(Data::Transport dataType, QVariantList dataList);
    void sgPlaylistDataUpdated(Data::Playlist dataType, QVariantList dataList);
    void sgContentDataUpdated(Data::File dataType, QVariantList dataList);

    void sgDeviceConnected();
    void sgDeviceDisconnected();

public slots:
    void sendRequest(FrameType frameType, uint8_t requestType,
                                            uint32_t data0 = 0,
                                            uint32_t data1 = 0,
                                            uint32_t parameters = 0);

    void updateData(FrameType frameType, uint8_t dataType, QVariantList data);

private slots:
    void processRecievedData(QByteArray data);
    QByteArray formUpdatedTransportData(uint8_t dataType, QVariantList data);
    QByteArray formUpdatedPlaylistData(uint8_t dataType, QVariantList data);
    QByteArray formUpdatedFileData(uint8_t dataType, QVariantList data);

private:
    FrameHeader lastRecvFrameHeader;
    QByteArray lastRecvFrame;
    QByteArray txBuffer;

    NetClient* netClient;

    QTimer* timer;
    void timerEvent();
    void requestProgress();
    void requestPlaylist();
    
    void processTransportAnswer();
    void processPlaylistAnswer();
    void processFileAnswer();

};

#endif // NETMANAGER_H
