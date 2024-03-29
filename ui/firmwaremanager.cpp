#include "firmwaremanager.h"

#ifdef __ANDROID__
#include <jni.h>
#include "androidutils.h"
#endif

FirmwareManager::FirmwareManager(NetManager* netManager, QObject *parent)
    : QObject{parent}
{

    QObject::connect(netManager, &NetManager::sgDeviceConnected, this, &FirmwareManager::slDeviceAvaliable);
    QObject::connect(netManager, &NetManager::sgDataUpdated, this, &FirmwareManager::slDataUpdated);

    QObject::connect(this, &FirmwareManager::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(this, &FirmwareManager::sgUpdateData, netManager, &NetManager::slUpdateData);

#ifdef Q_OS_ANDROID
    QObject::connect(&activityResultHandler, &ActivityResultManager::sgFilePicked, this, &FirmwareManager::slAndroidFilePicked);
#endif
}

void FirmwareManager::selectFile()
{
#ifdef Q_OS_ANDROID
    AndroidUtils::pickFile(ActivityType::PICK_FILE, "*/*", &activityResultHandler);
#else
    emit sgOpenPlatformFileDialog();
    qDebug() << "emiting";
#endif
}

void FirmwareManager::slAndroidFilePicked(QString filePath, QString fileName)
{
    updateFirmware(filePath);
}

void FirmwareManager::updateFirmware(QString filePath)
{   
    QVariantList data;
    data.append(filePath);
    emit sgUpdateData(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::FIRMWARE_UPLOAD_START, data);
}

QString FirmwareManager::currentFwVersion() const
{
    return m_currentFwVersion;
}

void FirmwareManager::setCurrentFwVersion(const QString &newCurrentFwVersion)
{
    if (m_currentFwVersion == newCurrentFwVersion)
        return;
    m_currentFwVersion = newCurrentFwVersion;
    emit currentFwVersionChanged();
}

void FirmwareManager::slDeviceAvaliable()
{
    emit sgRequest(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::FIRMWARE_VERSION);
}

void FirmwareManager::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
{
    if(frameType != FrameType::FIRMWARE_ACTIONS) return;

    switch((Data::Firmware)dataType)
    {
    case Data::Firmware::FIRMWARE_VERSION:
    {
        setCurrentFwVersion(data.at(0).toString());
        break;
    }
    }
}
