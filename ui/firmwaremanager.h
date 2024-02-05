#ifndef FIRMWAREMANAGER_H
#define FIRMWAREMANAGER_H

#include <QObject>

#include "netmanager.h"

#ifdef __ANDROID__
#include <jni.h>
#include "activityresultmanager.h"
#endif

class FirmwareManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentFwVersion READ currentFwVersion WRITE setCurrentFwVersion NOTIFY currentFwVersionChanged FINAL)
public:
    explicit FirmwareManager(NetManager* netManager, QObject *parent = nullptr);

    Q_INVOKABLE void selectFile();
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

    void sgOpenPlatformFileDialog();

public slots:
    void slDeviceAvaliable();
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);

    void slAndroidFilePicked(QString filePath, QString fileName);

private:
    QString m_currentFwVersion;

#ifdef __ANDROID__
    ActivityResultManager activityResultHandler;
#endif
};

#endif // FIRMWAREMANAGER_H
