#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>
#include <QTimer>

#include "frames.h"
#include "netclient.h"

#include "abstractmessage.h"

enum class NetEvents
{
    StartUploadData,
    UploadDataCompleted,
    UploadDataError,
    UploadFirmwareStart,
    UploadFirmware,
    UploadFirmwareError,
    UpdatingFirmwareFinished,
    UpdatingFirmwareError,
    PlaylistRequested,
    PlaylistRecieved,
    FileRequested,
    FileRecieved
};

class NetManager : public QObject
{
    Q_OBJECT
public:
    explicit NetManager(QObject *parent = nullptr);

signals:
    void sgDeviceConnected();
    void sgDeviceDisconnected();

    void sgNetEvent(NetEvents eventType, QString target, QVariantList data = {});

    void sgRecievingMessage(std::shared_ptr<AbstractMessage> msg_ptr);

public slots:
    void slSendMessage(std::shared_ptr<AbstractMessage> msg_ptr);

private:
    FrameHeader lastRecvFrameHeader;
    QByteArray lastRecvFrame;
    QByteArray txBuffer;

    NetClient* netClient;

    QTimer* timer;
    void timerEvent();
    void requestProgress();
    void requestPlaylist();
    
    void processRecievedData(QByteArray data);

    AbstractMessage* processHardwareAnswer();
    AbstractMessage* processPlaylistAnswer();
    AbstractMessage* processFileAnswer();
    AbstractMessage* processFirmwareAnswer();
};

#endif // NETMANAGER_H
