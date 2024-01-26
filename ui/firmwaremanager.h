#ifndef FIRMWAREMANAGER_H
#define FIRMWAREMANAGER_H

#include "netmanager.h"
#include <QObject>

class FirmwareManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentFwVersion READ currentFwVersion WRITE setCurrentFwVersion NOTIFY currentFwVersionChanged FINAL)
public:
    explicit FirmwareManager(NetManager* netManager, QObject *parent = nullptr);

    Q_INVOKABLE void updateFirmware(QString filePath);

    QString currentFwVersion() const;
    void setCurrentFwVersion(const QString &newCurrentFwVersion);

signals:
    void sgUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);
    void sgRequest(FrameType frameType, uint8_t requestType,
                   uint32_t data0 = 0,
                   uint32_t data1 = 0,
                   uint32_t parameters = 0);

    void currentFwVersionChanged();
private:
    QString m_currentFwVersion;
};

#endif // FIRMWAREMANAGER_H
