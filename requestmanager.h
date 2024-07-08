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
    void sgNetRequest(FrameType frameType, uint8_t request,
                     uint32_t data0 = 0,
                     uint32_t data1 = 0,
                     uint32_t parameters = 0);

    void sgUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);

    void sgTableAvaliable();
    void sgTableUnavaliable();

    void sgSendMessage(std::shared_ptr<AbstractMessage> msg_ptr);

public slots:
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);

    void slDeviceConnected();
    void slDeviceDisconnected();

    void slNetEvent(NetEvents eventType, QString target, QVariantList data = {});

    void requestParameter(Requests::Hardware request);
    void requestParameter(Requests::Playlist request);
    void requestParameter(Requests::Firmware request);

    void setHardwareParameter(Requests::Hardware request, uint32_t value);
    void setHardwareParameter(Requests::Hardware request, float_t value);

private:
    QTimer* updateDataTimer;

    void getTableBaseData();
    void checkDataUpdate();
};

#endif // REQUESTMANAGER_H
