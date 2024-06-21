#ifndef FIRMWAREMANAGER_H
#define FIRMWAREMANAGER_H

#include <QObject>

#include "netmanager.h"

#ifdef __ANDROID__
#include <jni.h>
#include "activityresultmanager.h"
#endif

struct FirmwareVersion
{
    quint8 major;
    quint8 minor;
};

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

    static bool isVerisonSufficient(QString versionString);

signals:
    void sgUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);

    void sgOpenPlatformFileDialog();
    void sgFirmwareVersionInsufficient();

    void currentFwVersionChanged();

public slots:
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);

    void slAndroidFilePicked(QString filePath, QString fileName);

private:
    QString m_currentFwVersion;
    static FirmwareVersion extractFirmwareVersion(QString versionString);

#ifdef __ANDROID__
    ActivityResultManager activityResultHandler;
#endif
};

#endif // FIRMWAREMANAGER_H
