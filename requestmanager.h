#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>
#include <QTimer>

#include "abstractmessage.h"

#include "requestactions.h"
#include "netmanager.h"

class RequestManager : public QObject
{
    Q_OBJECT
public:
    explicit RequestManager(QObject *parent = nullptr);

signals:
    void sgTableAvaliable();
    void sgTableUnavaliable();

    void sgSendMessage(std::shared_ptr<AbstractMessage> msg_ptr);
    void sgNetEvent(NetEvents eventType, QString target, QVariantList data = {});

public slots:
    void slDeviceConnected();
    void slDeviceDisconnected();

    void slSerialIdRecieved(QString serialId);
    void slFirmwareVerisonRecieved(QString firmwareVersion);

    void requestParameter(Requests::Hardware request);
    void requestParameter(Requests::Playlist request);
    void requestParameter(Requests::Firmware request);

    void requestFile(QString filePath);
    void requestFolder(QString folderPath);

    void uploadFile(QString dstPath, QString srcPath);
    void updateFirmware(QString firmwarePath);

    void setParameter(Requests::Hardware request, uint32_t value);
    void setParameter(Requests::Hardware request, float_t value);
    void setParameter(Requests::Playlist request, uint32_t value);
    void setParameter(Requests::Playlist request, QString string);

private:
    QTimer* updateDataTimer;

    void getTableBaseData();
    void checkDataUpdate();

    void updateFileUploadProgress(NetEvents type, QString filePath, quint64 currentPartSize, quint64 totalSize);
};

#endif // REQUESTMANAGER_H
