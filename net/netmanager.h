#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>
#include <QTimer>


#include "requestactions.h"
#include "frames.h"
#include "netclient.h"

#include "abstractmessage.h"

class FileManager;

enum class NetEvents
{
    UploadData,
    UploadDataCompleted,
    UploadDataError,
    UploadFirmware,
    UploadFirmwareError,
    UpdatingFirmware,
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
    explicit NetManager(FileManager* fileManager, QObject *parent = nullptr);

signals:
    void sgDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);

    void sgDeviceConnected();
    void sgDeviceDisconnected();

    void sgNetEvent(NetEvents eventType, QString target, QVariantList data = {});

    void sgRecievingMessage(std::shared_ptr<AbstractMessage> msg_ptr);

public slots:
    void sendNetRequest(FrameType frameType, uint8_t request,
                                            uint32_t data0 = 0,
                                            uint32_t data1 = 0,
                                            uint32_t parameters = 0);

    void slUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);

    void slSendMessage(std::shared_ptr<AbstractMessage> msg_ptr);

private slots:
    void processRecievedData(QByteArray data);
    void sendHardwareData(const QVariantList& data, FrameHeader_uni frameHeader);
    void sendPlaylistData(const QVariantList& data, FrameHeader_uni frameHeader);
    void sendFileData(const QVariantList &data, FrameHeader_uni frameHeader);
    void sendFirmwareData(const QVariantList &data, FrameHeader_uni frameHeader);

private:
    FrameHeader lastRecvFrameHeader;
    QByteArray lastRecvFrame;
    QByteArray txBuffer;

    NetClient* netClient;
    FileManager* fileManager;

    QTimer* timer;
    void timerEvent();
    void requestProgress();
    void requestPlaylist();
    
    AbstractMessage* processHardwareAnswer();
    AbstractMessage* processPlaylistAnswer();
    AbstractMessage* processFileAnswer();
    AbstractMessage* processFirmwareAnswer();

    void updateFileUploadProgress(NetEvents type, QString filePath, quint64 currentPartSize, quint64 totalSize);
    void updateFirmwareUploadProgress(QString filePath, quint64 current, quint64 total);
};

#endif // NETMANAGER_H
