#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>
#include <QTimer>

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

public slots:
    void slSendRequest(FrameType frameType, uint8_t request,
                        uint32_t data0 = 0,
                        uint32_t data1 = 0,
                        uint32_t parameters = 0);

    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);

    void slDeviceConnected();
    void slDeviceDisconnected();

    void slNetEvent(NetEvents eventType, QString target, QVariantList data = {});

private:
    QTimer* updateDataTimer;

    void getTableBaseData();
    void checkDataUpdate();
};

#endif // REQUESTMANAGER_H
